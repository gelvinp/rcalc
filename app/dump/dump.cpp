#include "dump.h"

#include "core/version.h"
#include "app/help_text.h"
#include "app/operators/operators.h"
#include "app/commands/commands.h"
#include "core/units/units.h"
#include "app/stack.h"
#include "core/filter.h"
#include "assets/license.gen.h"

#include <fstream>
#include <iostream>

#include "modules/nlohmann/json.hpp"
using json = nlohmann::json;


void RCalc::Dump::dump_info() {
    json info = json::object();

    // Statistics
    size_t op_category_count = 0;
    size_t op_count = 0;
    size_t cmd_scope_count = 0;
    size_t cmd_count = 0;
    size_t unit_family_count = 0;
    size_t unit_count = 0;

    // General info
    json general = json::object();

    general["version"] = VERSION_FULL_BUILD;
    general["git_hash"] = VERSION_HASH;

    info["general"] = general;

    // Help text
    json help = json::object();

    help["program_info"] = RCalc::HelpText::program_info;
    
    json help_sections = json::array();

    for (const RCalc::HelpText::HelpSection& section : RCalc::HelpText::sections) {
        help_sections.push_back({
            { "header", section.header },
            { "text", section.text }
        });
    }

    help["help_sections"] = help_sections;

    info["help"] = help;

    // Operators
    json op_categories = json::array();
    RPNStack stack;
    RCalc::OperatorMap& op_map = RCalc::OperatorMap::get_operator_map();

    for (RCalc::OperatorCategory const * const category : op_map.get_alphabetical()) {
        op_category_count += 1;
        json op_category = json::object();

        if (category->category_name) {
            op_category["name"] = category->category_name.value();
        }
        else {
            op_category["name"] = nullptr;
        }

        json ops = json::array();

        for (RCalc::Operator const * const op : category->category_ops) {
            op_count += 1;
            json allowed_types = json::array();

            for (const std::span<const RCalc::Type>& types : op->allowed_types) {
                json call_types = json::array();

                for (const RCalc::Type& type : types) {
                    call_types.push_back(RCalc::Value::get_type_name(type));
                }

                allowed_types.push_back(call_types);
            }

            json renderered_examples = json::array();

            for (const std::span<const char * const>& example : op->examples) {
                json rendered_example = json::object();
                stack.clear();

                json example_params = json::array();

                for (const char* const param : example) {
                    example_params.push_back(param);

                    Value value = Value::parse(param).value();
                    stack.push_item(StackItem { create_displayables_from(value), std::move(value), false });
                }

                rendered_example["params"] = example_params;

                std::string op_name = filter_name(op->name);
                Result<std::optional<size_t>> err = op_map.evaluate(op_name, stack);

                if (!err) {
                    Logger::log_err("Cannot display example: %s", err.unwrap_err().get_message().c_str());
                    continue;
                }

                json example_outputs = json::array();
                json example_outputs_formatted = json::array();

                for (const StackItem& item : stack.get_items()) {
                    example_outputs.push_back(item.result.to_string(DisplayableTag::ONE_LINE));
                    example_outputs_formatted.push_back(item.result.to_string(item.p_input->tags));
                }

                CowVec<StackItem> _items = stack.pop_items(1);
                const StackItem& res = _items[0];

                json example_params_formatted = json::array();

                for (const Displayable& disp : *res.p_input) {
                    std::string str;

                    switch (disp.get_type()) {
                        case Displayable::Type::CONST_CHAR: {
                            str = reinterpret_cast<const ConstCharDisplayable&>(disp).p_char;
                            break;
                        }
                        case Displayable::Type::STRING: {
                            str = reinterpret_cast<const StringDisplayable&>(disp).str;
                            break;
                        }
                        case Displayable::Type::VALUE: {
                            str = reinterpret_cast<const ValueDisplayable&>(disp).value.to_string(disp.tags);
                            break;
                        }
                        case Displayable::Type::RECURSIVE:
                            UNREACHABLE(); // Handled by the iterator
                    }

                    example_params_formatted.push_back(str);
                }

                rendered_example["outputs"] = example_outputs;
                rendered_example["formatted_params"] = example_params_formatted;
                rendered_example["formatted_outputs"] = example_outputs_formatted;

                renderered_examples.push_back(rendered_example);
            }

            ops.push_back({
                { "name", op->name },
                { "description", op->description },
                { "param_count", op->param_count },
                { "allowed_types", allowed_types },
                { "examples", renderered_examples }
            });
        }

        op_category["operators"] = ops;

        op_categories.push_back(op_category);
    }

    info["operator_categories"] = op_categories;

    json command_scopes = json::array();

    for (ScopeMeta const * const compiled_scope : RCalc::_GlobalCommandMap::get_compiled_alphabetical()) {
        cmd_scope_count += 1;
        json command_scope = json::object();

        command_scope["name"] = compiled_scope->scope_name;

        json commands = json::array();

        for (CommandMeta const * const compiled_command : compiled_scope->scope_cmds) {
            cmd_count += 1;
            json command = json::object();

            command["name"] = compiled_command->name;
            command["description"] = compiled_command->description;

            json aliases = json::array();

            for (const char * const alias : compiled_command->aliases) {
                aliases.push_back(alias);
            }
            
            command["aliases"] = aliases;

            commands.push_back(command);
        }

        command_scope["commands"] = commands;

        command_scopes.push_back(command_scope);
    }

    info["command_scopes"] = command_scopes;

    json unit_families = json::array();

    for (RCalc::UnitFamily const * const compiled_family : RCalc::UnitsMap::get_units_map().get_alphabetical()) {
        unit_family_count += 1;
        json unit_family = json::object();

        unit_family["name"] = compiled_family->p_name;
        unit_family["base_type"] = RCalc::Value::get_type_name(compiled_family->base_type);

        json units = json::array();

        for (RCalc::Unit const * const unit : compiled_family->units) {
            unit_count += 1;
            units.push_back(json::object({
                { "name", unit->p_name },
                { "usage", unit->p_usage }
            }));
        }

        unit_family["units"] = units;

        unit_families.push_back(unit_family);
    }

    info["unit_families"] = unit_families;

    info["statistics"] = json::object({
        { "operator_category_count", op_category_count },
        { "operator_count", op_count },
        { "operator_manual_impl_count", op_map.stat_manual_impl_count },
        { "operator_total_impl_count", op_map.stat_total_impl_count },
        { "command_scope_count", cmd_scope_count },
        { "command_count", cmd_count },
        { "unit_family_count", unit_family_count },
        { "unit_count", unit_count },
    });

    info["licenses"] = RCalc::Assets::license_md;

    std::ofstream file {
        "rcalc_dump.json",
        std::ofstream::out | std::ofstream::trunc
    };

    file << info.dump(1);

    file.close();
}