#pragma once

#include "app/stack.h"
#include "core/anycase_str.h"

#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>


namespace RCalc {

class AutocompleteManager {
public:
    void init_suggestions(const std::string_view& str);
    std::optional<std::string> get_next_suggestion();
    void cancel_suggestion();

    bool suggestions_active() const { return active_auto.has_value(); }

private:
    class Autocomplete {
    public:
        virtual void init_suggestions(std::string_view str) = 0;
        virtual std::optional<std::string> get_next_suggestion() = 0;
        virtual void cancel_suggestion() = 0;

    protected:
        std::vector<anycase_stringview> suggestions = {};
        std::optional<size_t> suggestion_index = std::nullopt;
    };

    class CommandAutocomplete : public Autocomplete {
    public:
        virtual void init_suggestions(std::string_view str) override;
        virtual std::optional<std::string> get_next_suggestion() override;
        virtual void cancel_suggestion() override;
    };

    class OperatorAutocomplete : public Autocomplete {
    public:
        virtual void init_suggestions(std::string_view str) override;
        virtual std::optional<std::string> get_next_suggestion() override;
        virtual void cancel_suggestion() override;
    };

    class UnitAutocomplete : public Autocomplete {
    public:
        virtual void init_suggestions(std::string_view str) override;
        virtual std::optional<std::string> get_next_suggestion() override;
        virtual void cancel_suggestion() override;
    };

    CommandAutocomplete cmd_auto = {};
    OperatorAutocomplete op_auto = {};
    UnitAutocomplete unit_auto = {};

    std::optional<Autocomplete*> active_auto = std::nullopt;
};

}