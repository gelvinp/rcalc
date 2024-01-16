#pragma once

#include "core/value.h"

namespace RCalc {
struct CheckedMath {

    static Value add(Int a, Int b);
    static Value sub(Int a, Int b);
    static Value mul(Int a, Int b);
    static Value abs(Int a);
    static Value neg(Int a);
    static Value pow(Int base, Int exp);

    static Value discretize(Real a);

};
}
