#pragma once

#include <optional>
#include <sstream>
#include <vector>

#include "app/stack.h"
#include "core/value.h"

namespace RCalc {


CowVec<Real> get_reals(CowVec<RCalc::StackItem> items, std::shared_ptr<Displayable>& input_disp_front) {
    CowVec<Real> values;
    values.reserve(items.size());
    std::shared_ptr<Displayable> input_disp;

    for (const RCalc::StackItem& item : items) {
        switch (item.result.get_type()) {
        case TYPE_INT:
            values.push_back((Real)(item.result.operator Int()));
            break;
        case TYPE_BIGINT:
            values.push_back((item.result.operator BigInt()).get_real<Real>());
            break;
        case TYPE_REAL:
            values.push_back((Real)item.result);
            break;
        default:
            UNREACHABLE();
        }
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