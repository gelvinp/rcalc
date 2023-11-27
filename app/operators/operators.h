#pragma once

#include "app/stack.h"
#include "core/error.h"

#include <functional>
#include <string>
#include <map>
#include <optional>
#include <span>


namespace RCalc {

struct Operator {
    const char* name;
    const char* description;
    uint64_t param_count;
    const std::span<const std::span<const Type>> allowed_types;
    const std::span<const std::span<const char * const>> examples;
    Result<>(*evaluate)(RPNStack&, const Operator&);

    const std::vector<StackItem>& get_examples();
};

struct OperatorCategory {
    std::optional<const char* const> category_name;
    const std::span<Operator const * const> category_ops;
};

class OperatorMap {
public:
    static OperatorMap& get_operator_map();
    bool has_operator(const std::string& str);
    Result<> evaluate(const std::string& str, RPNStack& stack);
    const std::span<OperatorCategory const * const> get_alphabetical() const;

private:
    bool built = false;
    void build();

    static OperatorMap singleton;
};

}