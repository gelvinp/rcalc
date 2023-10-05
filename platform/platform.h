#pragma once

#include "core/error.h"
#include "app/application.h"

#define _RCALC_PLATFORM_IMPL(platform_type) \
platform_type _RCALC_PLATFORM_##platform_type; \
Platform* Platform::singleton = &_RCALC_PLATFORM_##platform_type;

class Platform {
public:
    static Platform& get_singleton() { return *singleton; }

    virtual Result<> init(RCalc::AppConfig config) = 0;
    virtual void runloop() = 0;
    virtual void cleanup() = 0;

    virtual void start_frame() = 0;
    virtual void render_frame() = 0;

    virtual bool app_menu_bar() { return true; };
    virtual void copy_to_clipboard(const std::string_view& string) = 0;
    virtual float get_screen_dpi() { return 1.0; }

    bool close_requested = false;

    ~Platform();

protected:
    Result<> create_application(RCalc::AppConfig config);
    RCalc::Application* p_application = nullptr;

private:
    static Platform* singleton;
};