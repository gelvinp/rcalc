#pragma once

#include "ftxui/component/component.hpp"
#include "app/operators/operators.h"

#include <optional>


namespace RCalc::TerminalHelpCache {

struct CachedOperator {
    const Operator& op;
    std::optional<ftxui::Elements> cached_examples;

    CachedOperator(const Operator& op) : op(op) {}

    void build();
};

struct CachedOperatorCategory {
    std::optional<const char* const> category_name;
    std::vector<CachedOperator> category_ops;

    CachedOperatorCategory(const OperatorCategory& category);
};

ftxui::Component build_help_cache(std::vector<CachedOperatorCategory>& cachedOperatorCategories);

}