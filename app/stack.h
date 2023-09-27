#pragma once

#include "core/value.h"

#include <optional>
#include <string>
#include <vector>

namespace RCalc {


struct StackItem {
    std::string input;
    std::string output;
    Value result;
    bool input_is_expression; // When composing inputs, expressions should be wrapped in ()'s

    std::string get_input_formatted() const; // Wraps with parenthesis if expression
};

class RPNStack {
public:
    void push_item(StackItem&& item);
    void push_items(std::vector<StackItem>&& items);

    size_t size() const;
    void clear();

    std::string peek_types(uint64_t count) const;
    std::vector<StackItem> pop_items(uint64_t count);

    const std::vector<StackItem>& get_items() const;

private:
    std::vector<StackItem> stack;
};

}