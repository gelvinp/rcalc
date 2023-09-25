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

private:

    RPNStack stack;
    Renderer renderer;
    OperatorMap op_map;

    void on_renderer_submit_text(const std::string& str);
    bool on_renderer_submit_operator(const std::string& str);

    typedef std::function<void()> AppCommand;
    bool try_application_command(const std::string& str);
    CommandMap<Application> command_map;
};

}