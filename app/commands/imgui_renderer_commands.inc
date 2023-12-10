// @RCalcCommand
// Description: Copies the top element from the stack to the clipboard.
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, Copy) {
    if (display_stack.entries.size() == 0) { return; }
    backend.copy_to_clipboard(display_stack.entries.back().output.str);
}


// @RCalcCommand
// Description: Displays a help window.
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, Help) {
    help_requested = true;
}


// @RCalcCommand
// Description: Toggles queer mode.
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, Queer) {
    queer_active = !queer_active;
}

// @RCalcCommand
// Description: Quits RCalc.
// Alias: q
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, Quit) {
    backend.close_requested = true;
}


// @RCalcCommand
// Description: Clears the history buffer.
// Requires: "app/renderers/imgui/imgui_renderer.h"
RCALC_CMD(ImGuiRenderer, ClearHist) {
    history.clear();
    history_state = std::nullopt;
}