#pragma once

#include "core/error.h"
#include "ftxui/component/component.hpp"

#include <functional>
#include <string>

namespace RCalc {

class TerminalBackend {
public:
    Result<> init(std::function<void(const std::string&)> cb_report_error);
    void cleanup();

    void render_loop(ftxui::Component component);
    
    void copy_to_clipboard(const std::string_view& string);

    bool close_requested = false;

private:
    std::function<void(const std::string&)> cb_report_error = nullptr;
};

}