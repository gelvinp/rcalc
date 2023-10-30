#pragma once

#include "app/renderers/renderer.h"
#include "app/renderers/terminal/backends/terminal_backend.h"
#include "app/stack.h"
#include "app/autocomplete.h"
#include "app/commands/commands.h"

namespace RCalc {

class TerminalRenderer : public Renderer {
public:
    TerminalRenderer(RendererCreateInfo info);

    virtual Result<> init(Application* p_application) override;
    virtual void render_loop() override;
    virtual void cleanup() override;

    virtual void display_info(const std::string& str) override;
    virtual void display_error(const std::string& str) override;

    virtual bool try_renderer_command(const std::string& str) override;

    virtual void add_stack_item(const StackItem& item) override;
    virtual void remove_stack_item() override;
    virtual void replace_stack_items(const std::vector<StackItem>& items) override;

    REGISTER_COMMAND(TerminalRenderer, Help);
    REGISTER_COMMAND(TerminalRenderer, Queer);
    REGISTER_COMMAND(TerminalRenderer, ClearHist);

private:
    TerminalBackend& get_backend() const { return *reinterpret_cast<TerminalBackend*>(p_backend); }

    ftxui::Element render();
    ftxui::Element render_stack();
    ftxui::Element render_scratchpad();

    SubmitTextCallback cb_submit_text;
    SubmitOperatorCallback cb_submit_op;
    CommandMap<TerminalRenderer>& command_map;

    std::string message = "Welcome to RCalc! Type \\help to see what commands and operators are supported.";
    bool message_is_error = false;

    std::string scratchpad;
    ftxui::Component comp_scratchpad;
    ftxui::Component comp_stack;
    ftxui::Component comp_container;

    bool help_requested = false;
    bool queer_active = true;

    void submit_scratchpad();
    void scratchpad_changed();
    bool handle_event(ftxui::Event event);

    ftxui::Elements split_lines(std::string str);

    std::vector<std::string> history;
    std::optional<size_t> history_state = std::nullopt;

    size_t scroll_offset = 0;

    std::vector<Type> autocomp_types;
    AutocompleteManager autocomp;
};

}
