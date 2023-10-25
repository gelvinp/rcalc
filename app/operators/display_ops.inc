// @RCalcOperator
// Description: Displays arg0 as binary
// Category: Display
// Tags: [no_expr]
// Example: [42]
RCALC_OP_1(Bin, Int, arg0) {
    return Ok(Value(arg0, REPR_BINARY));
}

RCALC_FMT_1(Bin, arg0) {
    return create_displayables_from("bin(", arg0.get_input_formatted(), ")");
}


// @RCalcOperator
// Description: Displays arg0 as octal
// Category: Display
// Tags: [no_expr]
// Example: [42]
RCALC_OP_1(Oct, Int, arg0) {
    return Ok(Value(arg0, REPR_OCT));
}

RCALC_FMT_1(Oct, arg0) {
    return create_displayables_from("oct(", arg0.get_input_formatted(), ")");
}


// @RCalcOperator
// Description: Displays arg0 as decmial
// Category: Display
// Tags: [no_expr]
// Example: [0b101010]
RCALC_OP_1(Dec, Int, arg0) {
    return Ok(Value(arg0, REPR_DECIMAL));
}

RCALC_FMT_1(Dec, arg0) {
    return create_displayables_from("dec(", arg0.get_input_formatted(), ")");
}


// @RCalcOperator
// Description: Displays arg0 as hexadecimal
// Category: Display
// Tags: [no_expr]
// Example: [42]
RCALC_OP_1(Hex, Int, arg0) {
    return Ok(Value(arg0, REPR_HEX));
}

RCALC_FMT_1(Hex, arg0) {
    return create_displayables_from("hex(", arg0.get_input_formatted(), ")");
}