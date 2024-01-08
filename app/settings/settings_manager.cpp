#include "settings_manager.h"

namespace RCalc {

const char* SettingsManager::COLOR_LABELS[3] = {
    "Dark",
    "Light",
    "System"
};

Result<> SettingsManager::load() {
    return Err(ERR_INIT_FAILURE, "Not implemented");
}

Result<> SettingsManager::save() {
    return Err(ERR_INIT_FAILURE, "Not implemented");
}

}