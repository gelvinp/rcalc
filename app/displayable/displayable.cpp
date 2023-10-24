#include "displayable.h"

namespace RCalc {


void Displayable::Iterator::increment() {
    // Check if next is recursive
    if (p_displayable->p_next && p_displayable->p_next->get_type() == Displayable::TYPE_RECURSIVE) {
        display_stack.push_back(p_displayable->p_next);
        p_displayable = reinterpret_cast<RecursiveDisplayable*>(p_displayable->p_next)->p_displayable;
        return;
    }

    // Check if we need to continue after recursion
    if (p_displayable->p_next == nullptr && !display_stack.empty()) {
        p_displayable = display_stack.back();
        display_stack.pop_back();
        increment();
        return;
    }

    // Move to next displayable
    p_displayable = p_displayable->p_next;
}


Displayable* Displayable::back() {
    Displayable* p_back = nullptr;

    for (Displayable& displayable : *this) {
        p_back = &displayable;
    }

    return p_back;
}


Displayable* Displayable::create(const char* p_char) {
    return new ConstCharDisplayable(p_char);
}

Displayable* Displayable::create(Value value) {
    return new ValueDisplayable(value);
}

Displayable* Displayable::create(Displayable* p_displayable) {
    return new RecursiveDisplayable(p_displayable);
}


Displayable* Displayable::append(Displayable* other) {
    Displayable* p_back = back();
    p_back->p_next = other;
    return other;
}


Displayable::~Displayable() {
    if (p_next) { delete p_next; }
}

RecursiveDisplayable::~RecursiveDisplayable() {
    delete p_displayable;
}

}