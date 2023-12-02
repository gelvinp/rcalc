#include "autocomplete.h"

#include "app/application.h"
#include "app/operators/operators.h"
#include "app/commands/commands.h"
#include "core/format.h"

#include <algorithm>


namespace RCalc {

void AutocompleteManager::init_suggestions(const std::string_view& str) {
    if (active_auto) {
        active_auto.value()->cancel_suggestion();
    }

    const RPNStack& stack = Application::get_stack();

    if (str[0] == '\\') {
        cmd_auto.init_suggestions(str);
        active_auto = &cmd_auto;
    }
    else if (str[0] == '_') {
        unit_auto.init_suggestions(str, stack);
        active_auto = &unit_auto;
    }
    else {
        op_auto.init_suggestions(str, stack.peek_types_vec(stack.size()));
        active_auto = &op_auto;
    }
}


std::optional<std::string> AutocompleteManager::get_next_suggestion() {
    if (active_auto) {
        return active_auto.value()->get_next_suggestion();
    }
    return std::nullopt;
}


std::optional<std::string> AutocompleteManager::get_previous_suggestion() {
    if (active_auto) {
        return active_auto.value()->get_previous_suggestion();
    }
    return std::nullopt;
}


void AutocompleteManager::cancel_suggestion() {
    if (active_auto) {
        active_auto.value()->cancel_suggestion();
        active_auto = std::nullopt;
    }
}


std::optional<std::string> AutocompleteManager::Autocomplete::get_next_suggestion() {
    if (!suggestion_index) { return std::nullopt; }

    std::string str { suggestions[suggestion_index.value()].data() };
    std::transform(str.begin(), str.end(), str.begin(), [](const unsigned char ch) { return (char)std::tolower(ch); });

    suggestion_index = (suggestion_index.value() + 1) % suggestions.size();

    return str;
}


std::optional<std::string> AutocompleteManager::Autocomplete::get_previous_suggestion() {
    if (!suggestion_index) { return std::nullopt; }

    if (suggestion_index.value() == 0) {
        suggestion_index = std::max((size_t)2, suggestions.size()) - 2;
    }
    else if (suggestion_index.value() == 1) {
        suggestion_index = suggestions.size() - 1;
    }
    else {
        suggestion_index = suggestion_index.value() - 2;
    }

    std::string str { suggestions[suggestion_index.value()].data() };
    std::transform(str.begin(), str.end(), str.begin(), [](const unsigned char ch) { return (char)std::tolower(ch); });

    suggestion_index = (suggestion_index.value() + 1) % suggestions.size();

    return str;
}


void AutocompleteManager::Autocomplete::cancel_suggestion() {
    suggestions.clear();
    suggestion_index = std::nullopt;
}


// Command Autocomplete
// TODO: Eventually, we'll need a way to know which command scopes are *active* at runtime

void AutocompleteManager::CommandAutocomplete::init_suggestions(std::string_view str) {
    anycase_stringview input(str.begin() + 1, str.end());
    suggestions.clear();

    for (const CommandMeta* cmd : _GlobalCommandMap::get_alphabetical()) {
        anycase_stringview name(cmd->name);
        if (name.starts_with(input)) {
            suggestions.push_back(name);
        }

        for (const char* p_alias : cmd->aliases) {
            anycase_stringview alias(p_alias);
            if (alias.starts_with(input)) {
                suggestions.push_back(alias);
            }
        }
    }

    std::stable_sort(suggestions.begin(), suggestions.end(), [](const anycase_stringview& a, const anycase_stringview& b) {
        return a.size() < b.size();
    });

    if (suggestions.empty()) {
        suggestion_index = std::nullopt;
    }
    else {
        suggestion_index = 0;
    }
}

std::optional<std::string> AutocompleteManager::CommandAutocomplete::get_next_suggestion() {
    if (!suggestion_index) { return std::nullopt; }

    std::string str = "\\";
    str.append(suggestions[suggestion_index.value()].data());
    std::transform(str.begin(), str.end(), str.begin(), [](const unsigned char ch) { return (char)std::tolower(ch); });

    suggestion_index = (suggestion_index.value() + 1) % suggestions.size();

    return str;
}

std::optional<std::string> AutocompleteManager::CommandAutocomplete::get_previous_suggestion() {
    if (!suggestion_index) { return std::nullopt; }

    if (suggestion_index == 0) {
        suggestion_index = suggestions.size() - 2;
    }
    else if (suggestion_index == 1) {
        suggestion_index = suggestions.size() - 1;
    }
    else {
        suggestion_index = suggestion_index.value() - 2;
    }

    std::string str = "\\";
    str.append(suggestions[suggestion_index.value()].data());
    std::transform(str.begin(), str.end(), str.begin(), [](const unsigned char ch) { return (char)std::tolower(ch); });

    suggestion_index = (suggestion_index.value() + 1) % suggestions.size();

    return str;
}


// Operator Autocomplete

void AutocompleteManager::OperatorAutocomplete::init_suggestions(std::string_view str, const std::vector<Type>& stack_types) {
    anycase_stringview input(str.begin(), str.end());
    suggestions.clear();

    if (stack_types.empty()) { return; }

    for (const OperatorCategory* category : OperatorMap::get_operator_map().get_alphabetical()) {
        for (const Operator* op : category->category_ops) {
            if (op->param_count > stack_types.size()) { continue; }

            if (!stack_types.empty()) {
                auto stack_range = std::ranges::subrange(stack_types.end() - op->param_count, stack_types.end());

                auto it = std::find_if(op->allowed_types.begin(), op->allowed_types.end(), [&stack_range](const std::span<const Type>& op_types) {
                    auto op_range = std::ranges::subrange(op_types.begin(), op_types.end());
                    return std::ranges::equal(stack_range, op_range);
                });

                if (it == op->allowed_types.end()) {
                    continue;
                }
            }

            anycase_stringview name(op->name);
            if (name.starts_with(input)) {
                suggestions.push_back(name);
            }
        }
    }

    std::stable_sort(suggestions.begin(), suggestions.end(), [](const anycase_stringview& a, const anycase_stringview& b) {
        return a.size() < b.size();
    });

    if (suggestions.empty()) {
        suggestion_index = std::nullopt;
    }
    else {
        suggestion_index = 0;
    }
}


// Unit Autocomplete

void AutocompleteManager::UnitAutocomplete::init_suggestions(std::string_view str, const RPNStack& stack) {
    anycase_stringview input(str.begin(), str.end());
    suggestions.clear();

    std::vector<Type> stack_types = stack.peek_types_vec(stack.size());

    if (stack_types.empty()) { return; }

    std::optional<Type> type = std::nullopt;
    std::optional<const UnitFamily*> matchingFamily = std::nullopt;

    if (!stack_types.empty() && stack_types.back() != TYPE_UNIT) {
        type = stack_types.back();
    }
    else if (stack_types.size() >= 2 && stack_types[stack_types.size() - 2] != TYPE_UNIT) {
        type = stack_types[stack_types.size() - 2];

        if (stack_types[stack_types.size() - 1] != TYPE_UNIT) {
            return;
        }

        matchingFamily = stack.get_items()[stack.size() - 1].result.operator RCalc::Unit().p_family;
    }


    for (const UnitFamily* family : UnitsMap::get_units_map().get_alphabetical()) {
        if (type && !is_type_castable(type.value(), family->base_type)) { continue; }
        if (matchingFamily && matchingFamily.value() != family) { continue; }
        
        for (const Unit* unit : family->units) {
            anycase_stringview usage(unit->p_usage);
            if (usage.starts_with(input)) {
                suggestions.push_back(usage);
            }
        }
    }

    std::stable_sort(suggestions.begin(), suggestions.end(), [](const anycase_stringview& a, const anycase_stringview& b) {
        return a.size() < b.size();
    });

    if (suggestions.empty()) {
        suggestion_index = std::nullopt;
    }
    else {
        suggestion_index = 0;
    }
}

}
