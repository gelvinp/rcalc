#pragma once

#include "core/value.h"
#include "core/error.h"
#include "render_backend.h"

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace RCalc {

class Application;

struct RenderItem {
    std::string_view input;
    std::string_view output;
};

class Renderer {
public:
    typedef std::function<void(const std::string&)> SubmitTextCallback;
    typedef std::function<void(const std::string&)> SubmitOperatorCallback;

    struct RendererCreateInfo {
        SubmitTextCallback cb_submit_text;
        SubmitOperatorCallback cb_submit_op;
        Application* p_application;
    };

    static Result<Renderer*> create(const std::string_view& name, RendererCreateInfo info);
    static std::vector<const char*> get_enabled_renderers();

    virtual Result<> init(Application* p_application) = 0;
    virtual void render(const std::vector<RenderItem>& items) = 0;
    virtual void cleanup() = 0;

    virtual void display_info(const std::string& str) = 0;
    virtual void display_error(const std::string& str) = 0;

    virtual bool try_renderer_command(const std::string& str) = 0;

    static const char* default_renderer;

    RenderBackend* p_backend;
};

}