#pragma once

#include "core/error.h"

#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

#if (defined(ENABLE_PLATFORM_LINUX) && defined(DBUS_ENABLED)) || defined(ENABLE_PLATFORM_MACOS)
#define IS_SYSTEM_COLOR_AVAILABLE true
#else
#define IS_SYSTEM_COLOR_AVAILABLE false
#endif


namespace RCalc {

struct SettingsManager {
    enum ColorScheme : int {
        COLORS_DARK = 0,
        COLORS_LIGHT = 1,
        COLORS_SYSTEM = 2,
        MAX_COLORS
    };
    static const char* COLOR_LABELS[3];

    constexpr static bool SYSTEM_COLOR_AVAILABLE = IS_SYSTEM_COLOR_AVAILABLE;

    ColorScheme colors;
    float ui_scale;
    int precision;

    Result<> load();
    Result<> save();

    static std::optional<fs::path> get_data_path();

    SettingsManager();
};

}