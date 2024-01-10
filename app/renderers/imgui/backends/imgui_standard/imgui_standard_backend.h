#if defined(ENABLE_PLATFORM_LINUX) || defined(ENABLE_PLATFORM_WINDOWS)

#pragma once

#include "app/renderers/imgui/backends/imgui_backend.h"
#include "app/settings/settings_manager.h"
#include <GLFW/glfw3.h>

namespace RCalc {


class ImGuiStandardBackend : ImGuiBackend {
public:
    virtual Result<> init(Renderer::SubmitTextCallback cb_submit_text) override;
    virtual void cleanup() override;

    virtual void start_frame() override;
    virtual void render_frame() override;

    virtual void recreate_font_atlas() override;
    
    virtual void copy_to_clipboard(const std::string_view& string) override;

    virtual Result<bool> is_dark_theme() const override;

    virtual ~ImGuiStandardBackend() = default;

private:
    GLFWwindow* p_window;
};

}

#endif