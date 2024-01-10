#pragma once

#include "app/renderers/renderer.h"
#include "core/error.h"

#include <string_view>

namespace RCalc {


class ImGuiBackend {
public:
    static ImGuiBackend& get_platform_backend();

    virtual Result<> init(Renderer::SubmitTextCallback cb_submit_text) = 0;
    virtual void cleanup() = 0;

    virtual void start_frame() = 0;
    virtual void render_frame() = 0;

    virtual void recreate_font_atlas() = 0;
    
    virtual bool app_menu_bar() { return true; }
    virtual float get_screen_dpi() { return 1.0f; }

    virtual void copy_to_clipboard(const std::string_view& string) = 0;

    bool close_requested = false;

    virtual Result<bool> is_dark_theme() const { return Err(ERR_NOT_IMPLEMENTED, "Backend does not support system theme."); };

    virtual ~ImGuiBackend() = default;
};

}
