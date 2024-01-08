#pragma once

#include "core/error.h"

#if defined(ENABLE_PLATFORM_LINUX) && defined(DBUS_ENABLED)
#define IS_SYSTEM_COLOR_AVAILABLE true
#else
#define IS_SYSTEM_COLOR_AVAILABLE false
#endif


namespace RCalc {

struct SettingsManager {
    enum ColorScheme : int {
        COLORS_DARK = 0,
        COLORS_LIGHT = 1,
        COLORS_SYSTEM = 2
    };
    static const char* COLOR_LABELS[3];

    constexpr static bool SYSTEM_COLOR_AVAILABLE = IS_SYSTEM_COLOR_AVAILABLE;

    ColorScheme colors = SYSTEM_COLOR_AVAILABLE ? COLORS_SYSTEM : COLORS_DARK;
    float ui_scale = 1.0;
    int precision = 8;

    Result<> load();
    Result<> save();
};

}