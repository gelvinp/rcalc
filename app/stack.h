#pragma once

#include "core/value.h"
#include "app/displayable/displayable.h"
#include "core/memory/cowvec.h"

#include <any>
#include <optional>
#include <string>
#include <stdexcept>
#include <vector>

namespace RCalc {


struct StackItem {
    std::shared_ptr<Displayable> p_input; // Owned by StackItem
    Value result;
    bool input_is_expression; // When composing inputs, expressions should be wrapped in ()'s

    std::shared_ptr<Displayable> get_input_formatted() const; // Wraps with parenthesis if expression
};

class RPNStack {
public:
    void push_item(StackItem item);
    void push_items(CowVec<StackItem> items);
    void reserve_items(size_t count);

    size_t size() const;
    void clear();
    bool same_ref(const RPNStack& other) { return stack.same_ref(other.stack); }

    std::string peek_types(uint64_t count) const;
    CowVec<Type> peek_types_vec(uint64_t count) const;
    std::string display_types(uint64_t count) const;

    CowVec<StackItem> pop_items(uint64_t count);

    const CowVec<StackItem>& get_items() const;

    template<typename T, typename... Args>
    void set_message(Args&&... args) {
        #ifndef NDEBUG
        if (message.has_value()) {
            throw std::logic_error("Cannot set message on RPNStack without clearing the existing message!");
        }
        #endif
        message.emplace<T>(args...);
    }

    template<typename T, typename U, typename... Args>
    void set_message(std::initializer_list<U> il, Args&&... args) {
        #ifndef NDEBUG
        if (message.has_value()) {
            throw std::logic_error("Cannot set message on RPNStack without clearing the existing message!");
        }
        #endif
        message.emplace<T>(il, args...);
    }

    template<typename T, typename U = T&&>
    T get_message() {
        #ifndef NDEBUG
        if (!message.has_value()) {
            throw std::logic_error("Cannot get message on RPNStack without setting one first!");
        }
        if (typeid(T) != message.type()) {
            throw std::logic_error("Cannot get message on RPNStack of different type!");
        }
        #endif
        // Dereference address into ref to avoid creating copies
        T ret = std::any_cast<T>(std::move(message));
        message.reset();
        return ret;
    }

private:
    CowVec<StackItem> stack;
    std::any message;
};

}