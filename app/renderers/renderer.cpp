#include "renderer.h"
#include "core/logger.h"
#include "core/format.h"

#include <stdexcept>

#ifdef RENDERER_IMGUI_ENABLED
#include "app/renderers/imgui/imgui_renderer.h"
#endif

namespace RCalc {

Result<Renderer*> Renderer::create(const std::string_view& name, RendererCreateInfo info) {
#ifdef RENDERER_IMGUI_ENABLED
    if (name == "imgui") {
        ImGuiRenderer* p_renderer = new ImGuiRenderer(info);

        Result<> res = p_renderer->init(info.p_application);
        if (!res) { return res.unwrap_err(); }

        return Ok((Renderer*)p_renderer);
    }
#endif

    return Err(ERR_INIT_FAILURE, fmt("The requested renderer '%s' is invalid!", name.data()));
}

}