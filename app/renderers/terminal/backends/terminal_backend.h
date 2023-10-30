#pragma once

#include "app/renderers/render_backend.h"

#include "ftxui/screen/screen.hpp"
#include "ftxui/component/component.hpp"

namespace RCalc {

class TerminalBackend : public RenderBackend {
public:
    Result<> init();
    void cleanup();

    void render_loop(ftxui::Component component);
    
    // TODO: Replace w/ platform specific
    virtual void copy_to_clipboard(const std::string_view& string) override { UNUSED(string); }
};

}