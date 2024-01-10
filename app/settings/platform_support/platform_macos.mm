#ifdef ENABLE_PLATFORM_MACOS

#include "app/settings/settings_manager.h"
#include "core/logger.h"
#include "core/types.h"

#import <QuartzCore/QuartzCore.h>

namespace RCalc {

std::optional<fs::path> SettingsManager::get_data_path() {
    return std::nullopt;
}

Result<> SettingsManager::load() {
    if ([[NSUserDefaults standardUserDefaults] objectForKey:@"Colors"]) {
        int val = [[NSUserDefaults standardUserDefaults] integerForKey:@"Colors"];
        if (val < 0 || val >= MAX_COLORS) {
            return Err(ERR_LOAD_FAILURE, "Settings are invalid, please re-save settings."); 
        }
        if (system_color_available || static_cast<ColorScheme>(val) != COLORS_SYSTEM) {
            colors = static_cast<ColorScheme>(val);
        }
    }
    if ([[NSUserDefaults standardUserDefaults] objectForKey:@"UI_Scale"]) {
        float val = [[NSUserDefaults standardUserDefaults] floatForKey:@"UI_Scale"];
        if (val < 0.25f || val > 2.0f) {
            return Err(ERR_LOAD_FAILURE, "Settings are invalid, please re-save settings."); 
        }
        ui_scale = val;
    }
    if ([[NSUserDefaults standardUserDefaults] objectForKey:@"Precision"]) {
        int val = [[NSUserDefaults standardUserDefaults] integerForKey:@"Precision"];
        if (val < 1 || val > std::numeric_limits<Real>::max_digits10) {
            return Err(ERR_LOAD_FAILURE, "Settings are invalid, please re-save settings."); 
        }
        precision = val;
    }
    return Ok();
}

Result<> SettingsManager::save() {
    [[NSUserDefaults standardUserDefaults] setInteger:colors forKey:@"Colors"];
    [[NSUserDefaults standardUserDefaults] setFloat:ui_scale forKey:@"UI_Scale"];
    [[NSUserDefaults standardUserDefaults] setInteger:precision forKey:@"Precision"];
    return Ok();
}

}

#endif