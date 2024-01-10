#ifdef ENABLE_PLATFORM_MACOS

#pragma once

#include "app/renderers/imgui/backends/imgui_backend.h"
#include "macos_binding.h"

namespace RCalc {


class ImGuiMacOSBackend : public ImGuiBackend {
public:
    ImGuiMacOSBackend();

    virtual Result<> init(Renderer::SubmitTextCallback cb_submit_text) override;
    virtual void cleanup() override;

    virtual void start_frame() override;
    virtual void render_frame() override;

    virtual void recreate_font_atlas() override;
    
    virtual bool app_menu_bar() override { return false; }
    virtual void copy_to_clipboard(const std::string_view& string) override;
    virtual float get_screen_dpi() override;
    virtual Result<bool> is_dark_theme() const override;

    virtual ~ImGuiMacOSBackend() = default;

private:
    MacOS binding;
};

}

#endif