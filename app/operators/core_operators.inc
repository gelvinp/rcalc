// @RCalcOperator
// Name: Add
// Parameters: [Int, Int]
// Description: Adds two values together.
// Requires: "core/logger.h"
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
// Name: Pow
// Parameters: [Int, Int]
// Description: Raise arg0 to the power of arg1.
// Requires: <cmath>
// Requires: <limits>
RCALC_OP_2(Pow, Int, Int, arg0, arg1) {
    double value = std::pow(arg0, arg1);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }

    // No floating point, check for int64_t
    if (value <= (double)std::numeric_limits<int64_t>::max() && value >= (double)std::numeric_limits<int64_t>::min()) {
        return Ok(Value(static_cast<int64_t>(value)));
    }

    // Too big, must use a BigInt
    return Err(ERR_INVALID_PARAM, "Result of pow must be a BigInt!");
}


// @RCalcOperator
// Name: Pow
// Parameters: [Real, Real]
// Requires: <cmath>
// Tags: [real_cast]
RCALC_OP_2(Pow, Real, Real, arg0, arg1) {
    double value = std::pow(arg0, arg1);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }
    
    return Ok(Value(value));
}

RCALC_FMT_2(Pow, arg0, arg1) {
    return arg0.get_input_formatted() + " ^ " + arg1.get_input_formatted();
}


// @RCalcOperator
// Name: Sqr
// Parameters: [Int]
// Description: Raise arg0 to the power of 2.
// Requires: <cmath>
// Requires: <limits>
RCALC_OP_1(Sqr, Int, arg0) {
    double value = std::pow(arg0, 2.0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }

    // No floating point, check for int64_t
    if (value <= (double)std::numeric_limits<int64_t>::max() && value >= (double)std::numeric_limits<int64_t>::min()) {
        return Ok(Value(static_cast<int64_t>(value)));
    }

    // Too big, must use a BigInt
    return Err(ERR_INVALID_PARAM, "Result of pow must be a BigInt!");
}


// @RCalcOperator
// Name: Sqr
// Parameters: [Real]
// Requires: <cmath>
RCALC_OP_1(Sqr, Real, arg0) {
    double value = std::pow(arg0, 2.0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }
    
    return Ok(Value(value));
}

RCALC_FMT_1(Sqr, arg0) {
    return arg0.get_input_formatted() + " ^ 2";
}


// @RCalcOperator
// Name: Cube
// Parameters: [Int]
// Description: Raise arg0 to the power of 3.
// Requires: <cmath>
// Requires: <limits>
RCALC_OP_1(Cube, Int, arg0) {
    double value = std::pow(arg0, 3.0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }

    // No floating point, check for int64_t
    if (value <= (double)std::numeric_limits<int64_t>::max() && value >= (double)std::numeric_limits<int64_t>::min()) {
        return Ok(Value(static_cast<int64_t>(value)));
    }

    // Too big, must use a BigInt
    return Err(ERR_INVALID_PARAM, "Result of pow must be a BigInt!");
}


// @RCalcOperator
// Name: Cube
// Parameters: [Real]
// Requires: <cmath>
RCALC_OP_1(Cube, Real, arg0) {
    double value = std::pow(arg0, 3.0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }
    
    return Ok(Value(value));
}

RCALC_FMT_1(Cube, arg0) {
    return arg0.get_input_formatted() + " ^ 3";
}


// @RCalcOperator
// Name: Sqrt
// Parameters: [Real]
// Description: Takes the square root of arg0.
// Requires: <cmath>
// Tags: [real_cast]
RCALC_OP_1(Sqrt, Real, arg0) {
    double value = std::sqrt(arg0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }
    
    return Ok(Value(value));
}

RCALC_FMT_1(Sqrt, arg0) {
    return "sqrt(" + arg0.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: Cbrt
// Parameters: [Real]
// Description: Takes the cube root of arg0.
// Requires: <cmath>
// Tags: [real_cast]
RCALC_OP_1(Cbrt, Real, arg0) {
    double value = std::sqrt(arg0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }
    
    return Ok(Value(value));
}

RCALC_FMT_1(Cbrt, arg0) {
    return "cbrt(" + arg0.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: Abs
// Parameters: [Int]
// Description: Computes the absolute value of arg0.
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Abs, Int, arg0) {
    return Ok(Value(std::abs(arg0)));
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
    return "|" + arg0.get_input_formatted() + "|";
}


// @RCalcOperator
// Name: Neg
// Parameters: [Int]
// Description: Negates arg0.
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Neg, Int, arg0) {
    return Ok(Value(-arg0));
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