#include "terminal_renderer.h"

#include "core/format.h"
#include "colors.h"
#include "entry_component.h"
#include "scroll_component.h"
#include "app/operators/operators.h"
#include "main/main.h"
#include "core/memory/allocator.h"

#include <algorithm>
#include <cstring>


namespace RCalc {

TerminalRenderer::TerminalRenderer() :
    command_map(CommandMap<TerminalRenderer>::get_command_map())
{
    command_map.activate(Main::get_app().get_command_map());
}


void TerminalRenderer::early_init(const AppConfig& config, SubmitTextCallback submit_text) {
    if (config.logfile_path) {
        std::shared_ptr<FileLogger> logger = Allocator::make_shared<FileLogger>();

        if (config.quiet) {
            logger->set_min_severity(RCalc::Logging::LOG_ERROR);
        } else if (config.verbose) {
            logger->set_min_severity(RCalc::Logging::LOG_VERBOSE);
        }

        if (logger->open_file(*config.logfile_path)) {
            Logger::set_global_engine(logger);
        }
        else {
            display_error("Unable to open logfile!");
        }
    }
    cb_submit_text = submit_text;
}

Result<> TerminalRenderer::init() {
    Result<> res = backend.init({ this, &TerminalRenderer::_display_error });
    if (!res) { return res; }

    res = settings.load();
    if (!res) {
        Logger::log("Unable to load settings!");
    }
    Value::set_precision(settings.precision);

    return Ok();
}


void TerminalRenderer::render_loop() {
    comp_filler = ftxui::Renderer([]{ return ftxui::filler(); });

    std::function<ftxui::Element()> render_stack_func = std::bind(&TerminalRenderer::render_stack, this);
    comp_stack = ftxui::Container::Vertical({});
    comp_stack_renderer = ftxui::Renderer(comp_stack, render_stack_func);

    std::function<ftxui::Element()> render_stack_scroll_func = std::bind(&TerminalRenderer::render_stack_scroll, this);
    comp_stack_scroll = ScrollComponent::make(comp_stack_renderer, true, true);
    comp_stack_scroll_renderer = ftxui::Renderer(comp_stack_scroll, render_stack_scroll_func);

    ftxui::InputOption scratchpad_options = ftxui::InputOption::Default();
    scratchpad_options.multiline = false;
    scratchpad_options.transform = [](ftxui::InputState state) {
        state.element |= ftxui::borderRounded;
        state.element |= ftxui::color(ftxui::Color::White);

        if (state.focused) {
            state.element |= ftxui::bgcolor(ftxui::Color::Black);
        }

        return state.element;
    };
    scratchpad_options.on_enter = std::bind(&TerminalRenderer::submit_scratchpad, this);
    scratchpad_options.on_change = std::bind(&TerminalRenderer::scratchpad_changed, this);

    comp_scratchpad = ftxui::Input(&scratchpad, scratchpad_options);

    comp_container = ftxui::Container::Vertical({});
    activate_main_page();

    std::function<bool(ftxui::Event)> event_func = std::bind(&TerminalRenderer::handle_event, this, std::placeholders::_1);
    comp_container |= ftxui::CatchEvent(event_func);

    std::function<ftxui::Element()> render_func = std::bind(&TerminalRenderer::render, this);
    backend.render_loop(ftxui::Renderer(comp_container, render_func));
}


void TerminalRenderer::activate_main_page() {
    comp_container->DetachAllChildren();
    
    comp_container->Add(comp_filler);
    comp_container->Add(comp_stack_scroll);
    comp_container->Add(comp_scratchpad);
    
    comp_scratchpad->TakeFocus();

    help_open = false;
    help_close_requested = false;
    settings_open = false;
    settings_close_requested = false;
    variables_open = false;
    variables_close_requested = false;
}


void TerminalRenderer::activate_help_page() {
    comp_container->DetachAllChildren();

    if (!help_cache) {
        if (help_op_cache.empty()) { build_help_cache(); }
        help_cache = TerminalHelpCache::build_help_cache(help_op_cache);
    }
    
    comp_container->Add(help_cache);

    help_open = true;
    help_requested = false;
    settings_open = false;
    settings_close_requested = false;
    variables_open = false;
    variables_close_requested = false;
    message = "";
}


void TerminalRenderer::activate_settings_page() {
    comp_container->DetachAllChildren();

    ftxui::SliderOption<int> slider_options {
        .value = &settings.precision,
        .min = 1,
        .max = std::numeric_limits<Real>::max_digits10,
        .increment = 1
    };

    ftxui::Component headers = ftxui::Renderer([this]{
        return ftxui::vbox({
            ftxui::text("Settings"),
            ftxui::separatorEmpty(),
            ftxui::text(fmt("Precision: %d", this->settings.precision)),
        });
    });
    
    ftxui::Component precision_slider = ftxui::Slider(slider_options);

    ftxui::Component save_button  = ftxui::Button("Save Changes", [this](){
        Result<> res = settings.save();
        if (!res) {
            Logger::log("Unable to save settings!");
        }
    }, ftxui::ButtonOption::Border());

    settings_page = ftxui::Container::Vertical({ headers, precision_slider, save_button });
    comp_container->Add(settings_page);

    settings_page->SetActiveChild(precision_slider);

    help_open = false;
    help_close_requested = false;
    settings_open = true;
    settings_requested = false;
    variables_open = false;
    variables_close_requested = false;
    message = "";
}


void TerminalRenderer::activate_variables_page() {
    comp_container->DetachAllChildren();

    variables_cache = TerminalVariablesCache::build_variables_cache(
        variables_data,
        cb_submit_text,
        variables_close_requested,
        backend
    );
    
    comp_container->Add(variables_cache);

    help_open = false;
    help_requested = false;
    settings_open = false;
    settings_close_requested = false;
    variables_open = true;
    variables_requested = false;
    message = "";
}


ftxui::Element TerminalRenderer::render() {
    if (help_requested && !help_open) {
        activate_help_page();
    }
    else if (settings_requested && !settings_open) {
        activate_settings_page();
    }
    else if (variables_requested && !variables_open) {
        activate_variables_page();
    }
    else if (help_close_requested || settings_close_requested || variables_close_requested) {
        activate_main_page();
    }

    if (help_open || settings_open || variables_open) {
        return comp_container->Render() | ftxui::border;
    }

    if (message.empty()) {
        return ftxui::vbox({
            comp_filler->Render(),
            comp_stack_scroll_renderer->Render(),
            comp_scratchpad->Render()
        });
    }
    
    ftxui::Element message_element;

    if (message.find('\n') == message.npos) {
      message_element = ftxui::text(message);
    } else {
      message_element = ftxui::vbox(split_lines(message));
    }

    if (message_is_error) {
        message_element |= ftxui::color(ftxui::Color::Red);
    }

    return ftxui::vbox({
        comp_filler->Render(),
        comp_stack_scroll_renderer->Render(),
        ftxui::separator(),
        message_element,
        comp_scratchpad->Render()
    });
}


ftxui::Element TerminalRenderer::render_stack() {
    if (comp_stack->ChildCount() == 0) {
        return ftxui::emptyElement();
    }

    ftxui::Elements entries;

    for (size_t idx = 0; idx < comp_stack->ChildCount(); ++idx) {
        if (idx != 0) {
            entries.push_back(ftxui::separatorEmpty());
        }

        std::optional<ftxui::Color> fg_color;
        std::optional<ftxui::Color> bg_color;

        if (queer_active && SUPPORTS_PALETTE(Palette16)) {
            size_t color_idx = idx % COLOR_GRAY;

            if (SUPPORTS_PALETTE(TrueColor)) {
                fg_color = ftxui::Color(COLORS_TRUE[color_idx][0], COLORS_TRUE[color_idx][1], COLORS_TRUE[color_idx][2]);

                if (color_idx >= COLOR_BROWN) {
                    bg_color = ftxui::Color(COLORS_TRUE[COLOR_GRAY][0], COLORS_TRUE[COLOR_GRAY][1], COLORS_TRUE[COLOR_GRAY][2]);
                }
            }
            else if (SUPPORTS_PALETTE(Palette256)) {
                fg_color = ftxui::Color(COLORS_256[color_idx]);

                if (color_idx >= COLOR_BROWN) {
                    bg_color = ftxui::Color(COLORS_256[COLOR_GRAY]);
                }
            }
            else {
                fg_color = ftxui::Color(COLORS_16[color_idx]);

                if (color_idx >= COLOR_BROWN) {
                    bg_color = ftxui::Color(COLORS_16[COLOR_GRAY]);
                }
            }
        }

        auto entry = reinterpret_cast<StackEntryComponent*>(comp_stack->ChildAt(idx).get())->RenderEntry(bg_color);

        if (fg_color) {
            entry |= ftxui::color(fg_color.value());
        }

        entries.push_back(entry);
    }

    return ftxui::vbox(entries);
}


ftxui::Element TerminalRenderer::render_stack_scroll() {
    return comp_stack_scroll->Render() | ftxui::yflex_shrink;
}


void TerminalRenderer::submit_scratchpad() {
    message = "";

    if (scratchpad.empty()) {
        cb_submit_text("\\dup");
        return;
    }

    history.push_back(scratchpad);
    history_state = std::nullopt;

    std::transform(scratchpad.begin(), scratchpad.end(), scratchpad.begin(), [](unsigned char c){ return (char)std::tolower(c); });
    cb_submit_text(scratchpad);
    scratchpad.clear();
}


void TerminalRenderer::scratchpad_changed() {
    autocomp.cancel_suggestion();
}


bool TerminalRenderer::handle_event(ftxui::Event event) {
    if (help_open) {
        if (event == ftxui::Event::Escape) {
            help_close_requested = help_open;
            return true;
        }

        return help_cache->OnEvent(event);
    }
    else if (settings_open) {
        if (event == ftxui::Event::Escape) {
            settings_close_requested = settings_open;
            Value::set_precision(settings.precision);
            return true;
        }

        return settings_page->OnEvent(event);
    }
    else if (variables_open) {
        if (event == ftxui::Event::Escape) {
            variables_close_requested = variables_open;
            return true;
        }

        return variables_cache->OnEvent(event);
    }
    else {
        if (event == ftxui::Event::F1) {
            help_requested = true;
            return true;
        }
        else if (event == ftxui::Event::F2) {
            variables_requested = true;
            return true;
        }
        else if (event == ftxui::Event::F12) {
            settings_requested = true;
            return true;
        }
    }

    if (event.is_character()) {
        switch (event.input()[0]) {
            case '+':
                if (!scratchpad.empty()) {
                    submit_scratchpad();
                }
                cb_submit_text("add");
                scratchpad.clear();
                return true;
            case '-':
                if (!scratchpad.empty()) {
                    submit_scratchpad();
                }
                cb_submit_text("sub");
                scratchpad.clear();
                return true;
            case '*':
                if (!scratchpad.empty()) {
                    submit_scratchpad();
                }
                cb_submit_text("mul");
                scratchpad.clear();
                return true;
            case '/':
                if (!scratchpad.empty()) {
                    submit_scratchpad();
                }
                cb_submit_text("div");
                scratchpad.clear();
                return true;
            default:
                return false;
        }
    }

    if (event == ftxui::Event::Delete && scratchpad.empty()) {
        cb_submit_text("\\pop");
        return true;
    }

    if (event == ftxui::Event::Backspace && autocomp.suggestions_active()) {
        autocomp.cancel_suggestion();
        scratchpad.clear();
        return true;
    }

    if (event == ftxui::Event::ArrowUp) {
        size_t next_state;

        if (history_state) {
            next_state = history_state.value() + 1;
            if (next_state > history.size()) { return true; }
        }
        else if (!history.empty()) {
            next_state = 1;
        }
        else { return true; }

        scratchpad = *(history.end() - next_state);

        history_state = next_state;
        return true;
    }
    if (event == ftxui::Event::ArrowDown) {
        std::optional<size_t> next_state;

        if (history_state) {
            next_state = history_state.value() - 1;
            if (next_state == 0) { next_state = std::nullopt; }
            if (next_state > history.size()) /* Shouldn't happen but who knows */ { next_state = std::nullopt; }
        }
        else { return true; }
        
        if (next_state) {
            scratchpad = *(history.end() - next_state.value());
        }
        else {
            scratchpad.clear();
        }

        history_state = next_state;
        return true;
    }

    if (event == ftxui::Event::Tab) {
        if (!autocomp.suggestions_active()) {
            autocomp.init_suggestions(scratchpad, Main::get_app());
        }

        std::optional<std::string> next = autocomp.get_next_suggestion();
        if (!next) { return true; }

        scratchpad = next.value();
        return true;
    }
    if (event == ftxui::Event::TabReverse) {
        if (!autocomp.suggestions_active()) {
            autocomp.init_suggestions(scratchpad, Main::get_app());
        }

        std::optional<std::string> next = autocomp.get_previous_suggestion();
        if (!next) { return true; }

        scratchpad = next.value();
        return true;
    }

    if (event == ftxui::Event::Special({22})) {
        scratchpad.append(backend.get_clipboard());
        return true;
    }

    return comp_stack_scroll->OnEvent(event);
}


void TerminalRenderer::cleanup() {
    backend.cleanup();
}


void TerminalRenderer::display_info(std::string_view str) {
    message = str;
    message_is_error = false;
}


void TerminalRenderer::display_error(std::string_view str) {
    message = str;
    message_is_error = true;
}


bool TerminalRenderer::try_renderer_command(std::string_view str) {   
    if (command_map.has_command(str)) {
        command_map.execute(str, *this);
        return true;
    }

    return false;
}


void TerminalRenderer::add_stack_item(const StackItem& item) {
    ftxui::Elements input_chunks;

    for (Displayable& disp : *item.p_input) {
        std::string str;

        switch (disp.get_type()) {
            case Displayable::Type::CONST_CHAR: {
                str = reinterpret_cast<ConstCharDisplayable&>(disp).p_char;
                break;
            }
            case Displayable::Type::STRING: {
                str = reinterpret_cast<StringDisplayable&>(disp).str;
                break;
            }
            case Displayable::Type::VALUE: {
                str = reinterpret_cast<ValueDisplayable&>(disp).value.to_string(disp.tags);
                break;
            }
            case Displayable::Type::RECURSIVE:
                UNREACHABLE(); // Handled by the iterator
        }

        if (std::find(str.begin(), str.end(), '\n') == str.end()) {
            input_chunks.push_back(ftxui::text(str));
        }
        else {
            // FTXUI ignores newlines in strings (even in paragraph blocks)
            input_chunks.push_back(ftxui::vbox(split_lines(str)));
        }
    }

    std::string output_str = item.result.to_string(item.p_input->tags);

    auto flexconf = ftxui::FlexboxConfig()
        .Set(ftxui::FlexboxConfig::AlignItems::Center)
        .Set(ftxui::FlexboxConfig::AlignContent::FlexEnd);
    
    auto input_flow = ftxui::flexbox(input_chunks, flexconf);

    comp_stack->Add(StackEntryComponent::make(std::move(input_chunks), std::move(output_str), item.result));

    comp_stack_scroll->OnEvent(ftxui::Event::End);
}


void TerminalRenderer::remove_stack_item() {
    comp_stack->ChildAt(comp_stack->ChildCount() - 1)->Detach();
    comp_stack_scroll->OnEvent(ftxui::Event::End);
}


void TerminalRenderer::replace_stack_items(const CowVec<StackItem>& items) {
    comp_stack->DetachAllChildren();
    std::for_each(items.begin(), items.end(), std::bind(&TerminalRenderer::add_stack_item, this, std::placeholders::_1));
}


ftxui::Elements TerminalRenderer::split_lines(std::string str) {
    const char* delims = "\n";
    char* ctx = nullptr;
    ftxui::Elements lines;

    char* token = strtok_p(str.data(), delims, &ctx);
    while (token) {
        lines.push_back(ftxui::text(std::string(token)));
        token = strtok_p(nullptr, delims, &ctx);
    }

    return lines;
}


ftxui::Elements TerminalRenderer::split_paragraphs(std::string str) {
    ftxui::Elements lines;

    // Handle newlines differently
    std::string::size_type loc = 0;
    std::string::size_type prev = 0;

    while ((loc = str.find('\n', prev)) != std::string::npos) {
        std::string line = str.substr(prev, loc - prev);

        if (line.empty()) {
            lines.push_back(ftxui::separatorEmpty());
        }
        else {
            lines.push_back(ftxui::paragraph(line));
        }

        prev = loc + 1;
    }
    lines.push_back(ftxui::paragraph(str.substr(prev, str.size() - prev)));

    return lines;
}


void TerminalRenderer::build_help_cache() {
    help_op_cache.clear();

    for (const OperatorCategory* category : OperatorMap::get_operator_map().get_alphabetical()) {
        help_op_cache.emplace_back(*category);
    }
}

}
