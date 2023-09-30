#pragma once

#include "core/logger.h"
#include "platform/platform.h"
#include "imgui.h"
#include "stack.h"
#include "renderer.h"
#include "app/operators/operators.h"
#include "app/commands/commands.h"

#include <array>
#include <format>
#include <functional>
#include <string>
#include <optional>
#include <unordered_map>
#include <vector>

namespace RCalc {

struct AppConfig {
    bool quiet = false;
    bool verbose = false;
};

class Application {
public:
    Application();
    void step();

    REGISTER_COMMAND(Application, Clear);
    REGISTER_COMMAND(Application, Quit);
    REGISTER_COMMAND(Application, Pop);
    REGISTER_COMMAND(Application, Swap);
    REGISTER_COMMAND(Application, Copy);
    REGISTER_COMMAND(Application, Dup);
    REGISTER_COMMAND(Application, Count);

private:

    RPNStack stack;
    Renderer renderer;
    OperatorMap op_map;

    void on_renderer_submit_text(const std::string& str);
    bool on_renderer_submit_operator(const std::string& str);
    void on_renderer_requested_app_commands(Renderer::AppCommandCallback cb_app_cmd);
    void on_renderer_requested_operators(Renderer::OperatorCallback cb_ops_cmd);

    typedef std::function<void()> AppCommand;
    bool try_application_command(const std::string& str);
    CommandMap<Application> command_map;
};

}