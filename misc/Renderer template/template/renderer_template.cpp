#include "renderer_template.h"


namespace RCalc {

RendererTemplate::RendererTemplate(SubmitTextCallback submit_text) {}

void RendererTemplate::early_init(const AppConfig& config, SubmitTextCallback submit_text) {}
Result<> RendererTemplate::init() { return Ok(); }
void RendererTemplate::render_loop() { while(true) {} }
void RendererTemplate::cleanup() {}

void RendererTemplate::display_info(std::string_view str) {}
void RendererTemplate::display_error(std::string_view str) {}

bool RendererTemplate::try_renderer_command(std::string_view str) { return false; }

void RendererTemplate::add_stack_item(const StackItem& item) {}
void RendererTemplate::remove_stack_item() {}
void RendererTemplate::replace_stack_items(const CowVec<StackItem>& items) {}

}