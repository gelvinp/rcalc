#include "platform_macos.h"

#include "app/application.h"

#include <GLFW/glfw3.h>

_RCALC_PLATFORM_IMPL(PlatformMacOS);

PlatformMacOS::PlatformMacOS()
    : binding(&_RCALC_PLATFORM_PlatformMacOS)
{}


Result<> PlatformMacOS::init(RCalc::AppConfig config) {
    Result<> res = binding.init();
    UNWRAP_R(res);
    res = create_application(config);
    return res;
}


void PlatformMacOS::runloop() {
    binding.set_application(p_application);

    while (!close_requested) {
        start_frame();
        p_application->step();
        render_frame();
    }
}


void PlatformMacOS::cleanup() {
    p_application->cleanup();
    binding.cleanup();
}


void PlatformMacOS::start_frame() {
    binding.start_frame();
}


void PlatformMacOS::render_frame() {
    binding.render_frame();
}


void PlatformMacOS::copy_to_clipboard(const std::string_view& string) {
    binding.copy_to_clipboard(string);
}


float PlatformMacOS::get_screen_dpi() {
    return binding.get_screen_dpi();
}