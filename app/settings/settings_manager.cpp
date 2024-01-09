#include "settings_manager.h"
#include "core/value.h"

namespace RCalc {

const char* SettingsManager::COLOR_LABELS[3] = {
    "Dark",
    "Light",
    "System"
};


SettingsManager::SettingsManager() {
    colors = SYSTEM_COLOR_AVAILABLE ? COLORS_SYSTEM : COLORS_DARK;
    ui_scale = 1.0;
    precision = Value::get_precision();
}

}