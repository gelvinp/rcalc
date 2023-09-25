#include "application.h"

#include "core/logger.h"
#include "imgui_stdlib.h"

#include <algorithm>
#include <cmath>
#include <ranges>
#include <sstream>

const float STACK_HORIZ_PADDING = 8.0;
const float STACK_SCROLLBAR_COMPENSATION = 16.0;

const char* HELP_TEXT = R"helptext(
Recognized commands:
    \q \quit    Quit RCalc
    \clear      Clear the stack
    \help       Print this help message

Recognized operators:
    +, -, *, /  Operates on two parameters
)helptext";


namespace RCalc {

Application::Application() :
    renderer(Renderer(
        std::bind(&Application::on_renderer_submit_text, this, std::placeholders::_1),
        std::bind(&Application::on_renderer_submit_operator, this, std::placeholders::_1))
) {
    renderer.display_info("Welcome to RCalc! Type \\help to see what commands and operators are supported.");

    AppCommand quit_command = std::bind(&Application::appcmd_quit, this);
    app_commands.insert_or_assign("\\quit", quit_command);
    app_commands.insert_or_assign("\\q", quit_command);

    AppCommand clear_command = std::bind(&Application::appcmd_clear, this);
    app_commands.insert_or_assign("\\clear", clear_command);

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
        stack.push_item(StackItem{str, value.value().to_string(), std::move(value.value()), false});
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


bool Application::try_application_command(const std::string& str) {
    if (app_commands.contains(str)) {
        app_commands.at(str)();
        return true;
    }

    return false;
}


    /*
    Platform& platform = Platform::get_singleton();

    // Fullscreen window
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    if (!ImGui::Begin(
        "RPN Calculator",
        nullptr,
        ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_MenuBar
    )) {
        ImGui::End();
        Logger::log_err("Failed to render window!");
        return;
    }

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Copy Answer", "Ctrl+C", &copy_answer);
            ImGui::MenuItem("Quit", "Ctrl+Q", &platform.close_requested);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    ImVec2 window_size = ImGui::GetWindowSize();
    if (window_size.x != last_window_size.x || window_size.y != last_window_size.y) {
        Logger::log_info("Resizing window: %fx%f", window_size.x, window_size.y);

        for (StackItem& item : stack) {
            item.recalculate_size();
        }
    }
    last_window_size = window_size;

    // Calculate heights
    float desired_stack_height = 0.0;
    if (!stack.empty()) {
        for (const StackItem& item : stack) {
            desired_stack_height += item.display_height;
        }

        desired_stack_height += ImGui::GetStyle().ItemSpacing.y * (stack.size() - 1);
    }

    const float input_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    const float max_stack_height = ImGui::GetContentRegionAvail().y - input_height - ImGui::GetStyle().ItemSpacing.y;

    const float stack_height = std::min(max_stack_height, desired_stack_height);
    const float stack_offset = max_stack_height - stack_height;
    const float separator_size = stack_offset;

    ImGui::BeginChild("##separator", ImVec2(0, separator_size));
    ImGui::EndChild();

    if (!stack.empty()) {
        if (ImGui::BeginChild("##stack", ImVec2(0, stack_height))) {
            int index = 0;

            for (const StackItem& item : stack) {

                ImGui::PushTextWrapPos(item.input_display_width);
                ImGui::TextUnformatted(item.input.c_str());
                ImGui::PopTextWrapPos();

                if (item.output) {
                    ImGui::SameLine(item.input_display_width, STACK_HORIZ_PADDING);
                    ImGui::PushTextWrapPos(item.input_display_width + STACK_HORIZ_PADDING + item.output_display_width);
                    if (item.is_error) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), item.output->c_str());
                    }
                    else {
                        ImGui::TextUnformatted(item.output->c_str());
                    }
                    ImGui::PopTextWrapPos();

                    std::string item_id = std::format("##item-{:d}", index++);
                    if (ImGui::BeginPopupContextItem(item_id.c_str())) {
                        if (ImGui::Button("Copy to Clipboard")) {
                            platform.copy_to_clipboard(item.output.value());
                            ImGui::CloseCurrentPopup();
                        }

                        ImGui::EndPopup();
                    }
                }
            }
        }

        if (stack_needs_scroll_down) {
            ImGui::SetScrollHereY(1.0);
            stack_needs_scroll_down = false;
        }

        ImGui::EndChild();
    }

    ImGui::Separator();

    // Scratchpad

    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::InputText(
        "##input",
        &scratchpad_text,
        ImGuiInputTextFlags_EnterReturnsTrue
        | ImGuiInputTextFlags_EscapeClearsAll)
    ) {
        process_input();
        stack_needs_scroll_down = true;
    }

    ImGui::SetItemDefaultFocus();
    
    if (input_needs_focus) {
        ImGui::SetKeyboardFocusHere(-1);
        input_needs_focus = false;
    }

    ImGui::End(); // Window

    // Handle shortcuts
    if (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_Q)) {
        platform.close_requested = true;
    }
    if (copy_answer || (ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyPressed(ImGuiKey_C))) {
        copy_answer = false;
        if (!stack.empty()) {
            platform.copy_to_clipboard(stack.back().output.value_or(std::string()));
        }
    }
}


void Application::process_input() {
    Logger::log_info("Entered: %s", scratchpad_text.c_str());

    // Try to parse as number first
    std::stringstream ss;
    ss << scratchpad_text;

    double d;
    ss >> d;

    if (ss && ss.eof()) {
        // Parsed entire string as number
        stack.emplace_back(scratchpad_text, display_number(d), d);
    } else {
        // Attempt to parse string as command / operand
        auto err = parse_command();
        if (err) {
            stack.emplace_back(scratchpad_text, err.value(), std::nullopt, true);
        }
    }

    scratchpad_text = "";
    input_needs_focus = true;
}


std::string Application::display_number(double number) {
    return std::format("{:g}", number);
}


std::optional<std::string> Application::parse_command() {
    size_t start = scratchpad_text.find_first_not_of(" \t\n\r");
    size_t end = scratchpad_text.find_last_not_of(" \t\n\r");
    std::string_view search(scratchpad_text.begin() + start, scratchpad_text.begin() + end + 1);

    Logger::log_info("Attempting to parse command: '%s'", search.cbegin());

    if (search.compare("\\q") == 0 || search.compare("\\quit") == 0) {
        Logger::log_info("Got command: quit");
        Platform::get_singleton().close_requested = true;
        return std::nullopt;
    }
    else if (search.compare("\\clear") == 0) {
        Logger::log_info("Got command: clear");
        stack.clear();
        return std::nullopt;
    }
    else if (search.compare("\\help") == 0) {
        Logger::log_info("Got command: help");
        stack.emplace_back(scratchpad_text, HELP_TEXT);
        return std::nullopt;
    }
    else if (search.compare("+") == 0) {
        Logger::log_info("Got command: +");
        auto args = pop_stack<2>();
        if (args.is_error || !args.values) {
            return args.error;
        }
        
        double result = args.values->at(0) + args.values->at(1);
        stack.emplace_back(scratchpad_text, display_number(result), result);
        return std::nullopt;
    }
    else if (search.compare("-") == 0) {
        Logger::log_info("Got command: -");
        auto args = pop_stack<2>();
        if (args.is_error || !args.values) {
            return args.error;
        }
        
        double result = args.values->at(0) - args.values->at(1);
        stack.emplace_back(scratchpad_text, display_number(result), result);
        return std::nullopt;
    }
    else if (search.compare("*") == 0) {
        Logger::log_info("Got command: *");
        auto args = pop_stack<2>();
        if (args.is_error || !args.values) {
            return args.error;
        }
        
        double result = args.values->at(0) * args.values->at(1);
        stack.emplace_back(scratchpad_text, display_number(result), result);
        return std::nullopt;
    }
    else if (search.compare("/") == 0) {
        Logger::log_info("Got command: /");
        auto args = pop_stack<2>();
        if (args.is_error || !args.values) {
            return args.error;
        }
        
        double result = args.values->at(0) / args.values->at(1);
        stack.emplace_back(scratchpad_text, display_number(result), result);
        return std::nullopt;
    }

    return "ERROR: Unknown command";
}


Application::StackItem::StackItem(std::string input, std::optional<std::string> output, std::optional<double> result, bool is_error, bool calculate_now) :
        input(input), output(output), result(result), is_error(is_error) {
    if (calculate_now)
        recalculate_size();
}


void Application::StackItem::recalculate_size() {
    float available_width = ImGui::GetContentRegionAvail().x - STACK_HORIZ_PADDING - STACK_SCROLLBAR_COMPENSATION;

    if (output && !output->empty()) {
        float output_max_size = available_width / 2.0;
        float output_desired_size = ImGui::CalcTextSize(output->c_str()).x;
        output_display_width = std::min(output_max_size, output_desired_size);
        ImVec2 output_actual_size = ImGui::CalcTextSize(output->c_str(), nullptr, false, output_display_width);

        input_display_width = available_width - output_actual_size.x;
        ImVec2 input_actual_size = ImGui::CalcTextSize(input.c_str(), nullptr, false, input_display_width);
        display_height = std::max(output_actual_size.y, input_actual_size.y);
    }
    else {
        input_display_width = available_width;
        ImVec2 input_actual_size = ImGui::CalcTextSize(input.c_str(), nullptr, false, input_display_width);
        display_height = input_actual_size.y;
    }
}
*/
}