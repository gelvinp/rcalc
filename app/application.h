#pragma once

#include "stack.h"
#include "app/renderers/renderer.h"
#include "app/operators/operators.h"
#include "app/commands/commands.h"

#include <functional>
#include <string>
#include <string_view>

namespace RCalc {

struct AppConfig {
    bool quiet = false;
    bool verbose = false;
    std::string renderer_name;
};

class Application {
public:
    void run();
    void cleanup();

    static Result<Application*> create(AppConfig config);

    REGISTER_COMMAND(Application, Clear);
    REGISTER_COMMAND(Application, Pop);
    REGISTER_COMMAND(Application, Swap);
    REGISTER_COMMAND(Application, Copy);
    REGISTER_COMMAND(Application, Dup);
    REGISTER_COMMAND(Application, Count);
    REGISTER_COMMAND(Application, Undo);
    REGISTER_COMMAND(Application, Type);

    void on_renderer_submit_text(const std::string& str);
    bool on_renderer_submit_operator(const std::string& str);

    static const RPNStack& get_stack() { return singleton.stack; }

private:
    Application();

    RPNStack _stack_a, _stack_b;
    RPNStack *p_stack_active, *p_stack_backup;

    RPNStack stack;
    Renderer* p_renderer;
    OperatorMap& op_map;

    typedef std::function<void()> AppCommand;
    bool try_application_command(const std::string& str);
    CommandMap<Application> command_map;

    bool try_swizzle(const std::string& str);

    void swap_stacks();

    static Application singleton;
    bool initialized = false;
};

}