#pragma once

#include "app/stack.h"
#include "core/error.h"
#include "app/variable_map.h"

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
    Result<std::optional<size_t>>(*evaluate)(RPNStack&, const Operator&, OptionalVariables variables);

    const std::vector<StackItem>& get_examples();
};

struct OperatorCategory {
    std::optional<const char* const> category_name;
    const std::span<Operator const * const> category_ops;
};

class OperatorMap {
public:
    static OperatorMap& get_operator_map();
    bool has_operator(std::string_view str);
    Result<std::optional<size_t>> evaluate(std::string_view str, RPNStack& stack, OptionalVariables variables = std::nullopt);
    const std::span<OperatorCategory const * const> get_alphabetical() const;

    static size_t stat_manual_impl_count;
    static size_t stat_total_impl_count;

private:
    bool built = false;
    void build();

    static OperatorMap singleton;
};

}