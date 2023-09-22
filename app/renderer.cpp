#include "renderer.h"

#include "core/logger.h"
#include "platform/platform.h"

#include "imgui_stdlib.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <ranges>
#include <sstream>


namespace RCalc {

const float STACK_HORIZ_PADDING = 8.0;
const float STACK_SCROLLBAR_COMPENSATION = 16.0;

void Renderer::render(std::vector<RenderItem>& items) {
    Platform& platform = Platform::get_singleton();

    // Fullscreen window
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    if (!ImGui::Begin(
        "RPN Calculator",
        nullptr,
        ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_MenuBar
    )) {
        ImGui::End();
        Logger::log_err("Failed to render window!");
        return;
    }

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Copy Answer", "Ctrl+C", &last_entry_needs_copy);
            ImGui::MenuItem("Quit", "Ctrl+Q", &platform.close_requested);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    ImVec2 window_size = ImGui::GetWindowSize();
    if (window_size.x != last_window_size.x || window_size.y != last_window_size.y) {
        Logger::log_info("Resizing window: %fx%f", window_size.x, window_size.y);

        for (RenderItem& item : items) {
            item.recalculate_size();
        }
    }
    last_window_size = window_size;

    // Calculate heights
    float desired_stack_height = 0.0;
    if (!items.empty()) {
        for (const RenderItem& item : items) {
            desired_stack_height += item.display_height;
        }

        desired_stack_height += ImGui::GetStyle().ItemSpacing.y * (items.size() - 1);
    }

    const float input_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    const float max_stack_height = ImGui::GetContentRegionAvail().y - input_height - ImGui::GetStyle().ItemSpacing.y;

    const float stack_height = std::min(max_stack_height, desired_stack_height);
    const float stack_offset = max_stack_height - stack_height;
    const float separator_size = stack_offset;

    ImGui::BeginChild("##separator", ImVec2(0, separator_size));
    ImGui::EndChild();

    if (!items.empty()) {
        if (ImGui::BeginChild("##stack", ImVec2(0, stack_height))) {
            int index = 0;

            for (const RenderItem& item : items) {

                ImGui::PushTextWrapPos(item.input_display_width);
                ImGui::TextUnformatted(item.input.data());
                ImGui::PopTextWrapPos();

                ImGui::SameLine(item.input_display_width, STACK_HORIZ_PADDING);
                ImGui::PushTextWrapPos(item.input_display_width + STACK_HORIZ_PADDING + item.output_display_width);
                ImGui::TextUnformatted(item.output.data());
                ImGui::PopTextWrapPos();

                std::string item_id = std::format("##item-{:d}", index++); // TODO: Move away from format for compatibility with apple clang (unless sonoma fixes that?)
                if (ImGui::BeginPopupContextItem(item_id.c_str())) {
                    if (ImGui::Button("Copy to Clipboard")) {
                        platform.copy_to_clipboard(item.output);
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
            }
        }

        if (stack_needs_scroll_down) {
            ImGui::SetScrollHereY(1.0);
            stack_needs_scroll_down = false;
        }

        ImGui::EndChild();
    }

    ImGui::Separator();

    // Scratchpad

    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputText(
        "##input",
        &scratchpad,
        ImGuiInputTextFlags_EnterReturnsTrue
        | ImGuiInputTextFlags_EscapeClearsAll)
    ) {
        scratchpad = "";
        scratchpad_needs_focus = true;
        stack_needs_scroll_down = true;
    }

    ImGui::SetItemDefaultFocus();
    
    if (scratchpad_needs_focus) {
        ImGui::SetKeyboardFocusHere(-1);
        scratchpad_needs_focus = false;
    }

    ImGui::End(); // Window

    // Handle shortcuts
    if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_Q)) {
        platform.close_requested = true;
    }
    if (last_entry_needs_copy || (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_C))) {
        last_entry_needs_copy = false;
        if (!items.empty()) {
            platform.copy_to_clipboard(items.back().output);
        }
    }
}


void Renderer::display_info(const std::string& str) {
    message = str;
    message_is_error = false;
}


void Renderer::display_error(const std::string& str) {
    message = str;
    message_is_error = true;
}


void RenderItem::recalculate_size() {
    float available_width = ImGui::GetContentRegionAvail().x - STACK_HORIZ_PADDING - STACK_SCROLLBAR_COMPENSATION;

    float output_max_size = available_width / 2.0;
    float output_desired_size = ImGui::CalcTextSize(output.data()).x;
    output_display_width = std::min(output_max_size, output_desired_size);
    ImVec2 output_actual_size = ImGui::CalcTextSize(output.data(), nullptr, false, output_display_width);

    input_display_width = available_width - output_actual_size.x;
    ImVec2 input_actual_size = ImGui::CalcTextSize(input.data(), nullptr, false, input_display_width);
    display_height = std::max(output_actual_size.y, input_actual_size.y);
}

}