#include "settings_manager.h"
#include "core/value.h"

namespace RCalc {

bool SettingsManager::system_color_available = false;

const char* SettingsManager::COLOR_LABELS[3] = {
    "Dark",
    "Light",
    "System"
};


SettingsManager::SettingsManager() {
    colors = system_color_available ? COLORS_SYSTEM : COLORS_DARK;
    ui_scale = 1.0;
    precision = Value::get_precision();
}

}