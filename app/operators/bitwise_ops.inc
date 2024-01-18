// @RCalcOperator
// Description: Computes the bitwise and between arg0 and arg1
// Category: Bitwise
// Example: [0b1010, 0b0110]
// Requires: <bit>
RCALC_OP_2(And, Int, Int, arg0, arg1) {
    uint64_t res = std::bit_cast<uint64_t>(arg0) & std::bit_cast<uint64_t>(arg1);
    return Ok(Value(std::bit_cast<int64_t>(res), REPR_BINARY));
}

RCALC_FMT_2(And, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " & ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: Computes the bitwise or between arg0 and arg1
// Category: Bitwise
// Example: [0b1010, 0b0110]
// Requires: <bit>
RCALC_OP_2(Or, Int, Int, arg0, arg1) {
    uint64_t res = std::bit_cast<uint64_t>(arg0) | std::bit_cast<uint64_t>(arg1);
    return Ok(Value(std::bit_cast<int64_t>(res), REPR_BINARY));
}

RCALC_FMT_2(Or, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " | ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: Computes the bitwise exclusive or between arg0 and arg1
// Category: Bitwise
// Example: [0b1010, 0b0110]
// Requires: <bit>
RCALC_OP_2(Xor, Int, Int, arg0, arg1) {
    uint64_t res = std::bit_cast<uint64_t>(arg0) ^ std::bit_cast<uint64_t>(arg1);
    return Ok(Value(std::bit_cast<int64_t>(res), REPR_BINARY));
}

RCALC_FMT_2(Xor, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " ^ ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: Computes the bitwise inverse of arg0
// Category: Bitwise
// Example: [0b1010]
// Requires: <bit>
RCALC_OP_1(Not, Int, arg0) {
    uint64_t inverted = ~std::bit_cast<uint64_t>(arg0);

    // // Cut it off at the same number of bits used in arg0
    int width = std::bit_width(std::bit_cast<uint64_t>(arg0));
    if (width == 64) { return Ok(Value(std::bit_cast<int64_t>(inverted), REPR_BINARY)); }

    uint64_t mask = (1 << width) - 1;
    uint64_t res = inverted & mask;

    return Ok(Value(std::bit_cast<int64_t>(res), REPR_BINARY));
}

RCALC_FMT_1(Not, arg0) {
    return create_displayables_from("~", arg0.get_input_formatted());
}


// @RCalcOperator
// Description: Shifts arg0 to the left bitwise by arg1 places
// Category: Bitwise
// Example: [0b1010, 2]
// Requires: <bit>
RCALC_OP_2(Sll, Int, Int, arg0, arg1) {
    uint64_t res = std::bit_cast<uint64_t>(arg0) << arg1;
    return Ok(Value(std::bit_cast<int64_t>(res), REPR_BINARY));
}

RCALC_FMT_2(Sll, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " << ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: Shifts arg0 to the right bitwise by arg1 places
// Category: Bitwise
// Example: [0b1010, 2]
// Requires: <bit>
RCALC_OP_2(Srl, Int, Int, arg0, arg1) {
    uint64_t res = std::bit_cast<uint64_t>(arg0) >> arg1;
    return Ok(Value(std::bit_cast<int64_t>(res), REPR_BINARY));
}

RCALC_FMT_2(Srl, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " >> ", arg1.get_input_formatted());
}