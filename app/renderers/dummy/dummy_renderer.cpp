#include "dummy_renderer.h"


namespace RCalc {

DummyRenderer::DummyRenderer() {}

void DummyRenderer::early_init(const AppConfig& config, SubmitTextCallback submit_text) {}
Result<> DummyRenderer::init() { return Ok(); }
void DummyRenderer::render_loop() {}
void DummyRenderer::cleanup() {}

void DummyRenderer::display_info(std::string_view str) {}
void DummyRenderer::display_error(std::string_view str) {}

bool DummyRenderer::try_renderer_command(std::string_view str) { return false; }

void DummyRenderer::add_stack_item(const StackItem& item) {}
void DummyRenderer::remove_stack_item() {}
void DummyRenderer::replace_stack_items(const CowVec<StackItem>& items) {}

}