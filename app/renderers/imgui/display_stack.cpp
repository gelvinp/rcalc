#include "display_stack.h"
#include "constants.h"

#include <algorithm>
#include <numeric>


namespace RCalc {

using namespace ImGuiRendererConstants;


ImGuiDisplayEntry::ImGuiDisplayEntry(const StackItem& item, std::optional<int> precision) {
    for (Displayable& disp : *item.p_input) {
        std::string str;

        switch (disp.get_type()) {
            case Displayable::Type::CONST_CHAR: {
                str = reinterpret_cast<ConstCharDisplayable&>(disp).p_char;
                break;
            }
            case Displayable::Type::STRING: {
                str = reinterpret_cast<StringDisplayable&>(disp).str;
                break;
            }
            case Displayable::Type::VALUE: {
                str = reinterpret_cast<ValueDisplayable&>(disp).value.to_string(disp.tags, precision);
                break;
            }
            case Displayable::Type::RECURSIVE:
                UNREACHABLE(); // Handled by the iterator
        }

        input.chunks.emplace_back(str);
    }

    output.str = item.result.to_string(item.p_input->tags);
    result = item.result;
}



void ImGuiDisplayChunk::calculate_size(float max_width) {
    size = ImGui::CalcTextSize(str.c_str(), nullptr, false, max_width);
    position = ImVec2(0, 0);
}

void ImGuiDisplayLine::calculate_size(float max_width) {
    size = ImVec2(0, 0);

    // Step 1: Calculate sizes

    float available_width = max_width;
    float line_no = 0.1f; // Avoid any potential floating point weirdness

    for (ImGuiDisplayChunk& chunk : chunks) {
        chunk.calculate_size();
    }

    if (max_width > 0.0f) {
        for (size_t chunk_idx = 0; chunk_idx < chunks.size(); ++chunk_idx) {
            ImGuiDisplayChunk& chunk = chunks[chunk_idx];

            // Check for wrap
            bool chunk_overflows = chunk.size.x > available_width;

            bool chunk_is_short = chunk.size.x < 10;
            bool chunk_not_last = (chunk_idx + 1) < chunks.size();
            bool short_chunk_plus_next_overflows =
            (
                (chunk_is_short && chunk_not_last) &&
                (chunk.size.x + chunks[chunk_idx + 1].size.x) > available_width
            );

            if (chunk_overflows || short_chunk_plus_next_overflows) {
                line_no += 1;
                available_width = max_width;
            }

            chunk.calculate_size(available_width);
            chunk.position.y = line_no; // Encode wrapped lines in the position
            available_width -= chunk.size.x;
        }
    }

    // Step 2: Position chunks
    size_t line_count = static_cast<size_t>(line_no); // Truncate
    auto from_iter = chunks.begin();

    for (size_t line_idx = 0; line_idx <= line_count; line_idx++) {
        // Find all chunks on line
        auto to_iter = std::partition_point(
            from_iter,
            chunks.end(),
            [line_idx](ImGuiDisplayChunk& chunk){ return line_idx == static_cast<size_t>(chunk.position.y); }
        );

        // Get max height of line
        float line_height = std::accumulate(
            from_iter,
            to_iter,
            0.0f,
            [](float acc, ImGuiDisplayChunk& chunk) { return std::max(acc, chunk.size.y); }
        );

        // Get total width of line
        float line_width = std::accumulate(
            from_iter,
            to_iter,
            0.0f,
            [](float acc, ImGuiDisplayChunk& chunk) { return acc + chunk.size.x; }
        );

        // Position each chunk in the line
        float chunk_start_x = 0.0f;
        for (auto iter = from_iter; iter < to_iter; ++iter) {
            iter->position = ImVec2(
                chunk_start_x,                                  // Left align
                size.y + ((line_height - iter->size.y) / 2.0f)   // Center align
            );

            chunk_start_x += iter->size.x;
        }

        size = ImVec2(
            std::max(size.x, line_width),
            size.y + line_height
        );

        from_iter = to_iter;
    }
}

void ImGuiDisplayEntry::calculate_size(float max_width, bool scrollbar_visible, bool bottom_align) {
    float available_width = 0.0f;

    if (max_width < 0.0f) { /* Help page example */
        input.calculate_size();
        output.calculate_size();
        available_width = output.size.x;
    }
    else {
        available_width = max_width - STACK_HORIZ_BIAS - (2.0f * STACK_OUTER_PADDING);
        if (scrollbar_visible) {
            available_width -= ImGui::GetStyle().ScrollbarSize;
        }

        float output_max_width = available_width / 2.0f - STACK_HORIZ_PADDING;
        output.calculate_size(output_max_width);

        float input_max_width = available_width - output.size.x - STACK_HORIZ_PADDING;
        input.calculate_size(input_max_width);
    }
        
    height = std::max(input.size.y, output.size.y);

    // Final positioning
    if (input.size.y < height) {
        // Move input chunks down
        float diff = height - input.size.y;
        if (!bottom_align) { diff /= 2.0f; }

        for (ImGuiDisplayChunk& chunk : input.chunks) {
            chunk.position.y += diff;
        }
    }
    else {
        // Move output chunk down
        float diff = height - output.size.y;
        if (!bottom_align) { diff /= 2.0f; }
        output.position.y += diff;
    }

    output.position.x = available_width - output.size.x;
    valid = true;
}

void ImGuiDisplayStack::calculate_sizes(float max_width, bool scrollbar_visible, bool bottom_align) {
    for (ImGuiDisplayEntry& entry : entries) {
        if (entry.valid) { continue; }
        entry.calculate_size(max_width, scrollbar_visible, bottom_align);
    }
}

void ImGuiDisplayStack::invalidate_sizes() {
    for (ImGuiDisplayEntry& entry : entries) {
        entry.valid = false;
    }
}

}