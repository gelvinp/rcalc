#pragma once

#include "app/renderers/render_backend.h"

namespace RCalc {


class ImGuiBackend : public RenderBackend {
public:
    virtual Result<> init(Application* p_application) = 0;
    virtual void cleanup() = 0;

    virtual void start_frame() = 0;
    virtual void render_frame() = 0;
    
    virtual bool app_menu_bar() { return true; }
    virtual float get_screen_dpi() { return 1.0f; }
};

}
