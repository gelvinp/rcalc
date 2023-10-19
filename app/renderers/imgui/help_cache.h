#pragma once

#include "app/operators/operators.h"
#include "app/stack.h"

#include <string>
#include <vector>


namespace RCalc {

struct CachedOperator {
    const Operator& op;
    std::vector<std::string> examples;

    CachedOperator(const Operator& op, RPNStack& stack);
};

struct CachedOperatorCategory {
    std::optional<const char* const> category_name;
    std::vector<CachedOperator> category_ops;

    CachedOperatorCategory(const OperatorCategory& category, RPNStack& stack);
};

}