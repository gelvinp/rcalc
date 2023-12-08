// @RCalcCommand
// Description: Displays a help window.
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, Help, renderer) {
    renderer.help_requested = true;
}


// @RCalcCommand
// Description: Toggles queer mode.
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, Queer, renderer) {
    renderer.queer_active = !renderer.queer_active;
}

// @RCalcCommand
// Description: Quits RCalc.
// Alias: q
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, Quit, renderer) {
    renderer.p_backend->close_requested = true;
}


// @RCalcCommand
// Description: Clears the history buffer.
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, ClearHist, renderer) {
    renderer.history.clear();
    renderer.history_state = std::nullopt;
}