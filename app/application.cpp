#include "application.h"

#include "core/logger.h"
#include "imgui_stdlib.h"

#include <algorithm>
#include <cmath>
#include <ranges>
#include <set>
#include <sstream>


namespace RCalc {

Application::Application() :
    renderer(Renderer(
        std::bind(&Application::on_renderer_submit_text, this, std::placeholders::_1),
        std::bind(&Application::on_renderer_submit_operator, this, std::placeholders::_1),
        std::bind(&Application::on_renderer_requested_app_commands, this, std::placeholders::_1),
        std::bind(&Application::on_renderer_requested_operators, this, std::placeholders::_1))
) {
    command_map = get_command_map<Application>();
    op_map = get_operator_map();
}

void Application::step() {
    std::vector<RenderItem> render_items;
    for (const StackItem& item : stack.get_items()) {
        render_items.emplace_back(item.input, item.output);
    }
    renderer.render(render_items);
}

void Application::on_renderer_submit_text(const std::string& str) {
    // If it starts with '\', it's a command
    if (str.starts_with('\\')) {
        if (try_application_command(str)) { return; }
        if (renderer.try_renderer_command(str)) { return; }

        const char* error_format = "Unknown command: '%s'";
        int size = snprintf(nullptr, 0, error_format, str.c_str()) + 1;
        std::string error;
        error.resize(size);
        snprintf(error.data(), error.size(), error_format, str.c_str());
        renderer.display_error(error);
        return;
    }

    // Try to parse as Value first
    std::optional<Value> value = Value::parse(str);
    if (value) {
        stack.push_item(StackItem{value.value().to_string(), value.value().to_string(), std::move(value.value()), false});
        return;
    }

    // Try to parse as Operator second

    if (on_renderer_submit_operator(str)) { return; }

    // Unknown command!
    const char* error_format = "Unknown operator: '%s'";
    int size = snprintf(nullptr, 0, error_format, str.c_str()) + 1;
    std::string error;
    error.resize(size);
    snprintf(error.data(), error.size(), error_format, str.c_str());
    renderer.display_error(error);
}


bool Application::on_renderer_submit_operator(const std::string& str) {
    if (op_map.contains(str)) {
        Result<> res = op_map.at(str)->evaluate(stack);
        if (!res) { renderer.display_error(res.unwrap_err().get_message()); }
        return true;
    }
    return false;
}


void Application::on_renderer_requested_app_commands(Renderer::AppCommandCallback cb_app_cmd) {
    // Some commands have multiple aliases
    std::set<std::string> displayed_commands;
    for (const auto& [key, command] : command_map) {
        if (displayed_commands.contains(command->name)) { continue; }
        displayed_commands.insert(command->name);
        cb_app_cmd(command->name, command->description, command->signatures);
    }
}


void Application::on_renderer_requested_operators(Renderer::OperatorCallback cb_ops_cmd) {
    std::set<std::string> displayed_ops;
    for (const auto& [key, op] : op_map) {
        cb_ops_cmd(op->name, op->description, op->allowed_types);
    }
}


bool Application::try_application_command(const std::string& str) {
    if (command_map.contains(str)) {
        command_map.at(str)->execute(*this);
        return true;
    }

    return false;
}

}