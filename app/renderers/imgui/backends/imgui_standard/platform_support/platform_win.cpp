#ifdef ENABLE_PLATFORM_WINDOWS

#include "app/renderers/imgui/backends/imgui_standard/imgui_standard_backend.h"
#include "core/logger.h"

bool RCalc::ImGuiStandardBackend::is_dark_theme() const { return true; }

#endif