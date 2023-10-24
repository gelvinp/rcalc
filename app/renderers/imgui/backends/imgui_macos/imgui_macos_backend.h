#if defined(ENABLE_PLATFORM_MACOS)

#pragma once

#include "app/renderers/render_backend.h"
#include "macos_binding.h"


class ImGuiMacOSBackend : public RenderBackend {
public:
    ImGuiMacOSBackend();

    virtual Result<> init(RCalc::Application* p_application) override;
    virtual void cleanup() override;

    virtual void start_frame() override;
    virtual void render_frame() override;
    
    virtual bool app_menu_bar() override { return false; }
    virtual void copy_to_clipboard(const std::string_view& string) override;
    virtual float get_screen_dpi() override;

private:
    RCalc::MacOS binding;
};

#endif