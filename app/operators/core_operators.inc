// @RCalcOperator
// Description: Adds two values together.
// Example: [3, 5]
RCALC_OP_2(Add, Int, Int, arg0, arg1) {
    return Ok(Value::find_int((Real)arg0 + arg1));
}

// @RCalcOperator
// Tags: [real_cast]
RCALC_OP_2(Add, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

RCALC_FMT_2(Add, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " + ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: Subtracts two values.
// Example: [3, 5]
RCALC_OP_2(Sub, Int, Int, arg0, arg1) {
    return Ok(Value::find_int((Real)arg0 - arg1));
}

// @RCalcOperator
// Tags: [real_cast]
RCALC_OP_2(Sub, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}

RCALC_FMT_2(Sub, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " - ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: Multiplies two values together.
// Example: [3, 5]
RCALC_OP_2(Mul, Int, Int, arg0, arg1) {
    return Ok(Value::find_int((Real)arg0 * arg1));
}

// @RCalcOperator
// Tags: [real_cast]
RCALC_OP_2(Mul, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

RCALC_FMT_2(Mul, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " * ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: Divides two values.
// Tags: [real_cast]
// Example: [3, 5]
RCALC_OP_2(Div, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

RCALC_FMT_2(Div, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " / ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: Computes the absolute value of arg0.
// Requires: <cmath>
// Tags: [no_expr]
// Example: [3]
// Example: [-5]
RCALC_OP_1(Abs, Int, arg0) {
    return Ok(Value::find_int(std::abs((Real)arg0)));
}

// @RCalcOperator
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Abs, Real, arg0) {
    return Ok(Value(std::abs(arg0)));
}

RCALC_FMT_1(Abs, arg0) {
    return create_displayables_from("|", arg0.p_input, "|");
}


// @RCalcOperator
// Description: Negates arg0.
// Requires: <cmath>
// Tags: [no_expr]
// Example: [3]
// Example: [-5]
RCALC_OP_1(Neg, Int, arg0) {
    return Ok(Value::find_int(-(Real)arg0));
}

// @RCalcOperator
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Neg, Real, arg0) {
    return Ok(Value(-arg0));
}

RCALC_FMT_1(Neg, arg0) {
    return create_displayables_from("-(", arg0.get_input_formatted(), ")");
}