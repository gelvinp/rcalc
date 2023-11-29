#pragma once

#include "app/renderers/renderer.h"
#include "app/renderers/terminal/backends/terminal_backend.h"
#include "app/stack.h"
#include "app/autocomplete.h"
#include "app/commands/commands.h"
#include "help_cache.h"

namespace RCalc {

class TerminalRenderer : public Renderer {
public:
    TerminalRenderer(RendererCreateInfo&& info);

    virtual Result<> init(Application* p_application) override;
    virtual void render_loop() override;
    virtual void cleanup() override;

    virtual void display_info(const std::string& str) override;
    virtual void display_error(const std::string& str) override;

    virtual bool try_renderer_command(const std::string& str) override;

    virtual void add_stack_item(const StackItem& item) override;
    virtual void remove_stack_item() override;
    virtual void replace_stack_items(const std::vector<StackItem>& items) override;

    static ftxui::Elements split_lines(std::string str);
    static ftxui::Elements split_paragraphs(std::string str);

    REGISTER_COMMAND(TerminalRenderer, Help);
    REGISTER_COMMAND(TerminalRenderer, Queer);
    REGISTER_COMMAND(TerminalRenderer, ClearHist);

private:
    TerminalBackend& get_backend() const { return *reinterpret_cast<TerminalBackend*>(p_backend); }

    ftxui::Element render();
    ftxui::Element render_stack();
    ftxui::Element render_stack_scroll();

    SubmitTextCallback cb_submit_text;
    SubmitOperatorCallback cb_submit_op;
    CommandMap<TerminalRenderer>& command_map;

    std::string message = "Welcome to RCalc! Type \\help to see what commands and operators are supported.";
    bool message_is_error = false;

    std::string scratchpad;

    ftxui::Component comp_filler;
    ftxui::Component comp_scratchpad;
    ftxui::Component comp_stack;
    ftxui::Component comp_stack_renderer;
    ftxui::Component comp_stack_scroll;
    ftxui::Component comp_stack_scroll_renderer;
    ftxui::Component comp_container;

    bool help_requested = false;
    bool help_close_requested = false;
    bool help_open = false;
    ftxui::Component help_cache;

    bool queer_active = false;

    void submit_scratchpad();
    void scratchpad_changed();
    bool handle_event(ftxui::Event event);

    void activate_main_page();
    void activate_help_page();

    std::vector<std::string> history;
    std::optional<size_t> history_state = std::nullopt;

    AutocompleteManager autocomp;

    std::vector<TerminalHelpCache::CachedOperatorCategory> help_op_cache;
    void build_help_cache();
};

}
