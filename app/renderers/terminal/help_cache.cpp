#include "help_cache.h"

#include "core/version.h"
#include "core/format.h"
#include "app/help_text.h"
#include "app/commands/commands.h"
#include "app/operators/operators.h"
#include "core/units/units.h"
#include "terminal_renderer.h"
#include "colors.h"
#include "app/stack.h"
#include "core/filter.h"
#include "assets/license.gen.h"

#include <cstring>
#include <string>
#include <sstream>

#include "scroll_component.h"

namespace RCalc::TerminalHelpCache {

ftxui::Component render_help_section(const HelpText::HelpSection& section);
ftxui::Component render_help_command(const CommandMeta* cmd);
ftxui::Component render_help_operator(const Operator* op);
ftxui::Component render_help_unit_family(const UnitFamily* family);


std::optional<ftxui::Color> gray_color;
std::optional<ftxui::Color> blue_color;
std::optional<ftxui::Color> green_color;
std::optional<ftxui::Color> yellow_color;


HelpText::HelpSection terminal_tips {
    "How to use the Terminal Renderer",
R"foo(The terminal renderer is a text-based interface for RCalc with a few interaction caveats:

If your terminal supports the mouse, you can use the scroll wheel to scroll through the stack and this help page. Additionally, on this help page, you can click on bolded dropdowns to twirl down or up argument type lists, examples, and units for operators and unit families.

If your terminal does not support the mouse, you can navigate through RCalc entirely with the keyboard. You can scroll through this help page with the Up/Down arrows, and you can scroll through the stack with Control + Up/Down arrows. (Up/Down without control cycles through scratchpad history.) You can also scroll by one screen at a time using Page Up/Down, or scroll to the top or bottom using Home/End. To change which dropdown is selected, you can use 'j' or Tab to move the selection down, 'k' or Shift + Tab to move the selection up, and Space/Enter to toggle the dropdown.

At the bottom of this help screen is the open source license info for this app and its components. Due to a bug with the terminal renderer, it may be cut off and not display in full. The most reliable way to view the full license info from the terminal is to run `rcalc --print-licenses`.

To exit this help screen, press Escape.
)foo"
};


ftxui::Component build_help_cache() {
    if (SUPPORTS_PALETTE(Palette16)) {
        if (SUPPORTS_PALETTE(TrueColor)) {
            gray_color = ftxui::Color(COLORS_TRUE[COLOR_GRAY][0], COLORS_TRUE[COLOR_GRAY][1], COLORS_TRUE[COLOR_GRAY][2]);
            blue_color = ftxui::Color(COLORS_TRUE[COLOR_BLUE][0], COLORS_TRUE[COLOR_BLUE][1], COLORS_TRUE[COLOR_BLUE][2]);
            green_color = ftxui::Color(COLORS_TRUE[COLOR_GREEN][0], COLORS_TRUE[COLOR_GREEN][1], COLORS_TRUE[COLOR_GREEN][2]);
            yellow_color = ftxui::Color(COLORS_TRUE[COLOR_YELLOW][0], COLORS_TRUE[COLOR_YELLOW][1], COLORS_TRUE[COLOR_YELLOW][2]);
        }
        else if (SUPPORTS_PALETTE(Palette256)) {
            gray_color = ftxui::Color(COLORS_256[COLOR_GRAY]);
            blue_color = ftxui::Color(COLORS_256[COLOR_BLUE]);
            green_color = ftxui::Color(COLORS_256[COLOR_GREEN]);
            yellow_color = ftxui::Color(COLORS_256[COLOR_YELLOW]);
        }
        else {
            gray_color = ftxui::Color(COLORS_16[COLOR_GRAY]);
            blue_color = ftxui::Color(COLORS_16[COLOR_BLUE]);
            green_color = ftxui::Color(COLORS_16[COLOR_GREEN]);
            yellow_color = ftxui::Color(COLORS_16[COLOR_YELLOW]);
        }
    }

    ftxui::Element version = ftxui::text(fmt(" v %s (%.6s)", VERSION_FULL_BUILD, VERSION_HASH));
    if (gray_color) { version |= ftxui::color(gray_color.value()); }

    ftxui::Component v_lines = ftxui::Container::Vertical({});

    v_lines->Add(ftxui::Renderer([version]{
        return ftxui::vbox({
            ftxui::hbox({
                ftxui::text("RCalc"),
                version
            }),
            ftxui::separatorEmpty(),
            ftxui::paragraph(HelpText::program_info),
            ftxui::separatorEmpty()
        });
    }));

    for (const HelpText::HelpSection& section : HelpText::sections) {
        v_lines->Add(render_help_section(section));
    }
    v_lines->Add(render_help_section(terminal_tips));

    v_lines->Add(ftxui::Renderer([]{
        return ftxui::vbox({
            ftxui::separatorEmpty(),
            ftxui::paragraph("Commands"),
            ftxui::separatorHeavy()
        });
    }));
    
    for (const ScopeMeta* scope : CommandMap<TerminalRenderer>::get_alphabetical()) {
        if (strcmp(scope->scope_name, "Application") != 0 && strcmp(scope->scope_name, "TerminalRenderer") != 0) { continue; }

        v_lines->Add(ftxui::Renderer([scope]{
            return ftxui::vbox({
                ftxui::separatorEmpty(),
                ftxui::paragraph(fmt("%s Commands", scope->scope_name)),
                ftxui::separatorLight()
            });
        }));

        for (const RCalc::CommandMeta* cmd : scope->scope_cmds) {
            v_lines->Add(render_help_command(cmd));
        }
    }

    v_lines->Add(ftxui::Renderer([]{
        return ftxui::vbox({
            ftxui::separatorEmpty(),
            ftxui::paragraph("Operators"),
            ftxui::separatorHeavy()
        });
    }));

    OperatorMap& op_map = OperatorMap::get_operator_map();

    for (const OperatorCategory* category : op_map.get_alphabetical()) {
        v_lines->Add(ftxui::Renderer([]{
            return ftxui::separatorEmpty();
        }));

        if (category->category_name) {
            v_lines->Add(ftxui::Renderer([category]{
                return ftxui::vbox({
                    ftxui::paragraph(fmt("%s Operators", category->category_name)),
                    ftxui::separatorLight()
                });
            }));
        }

        for (const Operator* op : category->category_ops) {
            v_lines->Add(render_help_operator(op));
        }
    }

    v_lines->Add(ftxui::Renderer([]{
        return ftxui::vbox({
            ftxui::separatorEmpty(),
            ftxui::paragraph("Unit Families"),
            ftxui::separatorHeavy()
        });
    }));

    for (const UnitFamily* family : UnitsMap::get_units_map().get_alphabetical()) {
        v_lines->Add(render_help_unit_family(family));
    }

    v_lines->Add(ftxui::Renderer([]{
        return ftxui::vbox({
            ftxui::separatorEmpty(),
            ftxui::paragraph("Licenses"),
            ftxui::separatorHeavy(),
            ftxui::separatorEmpty(),
            ftxui::vbox(TerminalRenderer::split_paragraphs(Assets::license_md)),
            ftxui::separatorEmpty()
        });
    }));

    return ScrollComponent::make(v_lines, false);
}




ftxui::Component render_help_section(const HelpText::HelpSection& section) {
    return ftxui::Renderer([&section]{
        return ftxui::vbox({
            ftxui::separatorEmpty(),
            ftxui::paragraph(section.header),
            ftxui::separator(),
            ftxui::vbox(TerminalRenderer::split_paragraphs(section.text)),
            ftxui::separatorEmpty()
        });
    });
}

ftxui::Component render_help_command(const CommandMeta* cmd) {
    ftxui::Elements lines;

    ftxui::Element cmd_name = ftxui::text(cmd->name);
    if (blue_color) { cmd_name |= ftxui::color(blue_color.value()); }

    ftxui::Elements cmd_elements { cmd_name };

    if (!cmd->aliases.empty()) {
        std::stringstream ss;
        ss << " aliases: [";

        bool first = true;
        for (const char* sig : cmd->aliases) {
            if (first) {
                ss << sig;
            }
            else {
                ss << ", " << sig;
            }
        }

        ss << "]";

        ftxui::Element cmd_aliases = ftxui::text(ss.str());
        if (gray_color) { cmd_aliases |= ftxui::color(gray_color.value()); }

        cmd_elements.push_back(cmd_aliases);
    }

    lines.push_back(ftxui::hbox(cmd_elements));
    lines.push_back(ftxui::paragraph(cmd->description));
    lines.push_back(ftxui::separatorEmpty());

    return ftxui::Renderer([lines = std::move(lines)]{ return ftxui::vbox(lines); });
}

ftxui::Component render_help_operator(const Operator* op) {
    ftxui::Component v_lines = ftxui::Container::Vertical({});
    
    ftxui::Element op_name = ftxui::text(op->name);
    if (green_color) { op_name |= ftxui::color(green_color.value()); }

    v_lines->Add(ftxui::Renderer([op, op_name = std::move(op_name)]{
        return ftxui::vbox({
            op_name,
            ftxui::paragraph(op->description)
        });
    }));

    if (op->param_count != 0) {
        ftxui::Elements allowed_types;
        for (const std::vector<Type>& set : op->allowed_types) {
            std::stringstream ss;
            ss << "   ";
            
            if (!allowed_types.empty()) { ss << "or "; }

            bool first = true;
            for (Type type : set) {
                if (first) {
                    ss << Value::get_type_name(type);
                    first = false;
                }
                else {
                    ss << ", " << Value::get_type_name(type);
                }
            }

            ftxui::Element set_element = ftxui::text(ss.str());
            if (gray_color) { set_element |= ftxui::color(gray_color.value()); }
            allowed_types.push_back(set_element);
        }

        ftxui::Component allowed_renderer =
            ftxui::Renderer([allowed_types = std::move(allowed_types)]{
                return ftxui::vbox(allowed_types);
            }
        );

        if (op->allowed_types.size() == 1) {
            v_lines->Add(ftxui::Collapsible(
                "Accepts 1 argument",
                allowed_renderer
            ));
        }
        else {
            v_lines->Add(ftxui::Collapsible(
                fmt("Accepts %lld arguments", (unsigned long long)op->param_count),
                allowed_renderer
            ));
        }
    }

    if (!op->examples.empty()) {
        ftxui::Elements examples;
        RPNStack example_stack;

        for (const std::vector<const char*>& example_params : op->examples) {
            example_stack.clear();

            if (!examples.empty()) {
                examples.push_back(ftxui::separatorEmpty());
            }

            for (const char* param : example_params) {
                Value value = Value::parse(param).value();
                example_stack.push_item(StackItem { create_displayables_from(value), std::move(value), false });
            }

            std::string op_name = filter_name(op->name);
            Result<> err = OperatorMap::get_operator_map().evaluate(op_name, example_stack);

            if (!err) {
                Logger::log_err("Cannot display example: %s", err.unwrap_err().get_message().c_str());
                continue;
            }

            bool first = true;
            std::stringstream ss;

            for (const StackItem& item : example_stack.get_items()) {
                if (first) {
                    first = false;
                }
                else {
                    ss << ", ";
                }

                ss << item.result.to_string();
            }

            std::vector<StackItem> _items = example_stack.pop_items(1);
            StackItem& res = _items[0];

            ftxui::Elements chunks { ftxui::text("   ") };

            for (Displayable& disp : *res.p_input) {
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
                    chunks.push_back(ftxui::text(str));
                }
                else {
                    // FTXUI ignores newlines in strings (even in paragraph blocks)
                    chunks.push_back(ftxui::vbox(TerminalRenderer::split_lines(str)));
                }
            }

            chunks.push_back(ftxui::separatorEmpty());
            chunks.push_back(ftxui::text("->"));
            chunks.push_back(ftxui::separatorEmpty());

            std::string output_str = ss.str();
            if (std::find(output_str.begin(), output_str.end(), '\n') == output_str.end()) {
                chunks.push_back(ftxui::vbox({ ftxui::filler(), ftxui::text(output_str) }));
            }
            else {
                // FTXUI ignores newlines in strings (even in paragraph blocks)
                chunks.push_back(ftxui::vbox(TerminalRenderer::split_lines(output_str)));
            }

            auto flexconf = ftxui::FlexboxConfig()
                .Set(ftxui::FlexboxConfig::AlignItems::Center);
            
            examples.push_back(ftxui::flexbox(chunks, flexconf));
        }

        ftxui::Component examples_comp =
            ftxui::Renderer([examples = std::move(examples)]{
                return ftxui::vbox(examples);
            }
        );

        v_lines->Add(ftxui::Collapsible(
            "Examples",
            examples_comp
        ));
    }

    v_lines->Add(ftxui::Renderer([]{
        return ftxui::separatorEmpty();
    }));

    return v_lines;
}

ftxui::Component render_help_unit_family(const UnitFamily* family) {
    ftxui::Component v_lines = ftxui::Container::Vertical({});

    ftxui::Element family_name = ftxui::text(family->p_name);
    if (yellow_color) { family_name |= ftxui::color(yellow_color.value()); }

    ftxui::Element base_unit = ftxui::text(fmt("Base type: %s", Value::get_type_name(family->base_type)));
    if (gray_color) { base_unit |= ftxui::color(gray_color.value()); }

    v_lines->Add(ftxui::Renderer([
        family_name = std::move(family_name),
        base_unit = std::move(base_unit)
    ]{
        return ftxui::vbox({
            family_name,
            base_unit
        });
    }));

    ftxui::Elements units;
    for (const Unit* unit : family->units) {
        ftxui::Element usage = ftxui::text(fmt(" (Usage: %s)", unit->p_usage));
        if (gray_color) { usage |= ftxui::color(gray_color.value()); }

        units.push_back(ftxui::hbox({
            ftxui::text("   "),
            ftxui::text(unit->p_name),
            usage
        }));
    }

    ftxui::Component units_renderer =
        ftxui::Renderer([units = std::move(units)]{
            return ftxui::vbox(units);
        }
    );

    v_lines->Add(ftxui::Collapsible(
        "Units",
        units_renderer
    ));

    v_lines->Add(ftxui::Renderer([]{
        return ftxui::separatorEmpty();
    }));

    return v_lines;
}


}