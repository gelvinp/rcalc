#pragma once

#include "core/value.h"
#include "core/error.h"
#include "render_backend.h"
#include "app/stack.h"

#include <functional>
#include <string>
#include <string_view>
#include <span>

namespace RCalc {

class Application;

class Renderer {
public:
    typedef std::function<void(const std::string&)> SubmitTextCallback;
    typedef std::function<void(const std::string&)> SubmitOperatorCallback;

    struct RendererCreateInfo {
        SubmitTextCallback cb_submit_text;
        SubmitOperatorCallback cb_submit_op;
        Application* p_application;
    };

    static Result<Renderer*> create(const std::string_view& name, RendererCreateInfo&& info);
    static const std::span<const char*> get_enabled_renderers();

    virtual Result<> init(Application* p_application) = 0;
    virtual void render_loop() = 0;
    virtual void cleanup() = 0;

    virtual void display_info(const std::string& str) = 0;
    virtual void display_error(const std::string& str) = 0;

    virtual bool try_renderer_command(const std::string& str) = 0;

    virtual void add_stack_item(const StackItem& item) = 0;
    virtual void remove_stack_item() = 0;
    virtual void replace_stack_items(const std::vector<StackItem>& items) = 0;

    static const char* default_renderer;

    RenderBackend* p_backend = nullptr;

    virtual ~Renderer() = default;
};

}