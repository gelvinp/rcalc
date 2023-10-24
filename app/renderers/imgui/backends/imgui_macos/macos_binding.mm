#include "macos_binding.h"
#include "macos_binding_impl.h"

#include "imgui_macos_backend.h"

namespace RCalc
{

struct MacOSBinding {
    _RCALC_MacOS_BindingImpl* p_impl;
};


MacOS::MacOS(ImGuiMacOSBackend* p_backend)
    : p_binding(new MacOSBinding()), p_backend(p_backend)
{
    p_binding->p_impl = [[_RCALC_MacOS_BindingImpl alloc] init];
}

MacOS::~MacOS() {
    if (p_binding) {
        [p_binding->p_impl release];
    }

    delete p_binding;
}


Result<> MacOS::init() {
    if (!p_binding) { return Err(ERR_INIT_FAILURE, "MacOS Binding was not initialized!"); }
    return [p_binding->p_impl start];
}


void MacOS::cleanup() {
    if (!p_binding) { return; }
    [p_binding->p_impl stop];
}


void MacOS::set_application(Application* p_application) {
    if (!p_binding) { return; }
    [p_binding->p_impl setApplication:p_application];
}


void MacOS::start_frame() {
    if (!p_binding) { return; }
    p_backend->close_requested = [p_binding->p_impl windowShouldClose];
    [p_binding->p_impl beginFrame];
}


void MacOS::render_frame() {
    if (!p_binding) { return; }
    [p_binding->p_impl renderFrame];
}


void MacOS::copy_to_clipboard(const std::string_view& string) {
    if (!p_binding) { return; }
    [p_binding->p_impl copyToClipboard:string.data()];
}


float MacOS::get_screen_dpi() {
    if (!p_binding) { return 1.0; }
    return [p_binding->p_impl getScreenDPI];
}


}