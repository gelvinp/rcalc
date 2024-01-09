#pragma once

#include "core/error.h"
#include "ftxui/component/component.hpp"

#include <functional>
#include <string>

namespace RCalc {

class TerminalRenderer;

class TerminalBackend {
public:
    struct ReportErrorCallback {
        typedef void (*Callback)(TerminalRenderer*, const std::string&);

        ReportErrorCallback()
            : p_renderer(nullptr), callback(nullptr) {}
        ReportErrorCallback(TerminalRenderer* p_renderer, Callback callback)
            : p_renderer(p_renderer), callback(callback) {}
        
        void operator()(const std::string& str) { if (p_renderer) { callback(p_renderer, str); } }
    private:
        TerminalRenderer* p_renderer;
        Callback callback;
    };

    Result<> init(ReportErrorCallback cb_report_error);
    void cleanup();

    void render_loop(ftxui::Component component);
    
    void copy_to_clipboard(const std::string_view& string);

    bool close_requested = false;

private:
    ReportErrorCallback cb_report_error;
};

}