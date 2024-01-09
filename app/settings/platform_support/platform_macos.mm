#ifdef ENABLE_PLATFORM_MACOS

#include "app/settings/settings_manager.h"
#include "core/logger.h"
#include "core/types.h"


namespace RCalc {

std::optional<fs::path> SettingsManager::get_data_path() {
    return std::nullopt;
}

Result<> SettingsManager::load() {
    return Err(ERR_LOAD_FAILURE, "Not Implemented");
}

Result<> SettingsManager::save() {
    return Err(ERR_SAVE_FAILURE, "Not Implemented");
}

}

#endif