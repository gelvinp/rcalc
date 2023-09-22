#pragma once

#include "stack.h"
#include "imgui.h"

#include <functional>
#include <string>
#include <string_view>

namespace RCalc {

struct RenderItem {
    std::string_view input;
    std::string_view output;

    float input_display_width = 0.0;
    float output_display_width = 0.0;
    float display_height = 0.0;

    RenderItem(std::string_view input, std::string_view output)
        : input(input), output(output) {}

    void recalculate_size();
};

class Renderer {
public:
    typedef std::function<void(const std::string&)> SubmitTextCallback;
    typedef std::function<void(const std::string&)> SubmitOperatorCallback;

    Renderer(SubmitTextCallback cb_submit_text, SubmitOperatorCallback cb_submit_op) : cb_submit_text(cb_submit_text), cb_submit_op(cb_submit_op) {}

    void render(std::vector<RenderItem>& items);

    void display_info(const std::string& str);
    void display_error(const std::string& str);

private:
    SubmitTextCallback cb_submit_text;
    SubmitOperatorCallback cb_submit_op;

    std::string message;
    bool message_is_error = false;

    std::string scratchpad;
    ImVec2 last_window_size{};
    bool scratchpad_needs_focus = true;
    bool stack_needs_scroll_down = false;
    bool last_entry_needs_copy = false;
};

}