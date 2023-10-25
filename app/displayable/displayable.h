#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "core/value.h"

namespace RCalc {


struct Displayable {
    enum class Type {
        CONST_CHAR,
        STRING,
        VALUE,
        RECURSIVE
    };

    enum class Tag {
        NONE,
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV
    };

    std::shared_ptr<Displayable> p_next = {};
    Tag tag = Tag::NONE;

    virtual Type get_type() = 0;

    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::size_t;
        using value_type = Displayable;
        using pointer = Displayable*;
        using reference = Displayable&;

        Iterator(pointer p_displayable) : p_displayable(p_displayable), display_stack({}) {}

        reference operator*() const { return *p_displayable; }
        pointer operator->() const { return p_displayable; }

        Iterator& operator++() { increment(); return *this; }
        Iterator operator++(int) { Iterator prev = *this; increment(); return prev; }

        friend bool operator==(const Iterator& a, const Iterator& b) { return a.p_displayable == b.p_displayable; }

    private:
        pointer p_displayable;
        std::vector<pointer> display_stack;

        void increment();

        Iterator(pointer p_displayable, std::vector<pointer> display_stack) : p_displayable(p_displayable), display_stack(display_stack) {}
        friend class Displayable;
    };

    Iterator begin();
    Iterator end() { return Iterator(nullptr); }

    Displayable& back();

    static std::shared_ptr<Displayable> create(const char*);
    static std::shared_ptr<Displayable> create(std::string);
    static std::shared_ptr<Displayable> create(Value);
    static std::shared_ptr<Displayable> create(std::shared_ptr<Displayable>);

    std::string dbg_display();
};

struct ConstCharDisplayable : public Displayable {
    ConstCharDisplayable(const char* p_char) : Displayable(), p_char(p_char) {}

    const char* p_char;

    virtual Type get_type() override { return Type::CONST_CHAR; }
};

struct StringDisplayable : public Displayable {
    StringDisplayable(std::string str) : Displayable(), str(str) {}

    std::string str;

    virtual Type get_type() override { return Type::STRING; }
};

struct ValueDisplayable : public Displayable {
    ValueDisplayable(Value value) : Displayable(), value(value) {}

    Value value;

    virtual Type get_type() override { return Type::VALUE; }
};

struct RecursiveDisplayable : public Displayable {
    RecursiveDisplayable(std::shared_ptr<Displayable> p_displayable) : Displayable(), p_displayable(p_displayable) {}

    std::shared_ptr<Displayable> p_displayable;

    virtual Type get_type() override { return Type::RECURSIVE; }
};


template<typename T>
concept IsDisplayable = requires(T a) {
    Displayable::create(a);
};


template<IsDisplayable Type>
std::shared_ptr<Displayable> create_displayables_from(Type value) {
    // return Displayable::create(value);

    std::shared_ptr<Displayable> p_disp = Displayable::create(value);
    return p_disp;
}


template<IsDisplayable Type, IsDisplayable... Types>
std::shared_ptr<Displayable> create_displayables_from(Type value, Types... var_types) {

    std::shared_ptr<Displayable> p_displayable = Displayable::create(value);
    p_displayable->p_next = create_displayables_from(var_types...);

    return p_displayable;
}



}