#pragma once

#include "core/error.h"

class PlatformMacOS;


namespace RCalc
{

struct MacOSBinding;

class MacOS {
public:
    MacOS(::PlatformMacOS* p_platform);
    ~MacOS();

    Result<> init();
    void cleanup();

    void start_frame();
    void render_frame();

    void copy_to_clipboard(const std::string_view& string);
    float get_screen_dpi();

    // Rule of 5
    #ifndef __OBJC
    MacOS(const MacOS&) = delete;
    MacOS(MacOS&&) = delete;
    MacOS& operator=(const MacOS&) = delete;
    MacOS& operator=(MacOS&&) = delete;
    #endif

private:
    MacOSBinding* p_binding;
    PlatformMacOS* p_platform;
};

}