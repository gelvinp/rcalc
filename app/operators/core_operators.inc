// @RCalcOperator
// Name: Add
// Parameters: [Int, Int]
// Description: Adds two values together.
RCALC_OP_2(Add, Int, Int, arg0, arg1) {
    return Ok(Value::find_int(arg0 + arg1));
}

// @RCalcOperator
// Name: Add
// Parameters: [Real, Real]
// Tags: [real_cast]
RCALC_OP_2(Add, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

RCALC_FMT_2(Add, arg0, arg1) {
    return arg0.get_input_formatted() + " + " + arg1.get_input_formatted();
}


// @RCalcOperator
// Name: Sub
// Parameters: [Int, Int]
// Description: Subtracts two values.
RCALC_OP_2(Sub, Int, Int, arg0, arg1) {
    return Ok(Value::find_int(arg0 - arg1));
}

// @RCalcOperator
// Name: Sub
// Parameters: [Real, Real]
// Tags: [real_cast]
RCALC_OP_2(Sub, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}

RCALC_FMT_2(Sub, arg0, arg1) {
    return arg0.get_input_formatted() + " - " + arg1.get_input_formatted();
}


// @RCalcOperator
// Name: Mul
// Parameters: [Int, Int]
// Description: Multiplies two values together.
RCALC_OP_2(Mul, Int, Int, arg0, arg1) {
    return Ok(Value::find_int(arg0 * arg1));
}

// @RCalcOperator
// Name: Mul
// Parameters: [Real, Real]
// Tags: [real_cast]
RCALC_OP_2(Mul, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

RCALC_FMT_2(Mul, arg0, arg1) {
    return arg0.get_input_formatted() + " * " + arg1.get_input_formatted();
}


// @RCalcOperator
// Name: Div
// Parameters: [Real, Real]
// Description: Divides two values.
// Tags: [real_cast]
RCALC_OP_2(Div, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

RCALC_FMT_2(Div, arg0, arg1) {
    return arg0.get_input_formatted() + " / " + arg1.get_input_formatted();
}


// @RCalcOperator
// Name: Abs
// Parameters: [Int]
// Description: Computes the absolute value of arg0.
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Abs, Int, arg0) {
    return Ok(Value::find_int(std::abs(arg0)));
}

// @RCalcOperator
// Name: Abs
// Parameters: [Real]
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Abs, Real, arg0) {
    return Ok(Value(std::abs(arg0)));
}

RCALC_FMT_1(Abs, arg0) {
    return "|" + arg0.input + "|";
}


// @RCalcOperator
// Name: Neg
// Parameters: [Int]
// Description: Negates arg0.
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Neg, Int, arg0) {
    return Ok(Value::find_int(-arg0));
}

// @RCalcOperator
// Name: Neg
// Parameters: [Real]
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Neg, Real, arg0) {
    return Ok(Value(-arg0));
}

RCALC_FMT_1(Neg, arg0) {
    return "-" + arg0.get_input_formatted();
}