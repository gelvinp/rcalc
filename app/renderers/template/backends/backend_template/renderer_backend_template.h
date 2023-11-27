#pragma once

#include "app/renderers/render_backend.h"

namespace RCalc {

class RendererBackendTemplate : RenderBackend {
public:
    virtual void copy_to_clipboard(const std::string_view& string) override;
};

}