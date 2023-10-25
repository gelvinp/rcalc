#pragma once

#include "app/operators/operators.h"
#include "app/stack.h"
#include "display_stack.h"

#include <string>
#include <vector>


namespace RCalc {

struct CachedOperator {
    const Operator& op;
    std::vector<ImGuiDisplayEntry> examples;
    std::string id;

    CachedOperator(const Operator& op, RPNStack& stack);
};

struct CachedOperatorCategory {
    std::optional<const char* const> category_name;
    std::vector<CachedOperator> category_ops;

    CachedOperatorCategory(const OperatorCategory& category, RPNStack& stack);
};

}