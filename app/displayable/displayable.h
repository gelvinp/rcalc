#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "core/display_tags.h"
#include "core/value.h"

namespace RCalc {


struct Displayable {
    enum class Type : uint8_t {
        CONST_CHAR,
        STRING,
        VALUE,
        RECURSIVE
    };

    std::shared_ptr<Displayable> p_next = {};
    DisplayableTag tags = DisplayableTag::NONE;

    virtual Type get_type() const = 0;

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
        friend struct Displayable;
    };

    Iterator begin();
    Iterator end() { return Iterator(nullptr); }

    static std::shared_ptr<Displayable> create(const char*);
    static std::shared_ptr<Displayable> create(std::string);
    static std::shared_ptr<Displayable> create(Value);
    static std::shared_ptr<Displayable> create(std::shared_ptr<Displayable>);

    std::string dbg_display();

    virtual ~Displayable() = default;
};

struct ConstCharDisplayable : public Displayable {
    ConstCharDisplayable(const char* p_char) : Displayable(), p_char(p_char) {}

    const char* p_char;

    virtual Type get_type() const override { return Type::CONST_CHAR; }
};

struct StringDisplayable : public Displayable {
    StringDisplayable(std::string str) : Displayable(), str(str) {}

    std::string str;

    virtual Type get_type() const override { return Type::STRING; }
};

struct ValueDisplayable : public Displayable {
    ValueDisplayable(Value value) : Displayable(), value(value) {}

    Value value;

    virtual Type get_type() const override { return Type::VALUE; }
};

struct RecursiveDisplayable : public Displayable {
    RecursiveDisplayable(std::shared_ptr<Displayable> p_displayable) : Displayable(), p_displayable(p_displayable) {}

    std::shared_ptr<Displayable> p_displayable;

    virtual Type get_type() const override { return Type::RECURSIVE; }
};


template<typename T>
concept IsDisplayable = requires(T a) {
    Displayable::create(a);
};


template<IsDisplayable Type>
std::shared_ptr<Displayable> create_displayables_from(Type value) {
    return Displayable::create(value);
}


template<IsDisplayable Type>
std::shared_ptr<Displayable> create_tagged_displayables_from(DisplayableTag tags, Type value) {
    std::shared_ptr<Displayable> p_displayable = Displayable::create(value);
    p_displayable->begin()->tags = tags;
    p_displayable->tags = tags;
    
    return p_displayable;
}


template<IsDisplayable Type, IsDisplayable... Types>
std::shared_ptr<Displayable> create_displayables_from(Type value, Types... var_types) {
    std::shared_ptr<Displayable> p_displayable = Displayable::create(value);
    p_displayable->p_next = create_displayables_from(var_types...);

    return p_displayable;
}


template<IsDisplayable Type, IsDisplayable... Types>
std::shared_ptr<Displayable> create_tagged_displayables_from(DisplayableTag tags, Type value, Types... var_types) {
    std::shared_ptr<Displayable> p_displayable = Displayable::create(value);
    p_displayable->p_next = create_displayables_from(var_types...);
    p_displayable->begin()->tags = tags;
    p_displayable->tags = tags;

    return p_displayable;
}


}