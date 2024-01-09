#ifdef ENABLE_PLATFORM_WINDOWS

#include "app/settings/settings_manager.h"
#include "core/logger.h"
#include "core/types.h"

#include <array>
#include <charconv>
#include <ctype.h>
#include <fstream>
#include <stdlib.h>


namespace RCalc {

std::optional<fs::path> SettingsManager::get_data_path() {
    if (const char* home = getenv("APPDATA")) {
        fs::path path { home };
        if (path.is_absolute()) {
            path /= "rcalc";
            if (!fs::exists(path)) { fs::create_directory(path); }
            if (!fs::is_directory(path)) { return std::nullopt; }
            return path;
        }
    }

    return std::nullopt;
}

#include "file_backing.inc"

}


#endif