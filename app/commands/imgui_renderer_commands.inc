// @RCalcCommand
// Name: Help
// Scope: ImGuiRenderer
// Description: Displays a help window.
// Usage: help
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, Help, renderer) {
    renderer.help_requested = true;
}


// @RCalcCommand
// Name: Queer
// Scope: ImGuiRenderer
// Description: Toggles queer mode.
// Usage: queer
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, Queer, renderer) {
    renderer.queer_active = !renderer.queer_active;
}


// @RCalcCommand
// Name: ClearHist
// Scope: ImGuiRenderer
// Description: Clears the history buffer.
// Usage: clearhist
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, ClearHist, renderer) {
    renderer.history.clear();
    renderer.history_state = std::nullopt;
}