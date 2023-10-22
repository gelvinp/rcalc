#pragma once

#include <optional>
#include <sstream>
#include <vector>

#include "app/stack.h"
#include "core/value.h"


std::vector<Real> get_reals(std::vector<RCalc::StackItem>&& items, std::stringstream& ss) {
    std::vector<Real> values;
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