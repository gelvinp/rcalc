#include "terminal_backend.h"

#include "app/application.h"
#include "app/renderers/terminal/terminal_renderer.h"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/loop.hpp"

#include <iostream>
#include <thread>

namespace RCalc {

template<>
RenderBackend* RenderBackend::create<TerminalRenderer>() {
    return reinterpret_cast<RenderBackend*>(new TerminalBackend());
}


Result<> TerminalBackend::init() { return Ok(); }
void TerminalBackend::cleanup() {}


void TerminalBackend::render_loop(ftxui::Component component) {
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();
    ftxui::Loop loop(&screen, component);

    while (!close_requested) {
        loop.RunOnceBlocking();
        close_requested |= loop.HasQuitted();
    }
}

}