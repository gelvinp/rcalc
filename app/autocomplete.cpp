#include "autocomplete.h"

#include "app/application.h"
#include "app/operators/operators.h"
#include "app/commands/commands.h"
#include "core/format.h"


namespace RCalc {

void AutocompleteManager::init_suggestions(const std::string_view& str) {
    if (active_auto) {
        active_auto.value()->cancel_suggestion();
    }


    if (str[0] == '\\') {
        if (str.size() < 2) {
            active_auto = std::nullopt;
            return;
        }

        cmd_auto.init_suggestions(str);
        active_auto = &cmd_auto;
    }
    else if (str[0] == '_') {
        if (str.size() < 2) {
            active_auto = std::nullopt;
            return;
        }
        
        unit_auto.init_suggestions(str);
        active_auto = &unit_auto;
    }
    else {
        op_auto.init_suggestions(str);
        active_auto = &op_auto;
    }
}


std::optional<std::string> AutocompleteManager::get_next_suggestion() {
    if (active_auto) {
        return active_auto.value()->get_next_suggestion();
    }
    return std::nullopt;
}


void AutocompleteManager::cancel_suggestion() {
    if (active_auto) {
        active_auto.value()->cancel_suggestion();
        active_auto = std::nullopt;
    }
}


// Command Autocomplete
// TODO: Eventually, we'll need a way to know which command scopes are *active* at runtime

void AutocompleteManager::CommandAutocomplete::init_suggestions(std::string_view str) {
    anycase_stringview input(str.begin() + 1, str.end());
    suggestions.clear();

    for (const ScopeMeta* scope : _GlobalCommandMap::get_alphabetical()) {
        for (const CommandMeta* cmd : scope->scope_cmds) {
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
    std::transform(str.begin(), str.end(), str.begin(), [](const unsigned char ch) { return std::tolower(ch); });

    suggestion_index = (suggestion_index.value() + 1) % suggestions.size();

    return str;
}

void AutocompleteManager::CommandAutocomplete::cancel_suggestion() {
    suggestions.clear();
    suggestion_index = std::nullopt;
}


// Operator Autocomplete

void AutocompleteManager::OperatorAutocomplete::init_suggestions(std::string_view str) {
    anycase_stringview input(str.begin(), str.end());
    suggestions.clear();

    for (const OperatorCategory* category : OperatorMap::get_operator_map().get_alphabetical()) {
        for (const Operator* op : category->category_ops) {
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

std::optional<std::string> AutocompleteManager::OperatorAutocomplete::get_next_suggestion() {
    if (!suggestion_index) { return std::nullopt; }

    std::string str { suggestions[suggestion_index.value()].data() };
    std::transform(str.begin(), str.end(), str.begin(), [](const unsigned char ch) { return std::tolower(ch); });

    suggestion_index = (suggestion_index.value() + 1) % suggestions.size();

    return str;
}

void AutocompleteManager::OperatorAutocomplete::cancel_suggestion() {
    suggestions.clear();
    suggestion_index = std::nullopt;
}


// Unit Autocomplete

void AutocompleteManager::UnitAutocomplete::init_suggestions(std::string_view str) {
    anycase_stringview input(str.begin(), str.end());
    suggestions.clear();

    for (const UnitFamily* family : UnitsMap::get_units_map().get_alphabetical()) {
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

std::optional<std::string> AutocompleteManager::UnitAutocomplete::get_next_suggestion() {
    if (!suggestion_index) { return std::nullopt; }

    std::string str { suggestions[suggestion_index.value()].data() };
    std::transform(str.begin(), str.end(), str.begin(), [](const unsigned char ch) { return std::tolower(ch); });

    suggestion_index = (suggestion_index.value() + 1) % suggestions.size();

    return str;
}

void AutocompleteManager::UnitAutocomplete::cancel_suggestion() {
    suggestions.clear();
    suggestion_index = std::nullopt;
}

}