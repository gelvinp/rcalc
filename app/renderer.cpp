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

Renderer::Renderer(SubmitTextCallback cb_submit_text, SubmitOperatorCallback cb_submit_op) :
    cb_submit_text(cb_submit_text), cb_submit_op(cb_submit_op) {
        command_map = get_command_map<Renderer>();
}

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
        for (RenderItem& item : items) {
            item.recalculate_size();
        }
    } else {
        for (RenderItem& item : items) {
            if (item.input_display_width < 0.001) { item.recalculate_size(); }
        }
    }
    last_window_size = window_size;

    // Calculate heights
    const auto [window_width, window_height] = ImGui::GetContentRegionMax();
    const float padding = ImGui::GetStyle().ItemSpacing.y;

    const float input_height = ImGui::GetFrameHeight();
    const float input_position = window_height - input_height;

    const float message_height = message.empty() ? 0.0 : ImGui::CalcTextSize(message.c_str(), nullptr, false, window_width).y;
    const float message_padding = message.empty() ? 0.0 : padding;
    const float message_position = input_position - message_height - padding;

    const float separator_position = message_position - message_padding;

    const float menu_bar_height = ImGui::GetCursorPosY();
    const float available_stack_height = window_height - input_height - message_height - (padding * 3.0f) - message_padding - menu_bar_height;

    float desired_stack_height = 0.0;
    if (!items.empty()) {
        for (const RenderItem& item : items) {
            desired_stack_height += item.display_height;
        }

        desired_stack_height += ImGui::GetStyle().ItemSpacing.y * (items.size() - 1);
    }

    const float stack_height = std::min(available_stack_height, desired_stack_height);
    const float stack_position = separator_position - stack_height - padding;

    if (!items.empty()) {
        ImGui::SetCursorPosY(stack_position);

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

    // Separator

    ImGui::SetCursorPosY(separator_position);
    ImGui::Separator();

    // Message

    if (!message.empty()) {
        ImGui::SetCursorPosY(message_position);
        
        if (message_is_error) {
            ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.0f, 0.0f, 1.0f});
        }

        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextUnformatted(message.c_str());
        
        if (message_is_error) {
            ImGui::PopStyleColor();
        }
    }

    // Scratchpad

    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::SetCursorPosY(input_position);
    if (ImGui::InputText(
        "##input",
        (char*)scratchpad.c_str(),
        scratchpad.capacity() + 1,
        ImGuiInputTextFlags_EnterReturnsTrue
        | ImGuiInputTextFlags_EscapeClearsAll
        | ImGuiInputTextFlags_CallbackCharFilter
        | ImGuiInputTextFlags_CallbackResize
        | ImGuiInputTextFlags_CallbackAlways,
        &scratchpad_input_callback,
        (void*)this)
    ) {
        submit_scratchpad();
    }

    ImGui::SetItemDefaultFocus();
    ImGui::SetKeyboardFocusHere(-1);

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


void Renderer::submit_scratchpad() {
    message = "";
    std::transform(scratchpad.begin(), scratchpad.end(), scratchpad.begin(), [](unsigned char c){ return std::tolower(c); });
    cb_submit_text(scratchpad);
    scratchpad.clear();
    stack_needs_scroll_down = true;
}


int Renderer::scratchpad_input_callback(ImGuiInputTextCallbackData* p_cb_data) {
    switch (p_cb_data->EventFlag) {
        case ImGuiInputTextFlags_CallbackCharFilter:
            return scratchpad_input_filter_callback(p_cb_data);
        case ImGuiInputTextFlags_CallbackResize:
            return scratchpad_input_resize_callback(p_cb_data);
        case ImGuiInputTextFlags_CallbackAlways:
            return scratchpad_input_always_callback(p_cb_data);
        default:
            return 0;
    }
}


int Renderer::scratchpad_input_filter_callback(ImGuiInputTextCallbackData* p_cb_data) {
    Renderer* self = (Renderer*)p_cb_data->UserData;

    // Check for basic arithmetic operators
    switch ((char)p_cb_data->EventChar) {
        case '+':
            if (!self->scratchpad.empty()) {
                self->submit_scratchpad();
                self->scratchpad_needs_clear = true;
            }
            self->cb_submit_op("add");
            return 1;
        case '-':
            if (!self->scratchpad.empty()) {
                self->submit_scratchpad();
                self->scratchpad_needs_clear = true;
            }
            self->cb_submit_op("sub");
            return 1;
        case '*':
            if (!self->scratchpad.empty()) {
                self->submit_scratchpad();
                self->scratchpad_needs_clear = true;
            }
            self->cb_submit_op("mul");
            return 1;
        case '/':
            if (!self->scratchpad.empty()) {
                self->submit_scratchpad();
                self->scratchpad_needs_clear = true;
            }
            self->cb_submit_op("div");
            return 1;
        default:
            return 0;
    }
}


int Renderer::scratchpad_input_resize_callback(ImGuiInputTextCallbackData* p_cb_data) {
    Renderer* self = (Renderer*)p_cb_data->UserData;
    self->scratchpad.resize(p_cb_data->BufTextLen);
    p_cb_data->Buf = (char*)self->scratchpad.c_str();
    return 0;
}


int Renderer::scratchpad_input_always_callback(ImGuiInputTextCallbackData* p_cb_data) {
    Renderer* self = (Renderer*)p_cb_data->UserData;
    if (!self->scratchpad_needs_clear) { return 0; }
    p_cb_data->DeleteChars(0, p_cb_data->BufTextLen);
    self->scratchpad_needs_clear = false;
    return 0;
}


bool Renderer::try_renderer_command(const std::string& str) {
    if (command_map.contains(str)) {
        command_map.at(str)->execute(*this);
        return true;
    }

    return false;
}


void RenderItem::recalculate_size() {
    float available_width = ImGui::GetContentRegionMax().x - STACK_HORIZ_PADDING - STACK_SCROLLBAR_COMPENSATION;

    float output_max_size = available_width / 2.0;
    float output_desired_size = ImGui::CalcTextSize(output.data(), nullptr, false, -1).x;
    output_display_width = std::min(output_max_size, output_desired_size);
    ImVec2 output_actual_size = ImGui::CalcTextSize(output.data(), nullptr, false, output_display_width);

    input_display_width = available_width - output_actual_size.x;
    ImVec2 input_actual_size = ImGui::CalcTextSize(input.data(), nullptr, false, input_display_width);
    display_height = std::max(output_actual_size.y, input_actual_size.y);
}

}