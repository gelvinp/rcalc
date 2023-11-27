#include "renderer_template.h"

#include "backends/backend_template/renderer_backend_template.h"
#include <thread>

namespace RCalc {

RendererBackendTemplate backend;

RendererTemplate::RendererTemplate(RendererCreateInfo&& info) {
    p_backend = reinterpret_cast<RenderBackend*>(&backend);
}

Result<> RendererTemplate::init(Application* p_application) { return Ok(); }
void RendererTemplate::render_loop() {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5000ms);
}
void RendererTemplate::cleanup() { }

void RendererTemplate::display_info(const std::string& str) {}
void RendererTemplate::display_error(const std::string& str) {}

bool RendererTemplate::try_renderer_command(const std::string& str) { return false; }

void RendererTemplate::add_stack_item(const StackItem& item) {}
void RendererTemplate::remove_stack_item() {}
void RendererTemplate::replace_stack_items(const std::vector<StackItem>& items) {}

}