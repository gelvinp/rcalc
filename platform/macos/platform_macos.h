#pragma once

#include "platform/platform.h"
#include "macos_binding.h"

class PlatformMacOS : public Platform {
public:
    PlatformMacOS();

    virtual Result<> init(RCalc::AppConfig config) override;
    virtual void runloop() override;
    virtual void cleanup() override;

    virtual void start_frame() override;
    virtual void render_frame() override;

    virtual bool app_menu_bar() override { return false; }
    virtual void copy_to_clipboard(const std::string_view& string) override;
    virtual float get_screen_dpi() override;

private:
    RCalc::MacOS binding;
};