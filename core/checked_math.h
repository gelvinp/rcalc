#pragma once

#include "core/value.h"

namespace RCalc {
struct CheckedMath {

    static Value add(Int a, Int b);
    static Value sub(Int a, Int b);
    static Value mul(Int a, Int b);

};
}
