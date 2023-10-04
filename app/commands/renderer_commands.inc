// @RCalcCommand
// Name: Help
// Scope: Renderer
// Description: Displays a help window.
// Usage: help
// Requires: "app/renderer.h"
RCALC_CMD(Renderer, Help, renderer) {
    renderer.help_requested = true;
}


// @RCalcCommand
// Name: Queer
// Scope: Renderer
// Description: Toggles queer mode.
// Usage: queer
// Requires: "app/renderer.h"
RCALC_CMD(Renderer, Queer, renderer) {
    renderer.queer_active = !renderer.queer_active;
}


// @RCalcCommand
// Name: ClearHist
// Scope: Renderer
// Description: Clears the history buffer.
// Usage: clearhist
// Requires: "app/renderer.h"
RCALC_CMD(Renderer, ClearHist, renderer) {
    renderer.history.clear();
    renderer.history_state = std::nullopt;
}