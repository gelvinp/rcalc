#pragma once

#include "core/value.h"
#include "core/error.h"
#include "app/stack.h"

#include <functional>
#include <string>
#include <string_view>
#include <span>

namespace RCalc {

class Application;

class Renderer {
public:
    typedef std::function<void(std::string_view)> SubmitTextCallback;

    static Result<Renderer*> create(const std::string_view& name, SubmitTextCallback cb_submit_text);
    static const std::span<const char * const> get_enabled_renderers();

    virtual Result<> init() = 0;
    virtual void render_loop() = 0;
    virtual void cleanup() = 0;

    virtual void display_info(std::string_view str) = 0;
    virtual void display_error(std::string_view str) = 0;

    virtual bool try_renderer_command(std::string_view str) = 0;

    virtual void add_stack_item(const StackItem& item) = 0;
    virtual void remove_stack_item() = 0;
    virtual void replace_stack_items(const CowVec<StackItem>& items) = 0;

    static const char* default_renderer;

    virtual ~Renderer() = default;
};

}