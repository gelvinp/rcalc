#include "imgui_macos_backend.h"
#include "app/renderers/imgui/imgui_renderer.h"

namespace RCalc {


template<>
RenderBackend* RenderBackend::create<ImGuiRenderer>() {
    return reinterpret_cast<RenderBackend*>(new ImGuiMacOSBackend());
}


ImGuiMacOSBackend::ImGuiMacOSBackend()
    : binding(this)
{}


Result<> ImGuiMacOSBackend::init(Application* p_application) {
    binding.set_application(p_application);
    return binding.init();
}


void ImGuiMacOSBackend::cleanup() {
    binding.cleanup();
}


void ImGuiMacOSBackend::start_frame() {
    binding.start_frame();
}


void ImGuiMacOSBackend::render_frame() {
    binding.render_frame();
}


void ImGuiMacOSBackend::copy_to_clipboard(const std::string_view& string) {
    binding.copy_to_clipboard(string);
}


float ImGuiMacOSBackend::get_screen_dpi() {
    return binding.get_screen_dpi();
}

}