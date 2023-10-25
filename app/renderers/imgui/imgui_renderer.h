#pragma once

#include "app/renderers/renderer.h"

#include "app/stack.h"
#include "imgui.h"
#include "app/commands/commands.h"
#include "app/operators/operators.h"
#include "core/units/units.h"
#include "help_cache.h"

namespace RCalc {

struct ImGuiDisplayChunk {
    std::string str;
    ImVec2 size;
    ImVec2 position;

    void calculate_size(float max_width = -1.0f);

    ImGuiDisplayChunk(std::string str)
        : str(str), size({}), position({}) {}
};

struct ImGuiDisplayLine {
    std::vector<ImGuiDisplayChunk> chunks;
    ImVec2 size;

    void calculate_size(float max_width);
};

struct ImGuiDisplayEntry {
    ImGuiDisplayLine input;
    ImGuiDisplayChunk output;
    float height;
    bool valid;

    void calculate_size(float max_width, bool scrollbar_visible);
};

struct ImGuiDisplayStack {
    std::vector<ImGuiDisplayEntry> entries;

    void calculate_sizes(float max_width, bool scrollbar_visible);
    void invalidate_sizes();
};


class ImGuiRenderer : public Renderer {
public:
    ImGuiRenderer(RendererCreateInfo info);
    ~ImGuiRenderer();

    virtual Result<> init(Application* p_application) override;
    virtual void render() override;
    virtual void cleanup() override;

    virtual void display_info(const std::string& str) override;
    virtual void display_error(const std::string& str) override;

    virtual bool try_renderer_command(const std::string& str) override;

    virtual void add_stack_item(const StackItem& item) override;
    virtual void remove_stack_item() override;
    virtual void replace_stack_items(const std::vector<StackItem>& items) override;

    REGISTER_COMMAND(ImGuiRenderer, Help);
    REGISTER_COMMAND(ImGuiRenderer, Queer);
    REGISTER_COMMAND(ImGuiRenderer, ClearHist);

private:
    SubmitTextCallback cb_submit_text;
    SubmitOperatorCallback cb_submit_op;
    CommandMap<ImGuiRenderer> command_map;

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

    ImGuiDisplayStack display_stack;

    std::vector<std::string> history;
    std::optional<size_t> history_state = std::nullopt;

    std::vector<CachedOperatorCategory> help_op_cache;

    ImVector<ImWchar> glyph_ranges;
    ImFontGlyphRangesBuilder glyphs;
    ImFont* p_font_standard;
    ImFont* p_font_medium;
    ImFont* p_font_large;

    void submit_scratchpad();
    void render_help();

    static int scratchpad_input_callback(ImGuiInputTextCallbackData* p_cb_data);
    static int scratchpad_input_filter_callback(ImGuiInputTextCallbackData* p_cb_data);
    static int scratchpad_input_resize_callback(ImGuiInputTextCallbackData* p_cb_data);
    static int scratchpad_input_always_callback(ImGuiInputTextCallbackData* p_cb_data);
    static int scratchpad_input_history_callback(ImGuiInputTextCallbackData* p_cb_data);

    void render_help_command(const CommandMeta* cmd);
    void render_help_operator(const CachedOperator& op);
    void render_help_unit_family(const UnitFamily* family);

    void build_help_cache();
};

}