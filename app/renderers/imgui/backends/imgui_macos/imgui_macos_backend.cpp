#include "imgui_macos_backend.h"
#include "app/renderers/imgui/imgui_renderer.h"

namespace RCalc {


ImGuiBackend& ImGuiBackend::get_platform_backend() {
    static ImGuiMacOSBackend backend;
    return reinterpret_cast<ImGuiBackend&>(backend);
}


ImGuiMacOSBackend::ImGuiMacOSBackend()
    : binding(this)
{}


Result<> ImGuiMacOSBackend::init(Renderer::SubmitTextCallback cb_submit_text) {
    binding.set_submit_text_callback(cb_submit_text);
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


void ImGuiMacOSBackend::recreate_font_atlas() {
    binding.recreate_font_atlas();
}


void ImGuiMacOSBackend::copy_to_clipboard(const std::string_view& string) {
    binding.copy_to_clipboard(string);
}


float ImGuiMacOSBackend::get_screen_dpi() {
    return binding.get_screen_dpi();
}


bool ImGuiMacOSBackend::is_dark_theme() const {
    return binding.is_dark_theme();
}

}