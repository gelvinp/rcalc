#pragma once

#include "app/renderers/renderer.h"
#include "core/error.h"


namespace RCalc
{

class ImGuiMacOSBackend;
class Application;
struct MacOSBinding;

class MacOS {
public:
    MacOS(ImGuiMacOSBackend* p_backend);
    ~MacOS();

    Result<> init();
    void cleanup();

    void set_submit_text_callback(Renderer::SubmitTextCallback cb_submit_text);

    void start_frame();
    void render_frame();

    void copy_to_clipboard(const std::string_view& string);
    float get_screen_dpi();

    // Rule of 5
    #ifndef __OBJC
    MacOS(const MacOS&) = delete;
    MacOS(MacOS&&) = delete;
    MacOS& operator=(const MacOS&) = delete;
    MacOS& operator=(MacOS&&) = delete;
    #endif

private:
    MacOSBinding* p_binding;
    ImGuiMacOSBackend* p_backend;
};

}