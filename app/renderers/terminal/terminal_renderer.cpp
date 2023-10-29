#include "terminal_renderer.h"


namespace RCalc {

TerminalRenderer::TerminalRenderer(RendererCreateInfo info) {
    UNUSED(info);
    p_backend = RenderBackend::create<TerminalRenderer>();
}

Result<> TerminalRenderer::init(Application* p_application) { UNUSED(p_application); return Ok(); }
void TerminalRenderer::render() {}
void TerminalRenderer::cleanup() {}

void TerminalRenderer::display_info(const std::string& str) { UNUSED(str); }
void TerminalRenderer::display_error(const std::string& str) { UNUSED(str); }

bool TerminalRenderer::try_renderer_command(const std::string& str) { UNUSED(str); return false; }

void TerminalRenderer::add_stack_item(const StackItem& item) { UNUSED(item); }
void TerminalRenderer::remove_stack_item() {}
void TerminalRenderer::replace_stack_items(const std::vector<StackItem>& items) { UNUSED(items); }

}