#include "renderer_backend_template.h"

#include "app/application.h"

#include "app/renderers/template/renderer_template.h"

namespace RCalc {

template<>
RenderBackend* RenderBackend::create<RendererTemplate>() {
    return reinterpret_cast<RenderBackend*>(new RendererBackendTemplate());
}

void RendererBackendTemplate::copy_to_clipboard(const std::string_view& string) {}

}