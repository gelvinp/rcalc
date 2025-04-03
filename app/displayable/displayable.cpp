#include "displayable.h"

#include "core/memory/allocator.h"

#include <sstream>
#include <set>

namespace RCalc {


Displayable::Iterator Displayable::begin() {
    if (get_type() == Type::RECURSIVE) {
        Displayable* p_disp = this;
        std::vector<Displayable*> stack;

        while (p_disp->get_type() == Type::RECURSIVE) {
            stack.push_back(p_disp);
            p_disp = reinterpret_cast<RecursiveDisplayable*>(p_disp)->p_displayable.get();
        }

        return Iterator(p_disp, stack);
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


std::shared_ptr<Displayable> Displayable::create(const char* p_char) {
    return Allocator::make_shared<ConstCharDisplayable>(p_char);
    // return std::shared_ptr<Displayable>(reinterpret_cast<Displayable*>(new ConstCharDisplayable(p_char)));
}

std::shared_ptr<Displayable> Displayable::create(std::string str) {
    return Allocator::make_shared<StringDisplayable>(str);
    // return std::shared_ptr<Displayable>(reinterpret_cast<Displayable*>(new StringDisplayable(str)));
}

std::shared_ptr<Displayable> Displayable::create(Value value) {
    return Allocator::make_shared<ValueDisplayable>(value);
    // return std::shared_ptr<Displayable>(reinterpret_cast<Displayable*>(new ValueDisplayable(value)));
}

std::shared_ptr<Displayable> Displayable::create(std::shared_ptr<Displayable> p_displayable) {
    return Allocator::make_shared<RecursiveDisplayable>(p_displayable);
    // return std::shared_ptr<Displayable>(reinterpret_cast<Displayable*>(new RecursiveDisplayable(p_displayable)));
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
                ss << reinterpret_cast<ValueDisplayable*>(&disp)->value.to_string(disp.tags);
                break;
            }
            case Type::RECURSIVE:
                UNREACHABLE();
        }
    }

    return ss.str();
}


}