#pragma once

#include "core/value.h"

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

    std::vector<StackItem> pop_items(uint64_t count);

    const std::vector<StackItem>& get_items() const;

    // I'm sorry for this
    template<typename T, typename U = T*>
    void set_message(U message) {
        #ifndef NDEBUG
        if (p_message) {
            throw std::logic_error("Cannot set message on RPNStack without clearing the existing message!");
        }
        #endif
        p_message = static_cast<void*>(message);
        message_type = typeid(T).hash_code();
    }

    template<typename T, typename U = T*>
    U get_message() {
        #ifndef NDEBUG
        if (!p_message) {
            throw std::logic_error("Cannot get message on RPNStack without setting one first!");
        }
        if (typeid(T).hash_code() != message_type) {
            throw std::logic_error("Cannot get message on RPNStack of different type!");
        }
        #endif
        return static_cast<U>(p_message);
    }

    template<typename T, typename U = T*>
    void clear_message() {
        #ifndef NDEBUG
        if (!p_message) {
            throw std::logic_error("Cannot clear message on RPNStack without setting one first!");
        }
        if (typeid(T).hash_code() != message_type) {
            throw std::logic_error("Cannot clear message on RPNStack of different type!");
        }
        #endif
        delete (U)p_message;
        message_type = 0;
    }

private:
    std::vector<StackItem> stack;
    void* p_message = nullptr; // Used to communicate between operator implementations and formatting functions
    size_t message_type = 0;
};

}