#ifdef ENABLE_PLATFORM_LINUX

#include "app/settings/settings_manager.h"
#include "core/logger.h"
#include "core/types.h"

#include <array>
#include <charconv>
#include <fstream>
#include <stdlib.h>


namespace RCalc {

std::optional<fs::path> get_data_path() {
    if (const char* xdg_home = getenv("XDG_DATA_HOME")) {
        fs::path path { xdg_home };
        if (path.is_absolute()) {
            path /= "rcalc";
            if (!fs::exists(path)) { fs::create_directory(path); }
            if (!fs::is_directory(path)) { return std::nullopt; }
            return path;
        }
    }
    
    if (const char* home = getenv("HOME")) {
        fs::path path { home };
        if (path.is_absolute()) {
            path /= ".local/share/rcalc";
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