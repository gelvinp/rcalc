#include "renderer_backend_template.h"

#include "app/application.h"

#include "app/renderers/template/renderer_template.h"

namespace RCalc {

template<>
RenderBackend* RenderBackend::create<RendererTemplate>() {
    return reinterpret_cast<RenderBackend*>(new RendererBackendTemplate());
}


Result<> RendererBackendTemplate::init(Application* p_application) { return Ok(); }
void RendererBackendTemplate::cleanup() {}

void RendererBackendTemplate::start_frame() {}
void RendererBackendTemplate::render_frame() {}

void RendererBackendTemplate::copy_to_clipboard(const std::string_view& string) {}

}