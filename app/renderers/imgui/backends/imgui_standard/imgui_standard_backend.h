#if defined(ENABLE_PLATFORM_LINUX) || defined(ENABLE_PLATFORM_WIN)

#pragma once

#include "app/renderers/render_backend.h"

#include <GLFW/glfw3.h>


class ImGuiStandardBackend : RenderBackend {
public:
    virtual Result<> init(RCalc::Application* p_application) override;
    virtual void cleanup() override;

    virtual void start_frame() override;
    virtual void render_frame() override;
    
    virtual void copy_to_clipboard(const std::string_view& string) override;

private:
    GLFWwindow* p_window;
};

#endif