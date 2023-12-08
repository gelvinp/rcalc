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
    p_backend->close_requested = true;
}


// @RCalcCommand
// Description: Clears the history buffer.
// Requires: "app/renderers/terminal/terminal_renderer.h"
RCALC_CMD(TerminalRenderer, ClearHist) {
    history.clear();
    history_state = std::nullopt;
}