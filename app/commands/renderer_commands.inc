// @RCalcCommand
// Name: Help
// Scope: Renderer
// Description: Displays a help window.
// Usage: help
// Requires: "app/renderer.h"
RCALC_CMD(Renderer, Help, renderer) {
    renderer.help_requested = true;
}
