#pragma once

#include <cstdint>


namespace RCalc {

enum class DisplayableTag : uint8_t {
    NONE = 0,

    OP_ADD = 1, // Associative ops to identify redundant parens
    OP_MUL = 2, // e.g. ((1 + 2) + 3) * 4 -> (1 + 2 + 3) * 4

    COL_VEC = 4, // When multiplying Mat * Vec, represent as a column vector

    OP_NEG = 8 // Turn -(-(5)) into 5
};

}