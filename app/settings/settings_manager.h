#pragma once

#include "core/error.h"

#include <filesystem>
#include <optional>

namespace fs = std::filesystem;


namespace RCalc {

struct SettingsManager {
    enum ColorScheme : int {
        COLORS_DARK = 0,
        COLORS_LIGHT = 1,
        COLORS_SYSTEM = 2,
        MAX_COLORS
    };
    static const char* COLOR_LABELS[3];

    static bool system_color_available;

    ColorScheme colors;
    float ui_scale;
    int precision;

    Result<> load();
    Result<> save();

    SettingsManager();
};

}