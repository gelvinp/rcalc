#pragma once

#include "ftxui/component/component.hpp"
#include "app/variable_map.h"
#include "app/renderers/renderer.h"
#include "backends/terminal_backend.h"

#include <optional>


namespace RCalc::TerminalVariablesCache {

struct VariablesCacheData {
    ftxui::Components action_butttons;
    CowVec<bool> menu_open;
    CowVec<ftxui::Components> menu_buttons;
    ftxui::Components modals;
};

ftxui::Component build_variables_cache(VariablesCacheData& data, Renderer::SubmitTextCallback cb_submit_text, bool& variables_close_requested, TerminalBackend& backend);

}