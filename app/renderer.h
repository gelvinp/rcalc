#pragma once

#include "stack.h"
#include "imgui.h"
#include "app/commands/commands.h"

#include <functional>
#include <string>
#include <string_view>

namespace RCalc {

struct RenderItem {
    std::string_view input;
    std::string_view output;

    float input_display_width = 0.0;
    float output_display_width = 0.0;
    float display_height = 0.0;

    RenderItem(std::string_view input, std::string_view output)
        : input(input), output(output) {}

    void recalculate_size(bool scrollbar_visible = false);
};

class Renderer {
public:
    typedef std::function<void(const std::string&)> SubmitTextCallback;
    typedef std::function<void(const std::string&)> SubmitOperatorCallback;
    typedef std::function<void(const char*, const char*, const std::vector<const char*>&)> AppCommandCallback;
    typedef std::function<void(AppCommandCallback)> RequestAppCommandsCallback;
    typedef std::function<void(const char*, const char*, const std::vector<std::vector<Value::Type>>&)> OperatorCallback;
    typedef std::function<void(OperatorCallback)> RequestOperatorsCallback;

    Renderer(SubmitTextCallback cb_submit_text, SubmitOperatorCallback cb_submit_op, RequestAppCommandsCallback cb_request_app_cmds, RequestOperatorsCallback cb_request_ops);

    void render(std::vector<RenderItem>& items);

    void display_info(const std::string& str);
    void display_error(const std::string& str);

    bool try_renderer_command(const std::string& str);

    REGISTER_COMMAND(Renderer, Help);
    REGISTER_COMMAND(Renderer, Queer);

private:
    SubmitTextCallback cb_submit_text;
    SubmitOperatorCallback cb_submit_op;
    RequestAppCommandsCallback cb_request_app_cmds;
    RequestOperatorsCallback cb_request_ops;
    CommandMap<Renderer> command_map;

    std::string message = "Welcome to RCalc! Type \\help to see what commands and operators are supported.";
    bool message_is_error = false;

    std::string scratchpad;
    ImVec2 last_window_size{};
    bool stack_needs_scroll_down = false;
    bool scratchpad_needs_clear = false;
    bool scratchpad_needs_focus = true;
    bool copy_requested = false;
    bool dup_requested = false;
    bool help_requested = false;
    bool help_open = false;
    bool scrollbar_visible = false;
    bool queer_active = false;
    bool enter_pressed = false;

    ImVector<ImWchar> glyph_ranges;
    ImFontGlyphRangesBuilder glyphs;
    ImFont* p_font_standard;
    ImFont* p_font_large;

    void submit_scratchpad();
    void render_help();

    static int scratchpad_input_callback(ImGuiInputTextCallbackData* p_cb_data);
    static int scratchpad_input_filter_callback(ImGuiInputTextCallbackData* p_cb_data);
    static int scratchpad_input_resize_callback(ImGuiInputTextCallbackData* p_cb_data);
    static int scratchpad_input_always_callback(ImGuiInputTextCallbackData* p_cb_data);

    static void render_help_command(const char* name, const char* description, const std::vector<const char*>& signatures);
    static void render_help_operator(const char* name, const char* description, const std::vector<std::vector<Value::Type>>& types);
};

}