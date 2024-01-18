#pragma once

#include <optional>
#include <sstream>
#include <utility>
#include <vector>

#include "app/stack.h"
#include "core/value.h"

namespace RCalc {


CowVec<Real> get_reals(CowVec<RCalc::StackItem> items, std::shared_ptr<Displayable>& input_disp_front) {
    CowVec<Real> values;
    values.reserve(items.size());
    std::shared_ptr<Displayable> input_disp;

    for (const RCalc::StackItem& item : items) {
        values.push_back(item.result.get_real());
        
        if (input_disp) {
            input_disp->p_next = create_displayables_from(", ", item.result);
            input_disp = input_disp->p_next->p_next;
        }
        else {
            input_disp = create_displayables_from(item.result);
            input_disp_front = input_disp;
        }
    }
    return values;
}


CowVec<std::pair<Real, Value>> get_zipped_reals(CowVec<RCalc::StackItem> items, std::shared_ptr<Displayable>& input_disp_front) {
    CowVec<std::pair<Real, Value>> values;
    values.reserve(items.size());
    std::shared_ptr<Displayable> input_disp;

    for (const RCalc::StackItem& item : items) {
        values.emplace_back(item.result.get_real(), item.result);
        
        if (input_disp) {
            input_disp->p_next = create_displayables_from(", ", item.result);
            input_disp = input_disp->p_next->p_next;
        }
        else {
            input_disp = create_displayables_from(item.result);
            input_disp_front = input_disp;
        }
    }
    return values;
}

}