#include "platform_macos.h"

#include "app/application.h"

#include <GLFW/glfw3.h>

_RCALC_PLATFORM_IMPL(PlatformMacOS);

PlatformMacOS::PlatformMacOS()
    : binding(&_RCALC_PLATFORM_PlatformMacOS)
{}


Result<> PlatformMacOS::init() {
    return binding.init();
}


void PlatformMacOS::runloop() {
    RCalc::Application app;

    while (!close_requested) {
        start_frame();
        app.step();
        render_frame();
    }
}


void PlatformMacOS::cleanup() {
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