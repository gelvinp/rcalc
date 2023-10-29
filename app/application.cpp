#include "application.h"

#include "core/logger.h"
#include "core/format.h"

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
        {
            std::bind(&Application::on_renderer_submit_text, p_application, std::placeholders::_1),
            std::bind(&Application::on_renderer_submit_operator, p_application, std::placeholders::_1),
            p_application
        }
    );

    if (renderer_res.unwrap()->p_backend == nullptr) {
        throw std::logic_error("Renderer did not initialize correctly! p_backend == nullptr");
    }

    if (renderer_res) {
        p_application->p_renderer = renderer_res.unwrap();
        return Ok(p_application);
    }
    else {
        delete p_application;
        return renderer_res.unwrap_err();
    }
}

Application::Application() :
    op_map(OperatorMap::get_operator_map())
{
    command_map = CommandMap<Application>::get_command_map();
}


void Application::step() {
    p_renderer->render();
}


void Application::cleanup() {
    stack.clear();
    p_renderer->cleanup();
}


void Application::on_renderer_submit_text(const std::string& str) {
    // If it starts with '\', it's a command
    if (str.starts_with('\\')) {
        if (try_application_command(str) || p_renderer->try_renderer_command(str)) {
            p_renderer->replace_stack_items(stack.get_items());
            return;
        }

        const char* error_format = "Unknown command: '%s'";
        int size = snprintf(nullptr, 0, error_format, str.c_str()) + 1;
        std::string error;
        error.resize(size);
        snprintf(error.data(), error.size(), error_format, str.c_str());
        p_renderer->display_error(error);
        return;
    }

    // Try to parse as Value next
    std::optional<Value> value = Value::parse(str);
    if (value) {
        StackItem stack_item {
            create_displayables_from(value.value()),
            std::move(value).value(),
            false
        };

        p_renderer->add_stack_item(stack_item);
        stack.push_item(std::move(stack_item));
        return;
    }

    // Try to parse as Operator third

    if (on_renderer_submit_operator(str)) { return; }

    // Try to parse as swizzle last
    if (try_swizzle(str)) { return; }

    // Unknown command!
    p_renderer->display_error(fmt("Unknown operator: '%s'", str.c_str()));
}


bool Application::on_renderer_submit_operator(const std::string& str) {
    if (op_map.has_operator(str)) {
        Result<> res = op_map.evaluate(str, stack);
        if (!res) { p_renderer->display_error(res.unwrap_err().get_message()); }
        p_renderer->replace_stack_items(stack.get_items());
        return true;
    }
    return false;
}


bool Application::try_application_command(const std::string& str) {
    if (command_map.has_command(str)) {
        command_map.execute(str, *this);
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
        case TYPE_VEC2: {
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
        case TYPE_VEC3: {
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
        case TYPE_VEC4: {
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

    p_renderer->remove_stack_item();
    Value result;

    switch (values.size()) {
        case 1: {
            result = Value(values[0]);
            break;
        }
        case 2: {
            result = Value(Vec2(values[0], values[1]));
            break;
        }
        case 3: {
            result = Value(Vec3(values[0], values[1], values[2]));
            break;
        }
        case 4: {
            result = Value(Vec4(values[0], values[1], values[2], values[3]));
            break;
        }
        default:
            UNREACHABLE();
    }

    StackItem stack_item {
        create_displayables_from(result),
        std::move(result),
        true
    };

    p_renderer->add_stack_item(stack_item);
    stack.push_item(std::move(stack_item));
    return true;
}


}