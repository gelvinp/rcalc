#pragma once

#include "core/value.h"

#include <optional>
#include <string>
#include <vector>

namespace RCalc {


struct StackItem {
    std::string input;
    bool input_is_expression; // When composing inputs, expressions should be wrapped in ()'s

    std::string output;
    Value result;
};

class RPNStack {
public:
    void push_item(StackItem&& item);
    std::vector<Value::Type> peek_types(uint64_t count) const;
    std::vector<StackItem> pop_items(uint64_t count);
    void clear();

    const std::vector<StackItem>& get_items() const;

private:
    std::vector<StackItem> stack;
};

}