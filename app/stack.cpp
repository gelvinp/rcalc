#include "stack.h"

#include <sstream>

namespace RCalc {

void RPNStack::push_item(StackItem&& item) {
    stack.emplace_back(std::move(item));
}


void RPNStack::push_items(std::vector<StackItem>&& items) {
    for (auto it = items.rbegin(); it < items.rend(); ++it) {
        stack.push_back(std::move(*it));
    }
}


std::string RPNStack::peek_types(uint64_t count) const {
    uint64_t safe_count = std::min(count, stack.size());

    std::stringstream ss;
    for (uint64_t idx = safe_count; idx > 0; --idx) {
        if (idx < safe_count) { ss << '_'; }
        ss << (stack[stack.size() - idx].result.get_type_name());
    }

    std::string str;
    ss >> str;

    return str;
}


size_t RPNStack::size() const { return stack.size(); }


std::vector<StackItem> RPNStack::pop_items(uint64_t count) {
    uint64_t safe_count = std::min(count, stack.size());

    std::vector<StackItem> items;
    for (uint64_t idx = safe_count; idx > 0; --idx) {
        items.push_back(std::move(stack[stack.size() - idx]));
    }
    stack.resize(stack.size() - safe_count);

    return items;
}


void RPNStack::clear() {
    stack.clear();
}


const std::vector<StackItem>& RPNStack::get_items() const { return stack; }


std::string StackItem::get_input_formatted() const {
    if (input_is_expression) {
        return "(" + input + ")";
    } else {
        return input;
    }
}



}