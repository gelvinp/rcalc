#pragma once

#include "app/stack.h"
#include "core/error.h"

#include <functional>
#include <string>
#include <map>
#include <optional>
#include <vector>


namespace RCalc {

struct Operator {
    const char* name;
    const char* description;
    uint64_t param_count;
    std::vector<std::vector<Value::Type>> allowed_types;
    std::vector<std::vector<const char*>> examples;
    std::function<Result<>(RPNStack&, const Operator&)> evaluate;

    const std::vector<StackItem>& get_examples();
};

struct OperatorCategory {
    std::optional<const char* const> category_name;
    const std::vector<Operator const *>& category_ops;
};

class OperatorMap {
public:
    static OperatorMap& get_operator_map();
    bool has_operator(const std::string& str) const;
    Result<> evaluate(const std::string& str, RPNStack& stack) const;
    const std::vector<OperatorCategory const *>& get_alphabetical() const;

private:
    bool built = false;
    void build();

    std::string filter_name(const char* p_str) const;

    static OperatorMap singleton;
};

}