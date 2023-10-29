#include "renderer.h"
#include "core/logger.h"
#include "core/format.h"

#include <stdexcept>

#ifdef RENDERER_IMGUI_ENABLED
#include "app/renderers/imgui/imgui_renderer.h"
#endif

#ifdef RENDERER_TERMINAL_ENABLED
#include "app/renderers/terminal/terminal_renderer.h"
#endif

// TODO: Make this a generated file (Prereq for lib target!)

namespace RCalc {

Result<Renderer*> Renderer::create(const std::string_view& name, RendererCreateInfo info) {
    Renderer* p_renderer = nullptr;

#ifdef RENDERER_IMGUI_ENABLED
    if (name == "imgui") {
        p_renderer = reinterpret_cast<Renderer*>(new ImGuiRenderer(info));
    }
#endif

#ifdef RENDERER_TERMINAL_ENABLED
    if (name == "terminal") {
        p_renderer = reinterpret_cast<Renderer*>(new TerminalRenderer(info));
    }
#endif

    if (p_renderer == nullptr) {
        return Err(ERR_INIT_FAILURE, fmt("The requested renderer '%s' is invalid!", name.data()));
    }

    Result<> res = p_renderer->init(info.p_application);
    if (!res) { return res.unwrap_err(); }

    return Ok((Renderer*)p_renderer);
}

}