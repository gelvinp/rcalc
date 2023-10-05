#include "application.h"

#include "core/logger.h"
#include "core/format.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include <algorithm>
#include <cmath>
#include <ranges>
#include <set>
#include <sstream>


namespace RCalc {

Result<Application*> Application::create(AppConfig config) {
    Application* p_application = new Application();

    Result<Renderer*> renderer_res = Renderer::create(
        config.renderer_name.data(),
        std::bind(&Application::on_renderer_submit_text, p_application, std::placeholders::_1),
        std::bind(&Application::on_renderer_submit_operator, p_application, std::placeholders::_1),
        std::bind(&Application::on_renderer_requested_app_commands, p_application, std::placeholders::_1),
        std::bind(&Application::on_renderer_requested_operators, p_application, std::placeholders::_1)
    );

    if (renderer_res) {
        p_application->p_renderer = renderer_res.unwrap();
        return Ok(p_application);
    }
    else {
        delete p_application;
        return renderer_res.unwrap_err();
    }
}

Application::Application() {
    command_map = get_command_map<Application>();
    op_map = get_operator_map();
}

void Application::step() {
    std::vector<RenderItem> render_items;
    for (const StackItem& item : stack.get_items()) {
        RenderItem render_item { item.input, item.output };
        render_items.push_back(render_item);
    }
    p_renderer->render(render_items);
}

void Application::on_renderer_submit_text(const std::string& str) {
    // If it starts with '\', it's a command
    if (str.starts_with('\\')) {
        if (try_application_command(str)) { return; }
        if (p_renderer->try_renderer_command(str)) { return; }

        const char* error_format = "Unknown command: '%s'";
        int size = snprintf(nullptr, 0, error_format, str.c_str()) + 1;
        std::string error;
        error.resize(size);
        snprintf(error.data(), error.size(), error_format, str.c_str());
        p_renderer->display_error(error);
        return;
    }

    // Try to parse as Value first
    std::optional<Value> value = Value::parse(str);
    if (value) {
        stack.push_item(StackItem{value.value().to_string(), value.value().to_string(), std::move(value).value(), false});
        return;
    }

    // Try to parse as Operator second

    if (on_renderer_submit_operator(str)) { return; }

    // Try to parse as swizzle
    if (try_swizzle(str)) { return; }

    // Unknown command!
    p_renderer->display_error(fmt("Unknown operator: '%s'", str.c_str()));
}


bool Application::on_renderer_submit_operator(const std::string& str) {
    if (op_map.contains(str)) {
        Result<> res = op_map.at(str)->evaluate(stack);
        if (!res) { p_renderer->display_error(res.unwrap_err().get_message()); }
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


bool Application::try_swizzle(const std::string& str) {
    // Swizzles take the pattern .rgba or .xyzw (or any combination)
    if (!str.starts_with('.')) { return false; }
    if (str.length() > 5) { return false; }

    std::string pattern { str.data() + 1, str.length() - 1 };
    std::transform(pattern.begin(), pattern.end(), pattern.begin(), [](unsigned char c){ return std::tolower(c); });
    std::vector<Real> values;
    std::vector<StackItem> source_value = stack.pop_items(1);
    std::stringstream ss;
    ss << source_value[0].get_input_formatted();
    ss << '.';

    // Swizzles can only operate on vectors
    switch (source_value[0].result.get_type()) {
        case Value::TYPE_VEC2: {
            Vec2 value = source_value[0].result;

            for (char ch : pattern) {
                switch (ch) {
                    case 'r':
                    case 'x':
                        values.push_back(value.r);
                        break;
                    case 'g':
                    case 'y':
                        values.push_back(value.g);
                        break;
                    case 'b':
                    case 'z':
                    case 'a':
                    case 'w':
                        p_renderer->display_error(fmt("Swizzle out of bounds for Vec2: '%s'", str.c_str()));
                        stack.push_items(std::move(source_value));
                        return true;
                    default:
                        p_renderer->display_error(fmt("Unrecognized swizzle: '%s'", str.c_str()));
                        stack.push_items(std::move(source_value));
                        return true;
                }

                ss << ch;
            }

            break;
        }
        case Value::TYPE_VEC3: {
            Vec3 value = source_value[0].result;

            for (char ch : pattern) {
                switch (ch) {
                    case 'r':
                    case 'x':
                        values.push_back(value.r);
                        break;
                    case 'g':
                    case 'y':
                        values.push_back(value.g);
                        break;
                    case 'b':
                    case 'z':
                        values.push_back(value.b);
                        break;
                    case 'a':
                    case 'w':
                        p_renderer->display_error(fmt("Swizzle out of bounds for Vec3: '%s'", str.c_str()));
                        stack.push_items(std::move(source_value));
                        return true;
                    default:
                        p_renderer->display_error(fmt("Unrecognized swizzle: '%s'", str.c_str()));
                        stack.push_items(std::move(source_value));
                        return true;
                }

                ss << ch;
            }

            break;
        }
        case Value::TYPE_VEC4: {
            Vec4 value = source_value[0].result;

            for (char ch : pattern) {
                switch (ch) {
                    case 'r':
                    case 'x':
                        values.push_back(value.r);
                        break;
                    case 'g':
                    case 'y':
                        values.push_back(value.g);
                        break;
                    case 'b':
                    case 'z':
                        values.push_back(value.b);
                        break;
                    case 'a':
                    case 'w':
                        values.push_back(value.a);
                        break;
                    default:
                        p_renderer->display_error(fmt("Unrecognized swizzle: '%s'", str.c_str()));
                        stack.push_items(std::move(source_value));
                        return true;
                }

                ss << ch;
            }

            break;
        }
        default:
            stack.push_items(std::move(source_value));
            return false;
    }

    switch (values.size()) {
        case 1: {
            Value result(values[0]);
            stack.push_item(StackItem {
                ss.str(),
                result.to_string(),
                std::move(result),
                true
            });
            return true;
        }
        case 2: {
            Value result(Vec2(values[0], values[1]));
            stack.push_item(StackItem {
                ss.str(),
                result.to_string(),
                std::move(result),
                true
            });
            return true;
        }
        case 3: {
            Value result(Vec3(values[0], values[1], values[2]));
            stack.push_item(StackItem {
                ss.str(),
                result.to_string(),
                std::move(result),
                true
            });
            return true;
        }
        case 4: {
            Value result(Vec4(values[0], values[1], values[2], values[3]));
            stack.push_item(StackItem {
                ss.str(),
                result.to_string(),
                std::move(result),
                true
            });
            return true;
        }
        default:
            UNREACHABLE();
    }
}

}