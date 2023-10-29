#include "renderer.h"
#include "core/logger.h"
#include "core/format.h"

#include <stdexcept>

/*
 * RENDERER TEMPLATE INSTRUCTIONS
 * ===
 * Copy the below #ifdef -> #endif block into place in app/renderers/renderer.cpp,
 * and change the include path as needed
 */

#ifdef RENDERER_TEMPLATE_ENABLED
#include "app/renderers/template/renderer_template.h"
#endif

namespace RCalc {

Result<Renderer*> Renderer::create(const std::string_view& name, RendererCreateInfo info) {
/*
 * RENDERER TEMPLATE INSTRUCTIONS
 * ===
 * Copy the below #ifdef -> #endif block into place in app/renderers/renderer.cpp,
 * and change the if statement, and p_renderer definition as needed
 */

#ifdef RENDERER_TEMPLATE_ENABLED
    if (name == "template") {
        RendererTemplate* p_renderer = new RendererTemplate(info);

        Result<> res = p_renderer->init(info.p_application);
        if (!res) { return res.unwrap_err(); }

        return Ok((Renderer*)p_renderer);
    }
#endif

    return Err(ERR_INIT_FAILURE, fmt("The requested renderer '%s' is invalid!", name.data()));
}

}
