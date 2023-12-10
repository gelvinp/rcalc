// @RCalcCommand
// Description: Copies the top element from the stack to the clipboard.
// Requires: "app/renderers/terminal/terminal_renderer.h"
// Requires: "app/renderers/terminal/entry_component.h"
RCALC_CMD(TerminalRenderer, Copy) {
    if (comp_stack->ChildCount() == 0) { return; }
    backend.copy_to_clipboard(reinterpret_cast<StackEntryComponent*>(comp_stack->ChildAt(comp_stack->ChildCount() - 1).get())->output);
    // get_backend().copy_to_clipboard(display_stack.entries.back().output.str);
}


// @RCalcCommand
// Description: Displays a help window.
// Requires: "app/renderers/terminal/terminal_renderer.h"
RCALC_CMD(TerminalRenderer, Help) {
    help_requested = true;
}


// @RCalcCommand
// Description: Toggles queer mode.
// Requires: "app/renderers/terminal/terminal_renderer.h"
RCALC_CMD(TerminalRenderer, Queer) {
    queer_active = !queer_active;
}

// @RCalcCommand
// Description: Quits RCalc.
// Alias: q
// Requires: "app/renderers/terminal/terminal_renderer.h"
RCALC_CMD(TerminalRenderer, Quit) {
    backend.close_requested = true;
}


// @RCalcCommand
// Description: Clears the history buffer.
// Requires: "app/renderers/terminal/terminal_renderer.h"
RCALC_CMD(TerminalRenderer, ClearHist) {
    history.clear();
    history_state = std::nullopt;
}