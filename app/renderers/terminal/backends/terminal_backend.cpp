#include "terminal_backend.h"

#include "app/application.h"
#include "app/renderers/terminal/terminal_renderer.h"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/loop.hpp"

#ifdef MODULE_CLIP_ENABLED
#include "modules/clip/upstream/clip.h"
#endif

#include <iostream>
#include <thread>

namespace RCalc {

Result<> TerminalBackend::init(ReportErrorCallback _cb_report_error) {
    cb_report_error = _cb_report_error;
    return Ok();
}

void TerminalBackend::cleanup() {}


void TerminalBackend::render_loop(ftxui::Component component) {
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
    ftxui::Loop loop(&screen, component);

    while (!close_requested) {
        loop.RunOnceBlocking();
        close_requested |= loop.HasQuitted();
    }
}


void TerminalBackend::copy_to_clipboard(const std::string_view& string) {
#ifdef MODULE_CLIP_ENABLED
    if (!clip::set_text(std::string { string })) {
        cb_report_error("Failed to copy to the clipboard!");
    }
#else
    UNUSED(string);
    cb_report_error("RCalc was not compiled with terminal clipboard support!");
#endif
}

}