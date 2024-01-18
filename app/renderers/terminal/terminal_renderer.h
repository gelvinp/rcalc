#pragma once

#include "app/renderers/renderer.h"
#include "app/renderers/terminal/backends/terminal_backend.h"
#include "app/stack.h"
#include "app/autocomplete.h"
#include "app/commands/commands.h"
#include "help_cache.h"
#include "app/settings/settings_manager.h"
#include "core/logging/engines.h"

namespace RCalc {

class TerminalRenderer : public Renderer {
public:
    TerminalRenderer();

    virtual void early_init(const AppConfig& config, SubmitTextCallback cb_submit_text) override;
    virtual Result<> init() override;
    virtual void render_loop() override;
    virtual void cleanup() override;

    virtual void display_info(std::string_view str) override;
    virtual void display_error(std::string_view str) override;

    static void _display_error(TerminalRenderer* p_renderer, const std::string& str) { p_renderer->display_error(str); };

    virtual bool try_renderer_command(std::string_view str) override;

    virtual void add_stack_item(const StackItem& item) override;
    virtual void remove_stack_item() override;
    virtual void replace_stack_items(const CowVec<StackItem>& items) override;

    static ftxui::Elements split_lines(std::string str);
    static ftxui::Elements split_paragraphs(std::string str);

    REGISTER_COMMAND(TerminalRenderer, Copy);
    REGISTER_COMMAND(TerminalRenderer, Help);
    REGISTER_COMMAND(TerminalRenderer, Settings);
    REGISTER_COMMAND(TerminalRenderer, Queer);
    REGISTER_COMMAND(TerminalRenderer, Quit);
    REGISTER_COMMAND(TerminalRenderer, ClearHist);

private:
    ftxui::Element render();
    ftxui::Element render_stack();
    ftxui::Element render_stack_scroll();

    

    SubmitTextCallback cb_submit_text;
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
    void activate_settings_page();

    std::vector<std::string> history;
    std::optional<size_t> history_state = std::nullopt;

    AutocompleteManager autocomp;

    SettingsManager settings;
    bool settings_requested = false;
    bool settings_close_requested = false;
    bool settings_open = false;
    ftxui::Component settings_page;

    std::vector<TerminalHelpCache::CachedOperatorCategory> help_op_cache;
    void build_help_cache();

    TerminalBackend backend;
};

}
