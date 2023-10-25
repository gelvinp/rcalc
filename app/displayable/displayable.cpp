#include "displayable.h"

#include <sstream>

namespace RCalc {


Displayable::Iterator Displayable::begin() {
    if (get_type() == Type::RECURSIVE) {
        return Iterator {
            reinterpret_cast<RecursiveDisplayable*>(this)->p_displayable.get(),
            { this }
        };
    }
    return Iterator(this);
}


void Displayable::Iterator::increment() {
    // Check if next is recursive
    if (p_displayable->p_next) {
        Displayable* p_search = p_displayable->p_next.get();

        while (p_search->get_type() == Displayable::Type::RECURSIVE) {
            display_stack.push_back(p_search);
            p_search = reinterpret_cast<RecursiveDisplayable*>(p_search)->p_displayable.get();
        }

        p_displayable = p_search;
    }
    // Check if we need to continue after recursion
    else {
        while (!(p_displayable->p_next)) {
            if (!display_stack.empty()) {
                p_displayable = display_stack.back();
                display_stack.pop_back();
            }
            else {
                p_displayable = nullptr;
                return;
            }
        }

        // Move to next displayable
        p_displayable = p_displayable->p_next.get();
    }
}


Displayable& Displayable::back() {
    Displayable* p_back = {};

    for (Displayable& displayable : *this) {
        p_back = &displayable;
    }

    return *p_back;
}


std::shared_ptr<Displayable> Displayable::create(const char* p_char) {
    return std::shared_ptr<Displayable>(reinterpret_cast<Displayable*>(new ConstCharDisplayable(p_char)));
}

std::shared_ptr<Displayable> Displayable::create(std::string str) {
    return std::shared_ptr<Displayable>(reinterpret_cast<Displayable*>(new StringDisplayable(str)));
}

std::shared_ptr<Displayable> Displayable::create(Value value) {
    return std::shared_ptr<Displayable>(reinterpret_cast<Displayable*>(new ValueDisplayable(value)));
}

std::shared_ptr<Displayable> Displayable::create(std::shared_ptr<Displayable> p_displayable) {
    return std::shared_ptr<Displayable>(reinterpret_cast<Displayable*>(new RecursiveDisplayable(p_displayable)));
}


std::string Displayable::dbg_display() {
    std::stringstream ss;

    for (Displayable& disp : *this) {
        switch (disp.get_type()) {
            case Type::CONST_CHAR: {
                ss << reinterpret_cast<ConstCharDisplayable*>(&disp)->p_char;
                break;
            }
            case Type::STRING: {
                ss << reinterpret_cast<StringDisplayable*>(&disp)->str;
                break;
            }
            case Type::VALUE: {
                std::string str = reinterpret_cast<ValueDisplayable*>(&disp)->value.to_string();
                ss << reinterpret_cast<ValueDisplayable*>(&disp)->value.to_string();
                break;
            }
            case Type::RECURSIVE:
                UNREACHABLE();
        }
    }

    return ss.str();
}


}