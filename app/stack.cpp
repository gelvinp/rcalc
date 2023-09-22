#include "stack.h"

namespace RCalc {

void RPNStack::push_item(StackItem&& item) {
    stack.emplace_back(std::move(item));
}


std::vector<Value::Type> RPNStack::peek_types(uint64_t count) const {
    uint64_t safe_count = std::min(count, stack.size());

    std::vector<Value::Type> types;
    for (uint64_t idx = 1; idx <= safe_count; ++idx) {
        types.push_back(stack[stack.size() - idx].result.get_type());
    }

    return types;
}


std::vector<StackItem> RPNStack::pop_items(uint64_t count) {
    uint64_t safe_count = std::min(count, stack.size());

    std::vector<StackItem> items;
    for (uint64_t idx = 0; idx < safe_count; ++idx) {
        items.push_back(std::move(stack.back()));
        stack.pop_back();
    }

    return items;
}


void RPNStack::clear() {
    stack.clear();
}


const std::vector<StackItem>& RPNStack::get_items() const { return stack; }

}