#ifdef ENABLE_PLATFORM_WINDOWS

#include <windows.h>

#include "app/renderers/imgui/backends/imgui_standard/imgui_standard_backend.h"
#include "core/logger.h"

#include "main/main.h"
#include "core/format.h"

RCalc::Result<bool> RCalc::ImGuiStandardBackend::is_dark_theme() const {
    DWORD type, data, size = 4;

    LSTATUS res = RegGetValueA(
        HKEY_CURRENT_USER,
        TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"),
        TEXT("AppsUseLightTheme"),
        RRF_RT_REG_DWORD,
        &type,
        &data,
        &size
    );
    if (res != ERROR_SUCCESS || type != REG_DWORD) { return Err(ERR_NOT_SUPPORTED, "Unable to retreive system theme."); }

    return Ok(data == 0);
}

#endif