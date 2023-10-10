#include "renderer.h"
#include "core/logger.h"
#include "core/format.h"

#include <stdexcept>

#ifdef RENDERER_IMGUI_ENABLED
#include "app/renderers/imgui/imgui_renderer.h"
#endif

namespace RCalc {

Result<Renderer*> Renderer::create(const std::string_view& name, SubmitTextCallback cb_submit_text, SubmitOperatorCallback cb_submit_op, RequestAppCommandsCallback cb_request_app_cmds, RequestOperatorsCallback cb_request_ops) {
#ifdef RENDERER_IMGUI_ENABLED
    if (name == "imgui") {
        ImGuiRenderer* p_renderer = new ImGuiRenderer(cb_submit_text, cb_submit_op, cb_request_app_cmds, cb_request_ops);
        return Ok((Renderer*)p_renderer);
    }
#endif

    return Err(ERR_INIT_FAILURE, fmt("The requested renderer '%s' is invalid!", name.data()));
}

}