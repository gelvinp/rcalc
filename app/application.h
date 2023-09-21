#pragma once

#include "core/logger.h"
#include "platform/platform.h"
#include "imgui.h"

#include <array>
#include <format>
#include <string>
#include <optional>
#include <vector>

namespace RCalc {

struct AppConfig {
    bool quiet = false;
    bool verbose = false;
};

class Application {
public:
    static void step();

private:
    static Application singleton;
    void _step();

    void process_input();
    std::string display_number(double number);
    std::optional<std::string> parse_command();

    struct StackItem {
        std::string input;
        std::optional<std::string> output;
        std::optional<double> result = std::nullopt;
        bool is_error = false;

        float input_display_width = 0.0;
        float output_display_width = 0.0;
        float display_height = 0.0;

        StackItem(std::string input, std::optional<std::string> output = std::nullopt, std::optional<double> result = std::nullopt, bool is_error = false, bool calculate_now = true);

        void recalculate_size();
    };

    bool input_needs_focus = true;
    bool stack_needs_scroll_down = true;
    bool copy_answer = false;
    
    std::string scratchpad_text = "";
    std::vector<StackItem> stack;
    ImVec2 last_window_size{};

    template<int count>
    struct StackPopResult {
        std::optional<std::array<double, count>> values;
        std::optional<std::string> error;
        bool is_error;
    };

    template<int count>
    StackPopResult<count> pop_stack(std::optional<std::string> (*validate)(double) = nullptr) {
        std::array<double, count> values;
        int index = 0;

        for (std::vector<StackItem>::const_iterator it = stack.end() - 1; it >= stack.begin(); --it) {
            if (it->is_error || !(it->result)) { Logger::log_info("Skipping error or non-result!"); continue; }

            Logger::log_info("Looking at %s", it->input.c_str());

            std::optional<std::string> err = std::nullopt;
            if (validate != nullptr) {
                err = validate(it->result.value());
            }

            if (err) {
                return StackPopResult<count>{ std::nullopt, err, true };
            }

            values[count - ++index] = it->result.value();

            if (index == count) { break; }
        }

        if (count != index) {
            return StackPopResult<count>{ std::nullopt, std::format("ERROR: Expected {:d} args, but got {:d}!", count, index), true };
        }

        return StackPopResult<count>{ values, std::nullopt, false };
    }
};

}