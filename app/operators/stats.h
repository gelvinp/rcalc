#pragma once

#include <optional>
#include <sstream>
#include <vector>

#include "app/stack.h"
#include "core/value.h"

namespace RCalc {


CowVec<Real> get_reals(CowVec<RCalc::StackItem> items, std::stringstream& ss) {
    CowVec<Real> values;
    values.reserve(items.size());
    bool first = true;

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
        if (first) { first = false; } else { ss << ", "; }
        ss << item.result.to_string();
    }
    return values;
}

}