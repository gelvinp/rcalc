#include "imgui_renderer.h"

#include "core/logger.h"
#include "core/version.h"
#include "core/help_text.h"
#include "assets/B612Mono-Regular.ttf.gen.h"
#include "assets/license.gen.h"
#include "core/filter.h"
#include "core/format.h"

#include "imgui_ext.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <ranges>
#include <sstream>
#include <set>


namespace RCalc {

const float STACK_HORIZ_PADDING = 16.0;
const float STACK_HORIZ_BIAS = 8.0;
const float STACK_OUTER_PADDING = 4.0;
const float STACK_BOTTOM_PADDING = 8.0;

constexpr ImVec4 COLORS[] = {
    { 1.0, 0.239, 0.239, 1.0 },     // Red
    { 1.0, 0.619, 0.329, 1.0 },   // Orange
    { 1.0, 0.909, 0.388, 1.0 },     // Yellow
    { 0.439, 1.0, 0.388, 1.0 },     // Green
    { 0.412, 0.655, 1.0, 1.0 },     // Blue
    { 0.698, 0.451, 1.0, 1.0 },     // Purple
    { 1.0, 1.0, 1.0, 1.0 },         // White
    { 1.0, 0.561, 0.847, 1.0 },     // Pink
    { 0.309, 0.898, 1.0, 1.0 },     // Light Blue
    { 0.361, 0.196, 0.039, 1.0 },   // Brown
    { 0.0, 0.0, 0.0, 1.0 },         // Black
    { 0.8, 0.8, 0.8, 1.0 },         // Gray
};

enum ColorNames : int {
    COLOR_RED,
    COLOR_ORANGE,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_PURPLE,
    COLOR_WHITE,
    COLOR_PINK,
    COLOR_LIGHT_BLUE,
    COLOR_BROWN,
    COLOR_BLACK,
    COLOR_GRAY
};

ImGuiRenderer::ImGuiRenderer(RendererCreateInfo info) :
        cb_submit_text(info.cb_submit_text),
        cb_submit_op(info.cb_submit_op)
{
    command_map = CommandMap<ImGuiRenderer>::get_command_map();

    // Init backend
    p_backend = RenderBackend::create<ImGuiRenderer>();
}


Result<> ImGuiRenderer::init(Application* p_application) {
    Result<> res = p_backend->init(p_application);

    // Load font
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;

    glyphs.AddRanges(io.Fonts->GetGlyphRangesDefault());
    glyphs.AddText("⌈⌉⌊⌋°πτ·×θφ");
    glyphs.BuildRanges(&glyph_ranges);

    float screen_dpi = p_backend->get_screen_dpi();
    float font_size_standard = std::floor(14 * screen_dpi);
    float font_size_medium = std::floor(18 * screen_dpi);
    float font_size_large = std::floor(24 * screen_dpi);

    p_font_standard = io.Fonts->AddFontFromMemoryTTF((void*)RCalc::Assets::b612mono_regular_ttf, RCalc::Assets::b612mono_regular_ttf_size, font_size_standard, &font_cfg, &glyph_ranges[0]);
    p_font_medium = io.Fonts->AddFontFromMemoryTTF((void*)RCalc::Assets::b612mono_regular_ttf, RCalc::Assets::b612mono_regular_ttf_size, font_size_medium, &font_cfg, &glyph_ranges[0]);
    p_font_large = io.Fonts->AddFontFromMemoryTTF((void*)RCalc::Assets::b612mono_regular_ttf, RCalc::Assets::b612mono_regular_ttf_size, font_size_large, &font_cfg, &glyph_ranges[0]);

    io.FontGlobalScale = 1.0f / screen_dpi;

    return Ok();
}


void ImGuiRenderer::render() {
    p_backend->start_frame();

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
    )) {
        ImGui::End();
        Logger::log_err("Failed to render window!");
        return;
    }

    
    if (p_backend->app_menu_bar()) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Copy Answer", "Ctrl+C", &copy_requested);
                ImGui::MenuItem("Duplicate Item", "Ctrl+D", &dup_requested);
                ImGui::MenuItem("Show Help", "F1", &help_requested);
                ImGui::Separator();
                ImGui::MenuItem("Quit", "Ctrl+Q", &p_backend->close_requested);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    
    ImVec2 window_size = ImGui::GetWindowSize();
    if (window_size.x != last_window_size.x || window_size.y != last_window_size.y) {
        display_stack.invalidate_sizes();
    }
    last_window_size = window_size;

    // Calculate heights
    const auto [window_width, window_height] = ImGui::GetWindowContentRegionMax();
    const float padding = ImGui::GetStyle().ItemSpacing.y;

    const float input_height = ImGui::GetFrameHeight();
    const float input_position = window_height - input_height;

    const float message_height = message.empty() ? 0.0 : ImGui::CalcTextSize(message.c_str(), nullptr, false, window_width).y;
    const float message_padding = message.empty() ? 0.0 : padding;
    const float message_position = input_position - message_height - padding;

    const float separator_position = message_position - message_padding;

    const float menu_bar_height = ImGui::GetCursorPosY();
    const float available_stack_height = window_height - input_height - message_height - (padding * 3.0f) - message_padding - menu_bar_height - STACK_BOTTOM_PADDING;

    display_stack.calculate_sizes(window_width, scrollbar_visible);

    float desired_stack_height = 0.0;
    if (!display_stack.entries.empty()) {
        for (const ImGuiDisplayEntry& entry : display_stack.entries) {
            desired_stack_height += entry.height;
        }

        desired_stack_height += ImGui::GetStyle().ItemSpacing.y * (display_stack.entries.size() - 1);
    }

    if ((desired_stack_height >= available_stack_height) && !scrollbar_visible) {
        scrollbar_visible = true;
        desired_stack_height = 0.0;
        if (!display_stack.entries.empty()) {
            for (const ImGuiDisplayEntry& entry : display_stack.entries) {
                desired_stack_height += entry.height;
            }

            desired_stack_height += ImGui::GetStyle().ItemSpacing.y * (display_stack.entries.size() - 1);
        }
    }
    else if ((desired_stack_height < available_stack_height) && scrollbar_visible) {
        scrollbar_visible = false;
        desired_stack_height = 0.0;
        if (!display_stack.entries.empty()) {
            for (const ImGuiDisplayEntry& entry : display_stack.entries) {
                desired_stack_height += entry.height;
            }

            desired_stack_height += ImGui::GetStyle().ItemSpacing.y * (display_stack.entries.size() - 1);
        }
    }

    const float stack_height = std::min(available_stack_height, desired_stack_height);

    const float stack_position = separator_position - stack_height - padding - STACK_BOTTOM_PADDING;

    if (!display_stack.entries.empty()) {
        ImGui::SetCursorPosY(stack_position);

        if (ImGui::BeginChild("##stack", ImVec2(0, stack_height + STACK_BOTTOM_PADDING))) {
            int index = 0;

            for (const ImGuiDisplayEntry& entry : display_stack.entries) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + STACK_OUTER_PADDING);

                if (queer_active) {
                    int color_index = index % COLOR_GRAY;
                    ImGui::PushStyleColor(ImGuiCol_Text, COLORS[color_index]);
                    if (color_index >= COLOR_BROWN) {
                        const float padding = 2.0f;
                        ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
                        ImVec2 input_size = entry.input.size;
                        ImGui::GetWindowDrawList()->AddRectFilled(
                            ImVec2(cursor_pos.x - padding, cursor_pos.y - padding),
                            ImVec2(cursor_pos.x + input_size.x + padding, cursor_pos.y + input_size.y + padding),
                            ImGui::ColorConvertFloat4ToU32(COLORS[COLOR_GRAY])
                            
                        );
                        double output_offset = entry.output.position.x;
                        ImVec2 output_size = entry.output.size;
                        ImGui::GetWindowDrawList()->AddRectFilled(
                            ImVec2(cursor_pos.x + output_offset - padding, cursor_pos.y - padding),
                            ImVec2(cursor_pos.x + output_offset + output_size.x + padding, cursor_pos.y + output_size.y + padding),
                            ImGui::ColorConvertFloat4ToU32(COLORS[COLOR_GRAY])
                        );
                    }
                }

                ImVec2 entry_start_pos = ImGui::GetCursorPos();

                // Input
                for (const ImGuiDisplayChunk& chunk : entry.input.chunks) {
                    ImVec2 chunk_start_pos {
                        entry_start_pos.x + chunk.position.x,
                        entry_start_pos.y + chunk.position.y
                    };

                    ImGui::SetCursorPos(chunk_start_pos);

                    ImGui::PushTextWrapPos(chunk_start_pos.x + chunk.size.x);
                    ImGui::TextUnformatted(chunk.str.c_str());
                    ImGui::PopTextWrapPos();
                }

                // Output
                ImVec2 chunk_start_pos {
                    entry_start_pos.x + entry.output.position.x,
                    entry_start_pos.y + entry.output.position.y
                };

                ImGui::SetCursorPos(chunk_start_pos);

                ImGui::PushTextWrapPos(chunk_start_pos.x + entry.output.size.x);
                ImGui::TextUnformatted(entry.output.str.c_str());
                ImGui::EXT_SetIDForLastItem(index++);
                ImGui::PopTextWrapPos();

                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::Button("Copy to Clipboard")) {
                        p_backend->copy_to_clipboard(entry.output.str);
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                if (queer_active) {
                    ImGui::PopStyleColor();
                }
            }
        }

        // Must do a complicated song and dance because UI is a nightmare
        if (stack_needs_scroll_down && !scratchpad_needs_focus) {
            ImGui::SetScrollHereY(1.0);
            scratchpad_needs_focus = true;
        }

        ImGui::EndChild();
    }

    // Gets stuck when the stack is empty
    if (stack_needs_scroll_down && !scratchpad_needs_focus) {
        scratchpad_needs_focus = true;
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
        | ImGuiInputTextFlags_CallbackAlways
        | ImGuiInputTextFlags_CallbackHistory,
        &scratchpad_input_callback,
        (void*)this)
    ) {
        submit_scratchpad();
    }

    if (scratchpad_needs_focus && !help_requested && !help_open) {
        ImGui::SetKeyboardFocusHere(-1);
        scratchpad_needs_focus = false;
        stack_needs_scroll_down = false;
    }

    if (
        !ImGui::IsAnyItemActive()
        && !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId)
        && !stack_needs_scroll_down
        && !help_requested
        && !help_open
    ) {
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::End(); // Window

    if (help_requested) {
        help_open = true;
        ImGui::OpenPopup("Help");
        help_requested = false;
    }
    render_help();

    // Handle shortcuts
    if (p_backend->app_menu_bar()) {
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Q)) {
            p_backend->close_requested = true;
        }
        if (copy_requested || (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_C))) {
            if (!display_stack.entries.empty()) {
                p_backend->copy_to_clipboard(display_stack.entries.back().output.str);
            }
            copy_requested = false;
        }
        if (dup_requested || (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_D))) {
            cb_submit_text("\\dup");
            dup_requested = false;
        }
        if ((ImGui::IsKeyDown(ImGuiKey_F1))) {
            help_requested = true;
        }
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
        cb_submit_text("\\pop");
    }
    if (ImGui::IsKeyReleased(ImGuiKey_Enter) || ImGui::IsKeyReleased(ImGuiKey_KeypadEnter)) {
        enter_pressed = false;
    }
    if (help_open && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        help_open = false;
    }

    p_backend->render_frame();
}


void ImGuiRenderer::cleanup() {
    p_backend->cleanup();
}


void ImGuiRenderer::display_info(const std::string& str) {
    message = str;
    message_is_error = false;
}


void ImGuiRenderer::display_error(const std::string& str) {
    message = str;
    message_is_error = true;
}


void ImGuiRenderer::submit_scratchpad() {
    if (enter_pressed) { return; }
    enter_pressed = true;

    stack_needs_scroll_down = true;
    message = "";

    if (scratchpad.empty()) {
        cb_submit_text("\\dup");
        return;
    }

    history.push_back(scratchpad);
    history_state = std::nullopt;

    std::transform(scratchpad.begin(), scratchpad.end(), scratchpad.begin(), [](unsigned char c){ return std::tolower(c); });
    cb_submit_text(scratchpad);
    scratchpad.clear();
}


int ImGuiRenderer::scratchpad_input_callback(ImGuiInputTextCallbackData* p_cb_data) {
    switch (p_cb_data->EventFlag) {
        case ImGuiInputTextFlags_CallbackCharFilter:
            return scratchpad_input_filter_callback(p_cb_data);
        case ImGuiInputTextFlags_CallbackResize:
            return scratchpad_input_resize_callback(p_cb_data);
        case ImGuiInputTextFlags_CallbackAlways:
            return scratchpad_input_always_callback(p_cb_data);
        case ImGuiInputTextFlags_CallbackHistory:
            return scratchpad_input_history_callback(p_cb_data);
        default:
            return 0;
    }
}


int ImGuiRenderer::scratchpad_input_filter_callback(ImGuiInputTextCallbackData* p_cb_data) {
    ImGuiRenderer* self = (ImGuiRenderer*)p_cb_data->UserData;

    // Check for basic arithmetic operators
    switch ((char)p_cb_data->EventChar) {
        case '+':
            if (!self->scratchpad.empty()) {
                self->submit_scratchpad();
                self->scratchpad_needs_clear = true;
                self->enter_pressed = false;
            }
            self->cb_submit_op("add");
            return 1;
        case '-':
            if (!self->scratchpad.empty()) {
                self->submit_scratchpad();
                self->scratchpad_needs_clear = true;
                self->enter_pressed = false;
            }
            self->cb_submit_op("sub");
            return 1;
        case '*':
            if (!self->scratchpad.empty()) {
                self->submit_scratchpad();
                self->scratchpad_needs_clear = true;
                self->enter_pressed = false;
            }
            self->cb_submit_op("mul");
            return 1;
        case '/':
            if (!self->scratchpad.empty()) {
                self->submit_scratchpad();
                self->scratchpad_needs_clear = true;
                self->enter_pressed = false;
            }
            self->cb_submit_op("div");
            return 1;
        default:
            return 0;
    }
}


int ImGuiRenderer::scratchpad_input_resize_callback(ImGuiInputTextCallbackData* p_cb_data) {
    ImGuiRenderer* self = (ImGuiRenderer*)p_cb_data->UserData;
    self->scratchpad.resize(p_cb_data->BufTextLen);
    p_cb_data->Buf = (char*)self->scratchpad.c_str();
    return 0;
}


int ImGuiRenderer::scratchpad_input_always_callback(ImGuiInputTextCallbackData* p_cb_data) {
    ImGuiRenderer* self = (ImGuiRenderer*)p_cb_data->UserData;
    if (!self->scratchpad_needs_clear) { return 0; }
    p_cb_data->DeleteChars(0, p_cb_data->BufTextLen);
    self->scratchpad_needs_clear = false;
    return 0;
}


int ImGuiRenderer::scratchpad_input_history_callback(ImGuiInputTextCallbackData* p_cb_data) {
    ImGuiRenderer* self = (ImGuiRenderer*)p_cb_data->UserData;

    switch (p_cb_data->EventKey) {
        case ImGuiKey_UpArrow: {
            size_t next_state;

            if (self->history_state) {
                next_state = self->history_state.value() + 1;
                if (next_state > self->history.size()) { return 0; }
            }
            else if (!self->history.empty()) {
                next_state = 1;
            }
            else { return 0; }

            const std::string& str = *(self->history.end() - next_state);
            p_cb_data->DeleteChars(0, p_cb_data->BufTextLen);
            p_cb_data->InsertChars(0, str.data(), str.data() + str.length());

            self->history_state = next_state;
            return 0;
        }
        case ImGuiKey_DownArrow: {
            std::optional<size_t> next_state;

            if (self->history_state) {
                next_state = self->history_state.value() - 1;
                if (next_state == 0) { next_state = std::nullopt; }
                if (next_state > self->history.size()) /* Shouldn't happen but who knows */ { next_state = std::nullopt; }
            }
            else { return 0; }

            p_cb_data->DeleteChars(0, p_cb_data->BufTextLen);
            
            if (next_state) {
                const std::string& str = *(self->history.end() - next_state.value());
                p_cb_data->InsertChars(0, str.data(), str.data() + str.length());
            }

            self->history_state = next_state;
            return 0;
        }
        default:
            return 0;
    }
}


bool ImGuiRenderer::try_renderer_command(const std::string& str) {
    if (command_map.has_command(str)) {
        command_map.execute(str, *this);
        return true;
    }

    return false;
}


void ImGuiRenderer::render_help() {
    ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
    ImGui::SetNextWindowSize(ImVec2(viewport_size.x * 0.95, viewport_size.y * 0.95), ImGuiCond_Appearing);
    ImGui::SetNextWindowFocus();

    if (!ImGui::BeginPopupModal("Help", &help_open)) { return; }

    ImGui::PushTextWrapPos(0.0);

    ImGui::PushFont(p_font_large);
    ImGui::TextUnformatted("RCalc");
    ImGui::PopFont();

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, COLORS[COLOR_GRAY]);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.0);
    ImGui::TextUnformatted("v" VERSION_FULL_BUILD);
    
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.0);
    ImGui::Text("(%.6s)", VERSION_HASH);

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Click to copy: %s", VERSION_HASH);
    }

    ImGui::PopStyleColor();

    if (ImGui::IsItemClicked()) {
        p_backend->copy_to_clipboard(VERSION_HASH);
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);
    ImGui::TextUnformatted(HelpText::program_info);

    for (const HelpText::HelpSection& section : HelpText::sections) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 24.0);

        ImGui::PushFont(p_font_medium);
        ImGui::TextUnformatted(section.header);
        ImGui::PopFont();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);
        ImGui::TextUnformatted(section.text);
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);

    ImGui::PushFont(p_font_large);
    ImGui::TextUnformatted("Commands");
    ImGui::PopFont();
    
    for (const RCalc::ScopeMeta* scope : command_map.get_alphabetical()) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16.0);
        ImGui::PushFont(p_font_medium);
        ImGui::Text("%s Commands", scope->scope_name);
        ImGui::PopFont();

        for (const RCalc::CommandMeta* cmd : scope->scope_cmds) {
            render_help_command(cmd);
        }
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);

    ImGui::PushFont(p_font_large);
    ImGui::TextUnformatted("Operators");
    ImGui::PopFont();

    if (help_op_cache.empty()) {
        build_help_cache();
    }

    for (const CachedOperatorCategory& category : help_op_cache) {
        if (category.category_name) {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16.0);
            ImGui::PushFont(p_font_medium);
            ImGui::Text("%s Operators", category.category_name.value());
            ImGui::PopFont();
        }

        for (const CachedOperator& op : category.category_ops) {
            render_help_operator(op);
        }
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);

    ImGui::PushFont(p_font_large);
    ImGui::TextUnformatted("Unit Families");
    ImGui::PopFont();

    for (const RCalc::UnitFamily* family : UnitsMap::get_units_map().get_alphabetical()) {
        render_help_unit_family(family);
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);
    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);

    ImGui::PushFont(p_font_large);
    ImGui::TextUnformatted("Licenses");
    ImGui::PopFont();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);

    ImGui::PopTextWrapPos();

    if (ImGui::BeginChild(
        "LicenseInfo",
        ImVec2(ImGui::GetContentRegionAvail().x * 0.85, 300),
        true
    )) {
        ImGui::PushTextWrapPos(0.0);
        ImGui::TextUnformatted(Assets::license_md);
        ImGui::PopTextWrapPos();
    }
    ImGui::EndChild();

    ImGui::EndPopup();
}


void ImGuiRenderer::render_help_command(const CommandMeta* cmd) {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);

    ImGui::PushStyleColor(ImGuiCol_Text, COLORS[COLOR_BLUE]);
    ImGui::TextUnformatted(cmd->name);
    ImGui::PopStyleColor();

    if (!cmd->aliases.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, COLORS[COLOR_GRAY]);
        ImGui::SameLine();
        ImGui::TextUnformatted("aliases: [");

        bool first = true;
        for (const char* sig : cmd->aliases) {
            ImGui::SameLine(0.0, 0.0);

            if (first) {
                ImGui::TextUnformatted(sig);
                first = false;
            }
            else {
                ImGui::Text(", %s", sig);
            }
        }

        ImGui::SameLine(0.0, 0.0);
        ImGui::TextUnformatted("]");
        ImGui::PopStyleColor();
    }

    ImGui::TextUnformatted(cmd->description);
}


void ImGuiRenderer::render_help_operator(const CachedOperator& op) {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);

    ImGui::PushStyleColor(ImGuiCol_Text, COLORS[COLOR_GREEN]);
    ImGui::TextUnformatted(op.op.name);
    ImGui::PopStyleColor();

    ImGui::TextUnformatted(op.op.description);

    ImGui::PushStyleColor(ImGuiCol_Text, COLORS[COLOR_GRAY]);

    bool types_open = false;
    
    if (!op.op.allowed_types.empty()) {
        std::string type_list_id = fmt("#op_types_%s", op.op.name);

        if (op.op.allowed_types.front().size() == 1) {
            types_open = ImGui::TreeNode(type_list_id.c_str(), "Accepts 1 argument");
        }
        else {
            types_open = ImGui::TreeNode(type_list_id.c_str(), "Accepts %llu arguments", (unsigned long long)op.op.allowed_types.front().size()); // Windows compat
        }

        if (types_open) {
            bool first_set = true;
            for (const std::vector<Type>& call_types : op.op.allowed_types) {
                ImGui::SetCursorPosX(20.0);
                bool first = true;

                if (first_set) {
                    first_set = false;
                }
                else {
                    ImGui::TextUnformatted("or");
                    ImGui::SameLine();
                }

                for (Type type : call_types) {
                    if (first) {
                        ImGui::TextUnformatted(Value::get_type_name(type));
                        first = false;
                    }
                    else {
                        ImGui::SameLine(0.0, 0.0);
                        ImGui::Text(", %s", Value::get_type_name(type));
                    }
                }
            }

            ImGui::TreePop();
        }
    }
    
    if (!op.op.examples.empty()) {
        std::string example_list_id = fmt("#op_examples_%s", op.op.name);

        if (ImGui::TreeNode(example_list_id.c_str(), "Examples")) {
            for (const std::string& example : op.examples) {
                ImGui::BulletText("%s", example.c_str());
            }

            ImGui::TreePop();
        }
    }

    ImGui::PopStyleColor();
}


void ImGuiRenderer::render_help_unit_family(const UnitFamily* family) {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0);

    ImGui::PushStyleColor(ImGuiCol_Text, COLORS[COLOR_YELLOW]);
    ImGui::TextUnformatted(family->p_name);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, COLORS[COLOR_GRAY]);
    ImGui::Text("Base type: %s", Value::get_type_name(family->base_type));
    ImGui::PopStyleColor();

    if (ImGui::TreeNode((void*)family, "Units")) {
        for (const Unit* unit : family->units) {
                ImGui::BulletText("%s", unit->p_name);

                ImGui::PushStyleColor(ImGuiCol_Text, COLORS[COLOR_GRAY]);
                ImGui::SameLine();
                ImGui::Text("(Usage: %s)", unit->p_usage);
                ImGui::PopStyleColor();
        }
        ImGui::TreePop();
    }
}


void ImGuiRenderer::build_help_cache() {
    help_op_cache.clear();
    RPNStack example_stack;

    for (const RCalc::OperatorCategory* category : OperatorMap::get_operator_map().get_alphabetical()) {
        help_op_cache.emplace_back(*category, example_stack);
    }
}


void ImGuiRenderer::add_stack_item(const StackItem& item) {
    ImGuiDisplayLine input_line;

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
                // TODO: Column vectors and representations
                str = reinterpret_cast<ValueDisplayable&>(disp).value.to_string();
                break;
            }
            case Displayable::Type::RECURSIVE:
                UNREACHABLE(); // Handled by the iterator
        }

        input_line.chunks.emplace_back(str);
    }

    ImGuiDisplayChunk output_chunk { item.result.to_string() };
    
    display_stack.entries.emplace_back(input_line, output_chunk);
}


void ImGuiRenderer::remove_stack_item() {
    display_stack.entries.pop_back();
}


void ImGuiRenderer::replace_stack_items(const std::vector<StackItem>& items) {
    display_stack.entries.clear();
    std::for_each(items.begin(), items.end(), std::bind(&ImGuiRenderer::add_stack_item, this, std::placeholders::_1));
}


// ========================

void ImGuiDisplayChunk::calculate_size(float max_width) {
    size = ImGui::CalcTextSize(str.c_str(), nullptr, false, max_width);
    position = ImVec2(0, 0);
}

void ImGuiDisplayLine::calculate_size(float max_width) {
    size = ImVec2(0, 0);

    // Step 1: Calculate sizes

    float available_width = max_width;
    float line_no = 0.1; // Avoid any potential floating point weirdness

    for (ImGuiDisplayChunk& chunk : chunks) {
        chunk.calculate_size();
    }

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
                size.y + ((line_height - iter->size.y) / 2.0)   // Center align
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

void ImGuiDisplayEntry::calculate_size(float max_width, bool scrollbar_visible) {
    float available_width = max_width - STACK_HORIZ_BIAS - (2.0 * STACK_OUTER_PADDING);
    if (scrollbar_visible) {
        available_width -= ImGui::GetStyle().ScrollbarSize;
    }

    float output_max_width = available_width / 2.0 - STACK_HORIZ_PADDING;
    output.calculate_size(output_max_width);

    float input_max_width = available_width - output.size.x - STACK_HORIZ_PADDING;
    input.calculate_size(input_max_width);
    
    height = std::max(input.size.y, output.size.y);

    // Final positioning
    if (input.size.y < height) {
        // Move input chunks down
        float diff = height - input.size.y;

        for (ImGuiDisplayChunk& chunk : input.chunks) {
            chunk.position.y += diff;
        }
    }
    else {
        // Move output chunk down
        float diff = height - output.size.y;
        output.position.y += diff;
    }

    output.position.x = available_width - output.size.x;
}

void ImGuiDisplayStack::calculate_sizes(float max_width, bool scrollbar_visible) {
    for (ImGuiDisplayEntry& entry : entries) {
        if (entry.valid) { continue; }
        entry.calculate_size(max_width, scrollbar_visible);
    }
}

void ImGuiDisplayStack::invalidate_sizes() {
    for (ImGuiDisplayEntry& entry : entries) {
        entry.valid = false;
    }
}

}