#if defined(ENABLE_PLATFORM_LINUX) || defined(ENABLE_PLATFORM_WIN)

#pragma once

#include "app/renderers/imgui/backends/imgui_backend.h"

#include <GLFW/glfw3.h>

namespace RCalc {


class ImGuiStandardBackend : ImGuiBackend {
public:
    virtual Result<> init(Application* p_application) override;
    virtual void cleanup() override;

    virtual void start_frame() override;
    virtual void render_frame() override;
    
    virtual void copy_to_clipboard(const std::string_view& string) override;

private:
    GLFWwindow* p_window;
};

}

#endif