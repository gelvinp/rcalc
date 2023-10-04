#pragma once

#include "app/renderer.h"

#include "stack.h"
#include "imgui.h"
#include "app/commands/commands.h"

namespace RCalc {

struct ImGuiRenderItem {
    std::string_view input;
    std::string_view output;

    float input_display_width = 0.0;
    float output_display_width = 0.0;
    float display_height = 0.0;

    ImGuiRenderItem(const RenderItem& item)
        : input(item.input), output(item.output) {}

    void recalculate_size(bool scrollbar_visible = false);
};

class ImGuiRenderer : public Renderer {
public:
    ImGuiRenderer(SubmitTextCallback cb_submit_text, SubmitOperatorCallback cb_submit_op, RequestAppCommandsCallback cb_request_app_cmds, RequestOperatorsCallback cb_request_ops);

    virtual void render(const std::vector<RenderItem>& items) override;

    virtual void display_info(const std::string& str) override;
    virtual void display_error(const std::string& str) override;

    virtual bool try_renderer_command(const std::string& str) override;

    REGISTER_COMMAND(Renderer, Help);
    REGISTER_COMMAND(Renderer, Queer);
    REGISTER_COMMAND(Renderer, ClearHist);

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

    std::vector<std::string> history;
    std::optional<size_t> history_state = std::nullopt;

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
    static int scratchpad_input_history_callback(ImGuiInputTextCallbackData* p_cb_data);

    static void render_help_command(const char* name, const char* description, const std::vector<const char*>& signatures);
    static void render_help_operator(const char* name, const char* description, const std::vector<std::vector<Value::Type>>& types);
};

}