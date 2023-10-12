#pragma once

#include "core/value.h"

#include <any>
#include <optional>
#include <string>
#include <stdexcept>
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
    std::vector<Value::Type> peek_types_vec(uint64_t count) const;
    std::string display_types(uint64_t count) const;

    std::vector<StackItem> pop_items(uint64_t count);

    const std::vector<StackItem>& get_items() const;

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

    template<typename T, typename U = T&>
    U get_message() {
        #ifndef NDEBUG
        if (!message.has_value()) {
            throw std::logic_error("Cannot get message on RPNStack without setting one first!");
        }
        if (typeid(T) != message.type()) {
            throw std::logic_error("Cannot get message on RPNStack of different type!");
        }
        #endif
        // Dereference address into ref to avoid creating copies
        return *std::any_cast<T>(&message);
    }

    void clear_message() {
        #ifndef NDEBUG
        if (!message.has_value()) {
            throw std::logic_error("Cannot clear message on RPNStack without setting one first!");
        }
        #endif
        message.reset();
    }

private:
    std::vector<StackItem> stack;
    std::any message;
};

}