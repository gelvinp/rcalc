// @RCalcCommand
// Description: Displays a help window.
// Requires: "app/renderers/terminal/terminal_renderer.h"
RCALC_CMD(TerminalRenderer, Help, renderer) {
    renderer.help_requested = true;
}


// @RCalcCommand
// Description: Toggles queer mode.
// Requires: "app/renderers/terminal/terminal_renderer.h"
RCALC_CMD(TerminalRenderer, Queer, renderer) {
    renderer.queer_active = !renderer.queer_active;
}


// @RCalcCommand
// Description: Clears the history buffer.
// Requires: "app/renderers/terminal/terminal_renderer.h"
RCALC_CMD(TerminalRenderer, ClearHist, renderer) {
    renderer.history.clear();
    renderer.history_state = std::nullopt;
}