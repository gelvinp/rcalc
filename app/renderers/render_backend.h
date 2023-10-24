#pragma once

#include "core/error.h"

namespace RCalc {
    class Application;
}

class RenderBackend {
public:
    static RenderBackend* create();
    
    virtual Result<> init(RCalc::Application* p_application) = 0;
    virtual void cleanup() = 0;

    virtual void start_frame() = 0;
    virtual void render_frame() = 0;

    virtual bool app_menu_bar() { return true; }
    virtual void copy_to_clipboard(const std::string_view& string) = 0;
    virtual float get_screen_dpi() { return 1.0; }

    bool close_requested = false;
};