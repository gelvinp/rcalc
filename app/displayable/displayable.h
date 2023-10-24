#pragma once

#include <cstddef>
#include <iterator>
#include <vector>

#include "core/value.h"

namespace RCalc {


struct Displayable {
    enum Type {
        TYPE_CONST_CHAR,
        TYPE_VALUE,
        TYPE_RECURSIVE
    };

    enum Tag {
        TAG_NONE,
        TAG_OP_ADD,
        TAG_OP_SUB,
        TAG_OP_MUL,
        TAG_OP_DIV
    };

    Displayable* p_next = nullptr;
    Tag tag = TAG_NONE;

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
    };

    Iterator begin() { return Iterator(this); }
    Iterator end() { return Iterator(nullptr); }

    Displayable* back();

    static Displayable* create(const char*);
    static Displayable* create(Value);
    static Displayable* create(Displayable*);

    Displayable* append(Displayable*);

    virtual ~Displayable();
};

struct ConstCharDisplayable : public Displayable {
    ConstCharDisplayable(const char* p_char) : Displayable(), p_char(p_char) {}

    const char* p_char;

    virtual Type get_type() override { return TYPE_CONST_CHAR; }
};

struct ValueDisplayable : public Displayable {
    ValueDisplayable(Value value) : Displayable(), value(value) {}

    Value value;

    virtual Type get_type() override { return TYPE_VALUE; }
};

struct RecursiveDisplayable : public Displayable {
    RecursiveDisplayable(Displayable* p_displayable) : Displayable(), p_displayable(p_displayable) {}

    Displayable* p_displayable;

    virtual Type get_type() override { return TYPE_RECURSIVE; }

    virtual ~RecursiveDisplayable();
};


template<typename T>
concept IsDisplayable = requires(T a) {
    Displayable::create(a);
};


template<IsDisplayable Type>
Displayable* create_displayables_from(Type value) {
    return Displayable::create(value);
}


template<IsDisplayable Type, IsDisplayable... Types>
Displayable* create_displayables_from(Type value, Types... var_types) {
    Displayable* p_displayable = Displayable::create(value);
    Displayable* p_others = create_displayables_from(var_types...);
    p_displayable->append(p_others);

    return p_displayable;
}



}