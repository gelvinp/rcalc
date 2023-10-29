#include "terminal_backend.h"

#include "app/application.h"

#include "app/renderers/terminal/terminal_renderer.h"

namespace RCalc {

template<>
RenderBackend* RenderBackend::create<TerminalRenderer>() {
    return reinterpret_cast<RenderBackend*>(new TerminalBackend());
}


Result<> TerminalBackend::init(Application* p_application) { return Ok(); }
void TerminalBackend::cleanup() {}

void TerminalBackend::start_frame() {}
void TerminalBackend::render_frame() {}

void TerminalBackend::copy_to_clipboard(const std::string_view& string) {}

}