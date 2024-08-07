#ifdef ENABLE_PLATFORM_WINDOWS

#include "app/settings/settings_manager.h"
#include "core/logger.h"
#include "core/types.h"

#include <array>
#include <algorithm>
#include <charconv>
#include <ctype.h>
#include <fstream>
#include <stdlib.h>


namespace RCalc {

std::optional<fs::path> get_data_path() {
#ifdef USING_MINGW
    if (const char* home = getenv("HOME")) {
#else
    char* home = nullptr;
    size_t size = 0;
    if (_dupenv_s(&home, &size, "APPDATA") == 0 && home != nullptr) {
#endif
        fs::path path { home };
        if (path.is_absolute()) {
            path /= "rcalc";
            if (!fs::exists(path)) { fs::create_directory(path); }
            if (!fs::is_directory(path)) { return std::nullopt; }
            return path;
        }
#ifndef USING_MINGW
        free(home);
#endif
    }

    return std::nullopt;
}

#include "file_backing.inc"

}


#endif