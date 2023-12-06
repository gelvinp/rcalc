#include "renderer_template.h"


namespace RCalc {

RendererTemplate::RendererTemplate(RendererCreateInfo&& info) {
    p_backend = RenderBackend::create<RendererTemplate>();
}

Result<> RendererTemplate::init(Application* p_application) { return Ok(); }
void RendererTemplate::render_loop() { while(true) {} }
void RendererTemplate::cleanup() { delete p_backend; }

void RendererTemplate::display_info(const std::string& str) {}
void RendererTemplate::display_error(const std::string& str) {}

bool RendererTemplate::try_renderer_command(const std::string& str) { return false; }

void RendererTemplate::add_stack_item(const StackItem& item) {}
void RendererTemplate::remove_stack_item() {}
void RendererTemplate::replace_stack_items(const CowVec<StackItem> items) {}

}