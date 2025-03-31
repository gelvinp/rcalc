#include "stack.h"
#include "core/logger.h"

#include <string>
#include <sstream>

namespace RCalc {


bool RPNStack::try_push_item(StackItem item) {
    if (max_size && (stack.size() + 1) > (size_t)*max_size) { return false; }
    stack.push_back(item);
    return true;
}


bool RPNStack::try_push_items(CowVec<StackItem> items) {
    if (max_size && (stack.size() + items.size()) > (size_t)*max_size) { return false; }
    for (auto it = items.begin(); it < items.end(); ++it) {
        stack.push_back(*it);
    }
    return true;
}

bool RPNStack::try_reserve_items(size_t count) {
    if (max_size && (stack.size() + count) > (size_t)*max_size) { return false; }
    stack.reserve(count);
    return true;
}


void RPNStack::set_max_size(std::optional<Int> new_max_size) {
    max_size = new_max_size;

    if (max_size && stack.size() > (size_t)*max_size) {
        for (size_t from_index = stack.size() - *max_size, to_index = 0; to_index < (size_t)*new_max_size; ++to_index, ++from_index) {
            stack.mut_at(to_index) = stack.at(from_index);
        }
        stack.resize(*new_max_size);
    }
}


std::string RPNStack::peek_types(uint64_t count) const {
    uint64_t safe_count = std::min((size_t)count, stack.size());

    std::stringstream ss;
    for (uint64_t idx = safe_count; idx > 0; --idx) {
        if (idx < safe_count) { ss << '_'; }
        ss << (stack[stack.size() - idx].result.get_type_name());
    }

    std::string str;
    ss >> str;

    return str;
}


CowVec<Type> RPNStack::peek_types_vec(uint64_t count) const {
    if (stack.size() < count) { return {}; }

    CowVec<Type> types;
    types.reserve(count);
    for (uint64_t idx = count; idx > 0; --idx) {
        types.push_back(stack[stack.size() - idx].result.get_type());
    }

    return types;
}


CowVec<Type> RPNStack::peek_types_vec(uint64_t count, OptionalVariables variables) const {
    if (stack.size() < count) { return {}; }

    CowVec<Type> types;
    types.reserve(count);
    for (uint64_t idx = count; idx > 0; --idx) {
        Value result = stack[stack.size() - idx].result;
        Type type = result.get_type();

        if (type == TYPE_IDENTIFIER && variables) {
            std::optional<Value> value = variables->get().load(result);
            if (value) {
                types.push_back(value->get_type());
                continue;
            }
        }

        types.push_back(type);
    }

    return types;
}


std::string RPNStack::display_types(uint64_t count) const {
    uint64_t safe_count = std::min((size_t)count, stack.size());

    std::stringstream ss;

    if (safe_count == 1) {
        ss << "type ";
    }
    else {
        ss << "types ";
    }

    for (uint64_t idx = safe_count; idx > 0; --idx) {
        if (idx < safe_count) { ss << ", "; }
        ss << (stack[stack.size() - idx].result.get_type_name());
    }

    return ss.str();
}


size_t RPNStack::size() const { return stack.size(); }


CowVec<StackItem> RPNStack::pop_items(uint64_t count) {
    uint64_t safe_count = std::min((size_t)count, stack.size());

    CowVec<StackItem> items;
    items.reserve(count);
    for (uint64_t idx = safe_count; idx > 0; --idx) {
        items.push_back(std::move(stack[stack.size() - idx]));
    }
    stack.resize(stack.size() - safe_count);

    return items;
}


CowVec<StackItem> RPNStack::pop_items(uint64_t count, OptionalVariables variables) {
    uint64_t safe_count = std::min((size_t)count, stack.size());

    CowVec<StackItem> items;
    items.reserve(count);
    for (uint64_t idx = safe_count; idx > 0; --idx) {
        StackItem& item = stack.mut_at(stack.size() - idx);

        if (item.result.get_type() == TYPE_IDENTIFIER && variables) {
            std::optional<Value> value = variables->get().load(item.result);
            if (value) {
                item.result = value.value();
                item.p_input = create_displayables_from("load(", item.p_input, ")");
            }
        }

        items.push_back(std::move(stack[stack.size() - idx]));
    }
    stack.resize(stack.size() - safe_count);

    return items;
}


void RPNStack::clear() {
    stack.clear();
}


const CowVec<StackItem>& RPNStack::get_items() const { return stack; }

std::shared_ptr<Displayable> StackItem::get_input_formatted() const {
    if (input_is_expression) {
        return create_displayables_from("(", p_input, ")");
    } else {
        return p_input;
    }
}



void swap(StackItem& a, StackItem& b) {
    using std::swap;

    swap(a.p_input, b.p_input);
    swap(a.result, b.result);
    swap(a.input_is_expression, b.input_is_expression);
}


}