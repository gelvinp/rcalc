#pragma once

#include "app/stack.h"
#include "core/error.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>


namespace RCalc {

struct Operator {
    const char* name;
    const char* description;
    uint64_t param_count;
    std::function<Result<>(RPNStack&)> evaluate;
    std::vector<std::vector<Value::Type>> allowed_types;
};

typedef std::unordered_map<std::string, Operator const * const> OperatorMap;
OperatorMap get_operator_map();

}