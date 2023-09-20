#pragma once

#include "core/error.h"

#define _RCALC_PLATFORM_IMPL(platform_type) \
platform_type _RCALC_PLATFORM_##platform_type; \
Platform* Platform::singleton = &_RCALC_PLATFORM_##platform_type;

class Platform {
public:
    static Platform& get_singleton() { return *singleton; }

    virtual Result<> init() = 0;
    virtual void start_frame() = 0;
    virtual void render_frame() = 0;
    virtual void cleanup() = 0;

    virtual void copy_to_clipboard(const std::string& string) = 0;

    bool close_requested = false;

private:
    static Platform* singleton;
};