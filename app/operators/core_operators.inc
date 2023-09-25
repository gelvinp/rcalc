// @RCalcOperator
// Name: Add
// Parameters: [Int, Int]
// Description: Adds two values together.
RCALC_OP_2(Add, Int, Int, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

// @RCalcOperator
// Name: Add
// Parameters: [Real, Real]
RCALC_OP_2(Add, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

// @RCalcOperator
// Name: Add
// Parameters: [Int, Real]
// Tags: [reversable]
RCALC_OP_2(Add, Int, Real, arg0, arg1) {
    return Ok(Value((Real)arg0 + arg1));
}

RCALC_FMT_2(Add, arg0, arg1) {
    return arg0.get_input_formatted() + " + " + arg1.get_input_formatted();
}


// @RCalcOperator
// Name: Sub
// Parameters: [Int, Int]
// Description: Subtracts two values.
RCALC_OP_2(Sub, Int, Int, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}

// @RCalcOperator
// Name: Sub
// Parameters: [Real, Real]
RCALC_OP_2(Sub, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}

// @RCalcOperator
// Name: Sub
// Parameters: [Int, Real]
RCALC_OP_2(Sub, Int, Real, arg0, arg1) {
    return Ok(Value((Real)arg0 - arg1));
}

// @RCalcOperator
// Name: Sub
// Parameters: [Real, Int]
RCALC_OP_2(Sub, Real, Int, arg0, arg1) {
    return Ok(Value(arg0 - (Real)arg1));
}

RCALC_FMT_2(Sub, arg0, arg1) {
    return arg0.get_input_formatted() + " - " + arg1.get_input_formatted();
}


// @RCalcOperator
// Name: Mul
// Parameters: [Int, Int]
// Description: Multiplies two values together.
RCALC_OP_2(Mul, Int, Int, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

// @RCalcOperator
// Name: Mul
// Parameters: [Real, Real]
RCALC_OP_2(Mul, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

// @RCalcOperator
// Name: Mul
// Parameters: [Int, Real]
// Tags: [reversable]
RCALC_OP_2(Mul, Int, Real, arg0, arg1) {
    return Ok(Value((Real)arg0 * arg1));
}

RCALC_FMT_2(Mul, arg0, arg1) {
    return arg0.get_input_formatted() + " * " + arg1.get_input_formatted();
}


// @RCalcOperator
// Name: Div
// Parameters: [Int, Int]
// Description: Divides two values.
RCALC_OP_2(Div, Int, Int, arg0, arg1) {
    return Ok(Value((Real)arg0 /(Real)arg1));
}

// @RCalcOperator
// Name: Div
// Parameters: [Real, Real]
RCALC_OP_2(Div, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Name: Div
// Parameters: [Int, Real]
RCALC_OP_2(Div, Int, Real, arg0, arg1) {
    return Ok(Value((Real)arg0 / arg1));
}

// @RCalcOperator
// Name: Div
// Parameters: [Real, Int]
RCALC_OP_2(Div, Real, Int, arg0, arg1) {
    return Ok(Value(arg0 / (Real)arg1));
}

RCALC_FMT_2(Div, arg0, arg1) {
    return arg0.get_input_formatted() + " / " + arg1.get_input_formatted();
}
