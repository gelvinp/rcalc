#include "application.h"

#include "core/logger.h"
#include "core/format.h"

#include <algorithm>
#include <cmath>
#include <ranges>
#include <set>
#include <sstream>


namespace RCalc {

Application Application::singleton;

Result<Application*> Application::create(AppConfig config) {
    if (singleton.initialized) {
        throw std::logic_error("Cannot create two application instances!");
    }

    Application* p_application = &singleton;

    Result<Renderer*> renderer_res = Renderer::create(
        config.renderer_name.data(),
        {
            std::bind(&Application::on_renderer_submit_text, p_application, std::placeholders::_1),
            std::bind(&Application::on_renderer_submit_operator, p_application, std::placeholders::_1),
            p_application
        }
    );

    if (renderer_res) {
        p_application->p_renderer = renderer_res.unwrap();
        singleton.initialized = true;

        return Ok(p_application);
    }
    else {
        return renderer_res.unwrap_err();
    }
}

Application::Application() :
    op_map(OperatorMap::get_operator_map())
{
    command_map = CommandMap<Application>::get_command_map();

    p_stack_active = &_stack_a;
    p_stack_backup = &_stack_b;
}


void Application::run() {
    command_map.activate();
    p_renderer->render_loop();
}


void Application::cleanup() {
    p_renderer->cleanup();
    delete p_renderer;

    stack.clear();
    _stack_a.clear();
    _stack_b.clear();
}


void Application::on_renderer_submit_text(const std::string& str) {
    stack = *p_stack_active;

    // If it starts with '\', it's a command
    if (str.starts_with('\\')) {
        if (try_application_command(str) || p_renderer->try_renderer_command(str)) {
            if (!stack.same_ref(*p_stack_active)) {
                // Copy on write happened, stack was mutated
                p_renderer->replace_stack_items(stack.get_items());
            }
            swap_stacks();
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
        swap_stacks();
        return;
    }

    // Try to parse as Operator third

    if (op_map.has_operator(str)) {
        Result<std::optional<size_t>> res = op_map.evaluate(str, stack);
        if (!res) { p_renderer->display_error(res.unwrap_err().get_message()); }
        if (!stack.same_ref(*p_stack_active)) {
            // Copy on write happened, stack was mutated
            std::optional<size_t> pop_count = res.unwrap();

            if (pop_count.has_value()) {
                // Stack was not mutated further during the operator, we don't need to replace the entire stack
                for (size_t index = 0; index < pop_count.value(); ++index) {
                    p_renderer->remove_stack_item();
                }
                p_renderer->add_stack_item(stack.get_items().back());
            }
            else {
                p_renderer->replace_stack_items(stack.get_items());
            }
        }
        swap_stacks();
        return;
    }

    // Try to parse as swizzle last
    if (try_swizzle(str)) { 
        swap_stacks();
        return;
    }

    // Unknown command!
    p_renderer->display_error(fmt("Unknown operator: '%s'", str.c_str()));
}


bool Application::on_renderer_submit_operator(const std::string& str) {
    stack = *p_stack_active;
    
    if (op_map.has_operator(str)) {
        Result<std::optional<size_t>> res = op_map.evaluate(str, stack);
        if (!res) { p_renderer->display_error(res.unwrap_err().get_message()); }
        if (!stack.same_ref(*p_stack_active)) {
            // Copy on write happened, stack was mutated
            std::optional<size_t> pop_count = res.unwrap();

            if (pop_count.has_value()) {
                // Stack was not mutated further during the operator, we don't need to replace the entire stack
                for (size_t index = 0; index < pop_count.value(); ++index) {
                    p_renderer->remove_stack_item();
                }
                p_renderer->add_stack_item(stack.get_items().back());
            }
            else {
                p_renderer->replace_stack_items(stack.get_items());
            }
        }
        swap_stacks();
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
    CowVec<Real> values;
    CowVec<StackItem> source_value = stack.pop_items(1);

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
        create_displayables_from(source_value[0].p_input, ".", pattern),
        std::move(result),
        true
    };

    p_renderer->add_stack_item(stack_item);
    stack.push_item(std::move(stack_item));
    return true;
}


void Application::swap_stacks() {
    *p_stack_backup = stack;
    std::swap(p_stack_backup, p_stack_active);
}


}