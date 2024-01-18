#include "application.h"

#include "core/logger.h"
#include "core/format.h"
#include "core/memory/allocator.h"

#include <algorithm>
#include <cmath>
#include <ranges>
#include <set>
#include <sstream>


namespace RCalc {

Result<Application*> Application::create(AppConfig config) {
    Application* p_application = Allocator::create<Application>();

    Result<Renderer*> renderer_res = Renderer::create(
        config,
        { p_application, &_on_renderer_submit_text }
    );

    if (renderer_res) {
        p_application->p_renderer = renderer_res.unwrap();
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

Result<> Application::init() {
    command_map.activate(cmd_map);

	Result<> res = p_renderer->init();
	if (!res) { return res; }

    return Ok();
}

void Application::run() {
    p_renderer->render_loop();
}


void Application::cleanup() {
    p_renderer->cleanup();
    Allocator::destroy(p_renderer);

    stack.clear();
    _stack_a.clear();
    _stack_b.clear();
}


void Application::on_renderer_submit_text(std::string_view str) {
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
        int size = snprintf(nullptr, 0, error_format, str.data()) + 1;
        std::string error;
        error.resize(size);
        snprintf(error.data(), error.size(), error_format, str.data());
        p_renderer->display_error(error);
        return;
    }

    // Try to parse as Value next
    std::optional<Value> value = Value::parse(str);
    if (value) {
        StackItem stack_item {
            create_displayables_from(value.value()),
            *value,
            value->is_negative()
        };

        p_renderer->add_stack_item(stack_item);
        stack.push_item(std::move(stack_item));
        swap_stacks();
        return;
    }

    // Try to parse as Operator third

    if (op_map.has_operator(str)) {
        Result<std::optional<size_t>> res = op_map.evaluate(str, stack);
        if (!res) {
            p_renderer->display_error(res.unwrap_err().get_message());
            return;
        }
        
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
    p_renderer->display_error(fmt("Unknown operator: '%s'", str.data()));
}


bool Application::try_application_command(std::string_view str) {
    if (command_map.has_command(str)) {
        command_map.execute(str, *this);
        return true;
    }

    return false;
}


bool Application::try_swizzle(std::string_view str) {
    // Swizzles take the pattern .rgba or .xyzw (or any combination)
    if (!str.starts_with('.')) { return false; }
    if (str.length() > 5) { return false; }

    if (stack.size() == 0) { return true; }

    std::string pattern { str.data() + 1, str.length() - 1 };
    std::transform(pattern.begin(), pattern.end(), pattern.begin(), [](unsigned char c){ return (char)std::tolower(c); });
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
                        p_renderer->display_error(fmt("Swizzle out of bounds for Vec2: '%s'", str.data()));
                        stack.push_items(std::move(source_value));
                        return true;
                    default:
                        p_renderer->display_error(fmt("Unrecognized swizzle: '%s'", str.data()));
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
                        p_renderer->display_error(fmt("Swizzle out of bounds for Vec3: '%s'", str.data()));
                        stack.push_items(std::move(source_value));
                        return true;
                    default:
                        p_renderer->display_error(fmt("Unrecognized swizzle: '%s'", str.data()));
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
                        p_renderer->display_error(fmt("Unrecognized swizzle: '%s'", str.data()));
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
        create_displayables_from(source_value[0].get_input_formatted(), ".", pattern),
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