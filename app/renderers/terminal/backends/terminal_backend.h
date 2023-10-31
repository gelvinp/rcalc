#pragma once

#include "app/renderers/render_backend.h"

#include "ftxui/component/component.hpp"

#include <functional>
#include <string>

namespace RCalc {

class TerminalBackend : public RenderBackend {
public:
    Result<> init(std::function<void(const std::string&)> cb_report_error);
    void cleanup();

    void render_loop(ftxui::Component component);
    
    virtual void copy_to_clipboard(const std::string_view& string) override;

private:
    std::function<void(const std::string&)> cb_report_error = nullptr;
};

}