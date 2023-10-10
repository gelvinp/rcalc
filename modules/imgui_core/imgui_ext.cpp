#include "imgui_ext.h"
#include "imgui_internal.h"

namespace ImGui {

void EXT_SetIDForLastItem(int int_id) {
    ImGuiContext& g = *GetCurrentContext();
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID(int_id);
    KeepAliveID(id);
    g.LastItemData.ID = id;
}

}