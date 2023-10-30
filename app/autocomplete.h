#pragma once

#include "app/stack.h"
#include "core/anycase_str.h"
#include "core/types.h"

#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>


namespace RCalc {

class AutocompleteManager {
public:
    void init_suggestions(const std::string_view& str, const std::vector<Type>& stack_types);
    std::optional<std::string> get_next_suggestion();
    std::optional<std::string> get_previous_suggestion();
    void cancel_suggestion();

    bool suggestions_active() const { return active_auto.has_value(); }

private:
    class Autocomplete {
    public:
        virtual std::optional<std::string> get_next_suggestion();
        virtual std::optional<std::string> get_previous_suggestion();
        void cancel_suggestion();

    protected:
        std::vector<anycase_stringview> suggestions = {};
        std::optional<size_t> suggestion_index = std::nullopt;
    };

    class CommandAutocomplete : public Autocomplete {
    public:
        void init_suggestions(std::string_view str);
        virtual std::optional<std::string> get_next_suggestion() override;
        virtual std::optional<std::string> get_previous_suggestion() override;
    };

    class OperatorAutocomplete : public Autocomplete {
    public:
        void init_suggestions(std::string_view str, const std::vector<Type>& stack_types);
    };

    class UnitAutocomplete : public Autocomplete {
    public:
        void init_suggestions(std::string_view str, const std::vector<Type>& stack_types);
    };

    CommandAutocomplete cmd_auto = {};
    OperatorAutocomplete op_auto = {};
    UnitAutocomplete unit_auto = {};

    std::optional<Autocomplete*> active_auto = std::nullopt;
};

}