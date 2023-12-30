#pragma once

#include "stack.h"
#include "app/renderers/renderer.h"
#include "app/operators/operators.h"
#include "app/commands/commands.h"

#ifdef TESTS_ENABLED
#include "tests/test_config.h"
#endif

#include <functional>
#include <string>
#include <string_view>
#include <optional>

namespace RCalc {

struct AppConfig {
    bool quiet = false;
    bool verbose = false;
    std::string renderer_name;

    #ifdef TESTS_ENABLED
    std::optional<TestConfig> test_config;
    #endif
};

class Application {
public:
    Result<> init();
    void run();
    void cleanup();

    static Result<Application*> create(AppConfig config);

    REGISTER_COMMAND(Application, Clear);
    REGISTER_COMMAND(Application, Pop);
    REGISTER_COMMAND(Application, Swap);
    REGISTER_COMMAND(Application, Dup);
    REGISTER_COMMAND(Application, Count);
    REGISTER_COMMAND(Application, Undo);
    REGISTER_COMMAND(Application, Type);

    void on_renderer_submit_text(std::string_view str);

    const RPNStack& get_stack() const { return stack; }
    Renderer* get_renderer() const { return p_renderer; }

    _CommandMap& get_command_map() { return cmd_map; }
    const _CommandMap& get_command_map() const { return cmd_map; }

private:
    Application();

    RPNStack _stack_a, _stack_b;
    RPNStack *p_stack_active, *p_stack_backup;

    RPNStack stack;
    Renderer* p_renderer;
    OperatorMap& op_map;

    _CommandMap cmd_map;

    typedef std::function<void()> AppCommand;
    bool try_application_command(std::string_view str);
    CommandMap<Application> command_map;

    bool try_swizzle(std::string_view str);

    void swap_stacks();

    friend class Allocator;
};

}