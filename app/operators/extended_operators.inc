// @RCalcOperator
// Name: Pow
// Parameters: [Int, Int]
// Description: Raise arg0 to the power of arg1.
// Requires: <cmath>
RCALC_OP_2(Pow, Int, Int, arg0, arg1) {
    double value = std::pow(arg0, arg1);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }
    
    return Ok(Value::find_int(value));
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
RCALC_OP_1(Sqr, Int, arg0) {
    double value = std::pow(arg0, 2.0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }
    
    return Ok(Value::find_int(value));
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
RCALC_OP_1(Cube, Int, arg0) {
    double value = std::pow(arg0, 3.0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }
    
    return Ok(Value::find_int(value));
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
    return "sqrt(" + arg0.input + ")";
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
    return "cbrt(" + arg0.input + ")";
}


// @RCalcOperator
// Name: Log
// Parameters: [Real, Real]
// Description: Calculates log base arg0 of arg1.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_2(Log, Real, Real, base, arg) {
    return Ok(Value(std::log(arg) / std::log(base)));
}

RCALC_FMT_2(Log, base, arg) {
    return "log_" + base.get_input_formatted() + "(" + arg.input + ")";
}


// @RCalcOperator
// Name: Ln
// Parameters: [Real]
// Description: Calculates natural log of arg0.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Ln, Real, arg) {
    return Ok(Value(std::log(arg)));
}

RCALC_FMT_1(Ln, arg) {
    return "ln(" + arg.input + ")";
}


// @RCalcOperator
// Name: Exp
// Parameters: [Real]
// Description: Calculates e to the power of arg0.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Exp, Real, arg) {
    return Ok(Value(std::exp(arg)));
}

RCALC_FMT_1(Exp, arg) {
    return "e^" + arg.get_input_formatted();
}


// @RCalcOperator
// Name: Ceil
// Parameters: [Real]
// Description: Calculates the nearest integer not less than the given value.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Ceil, Real, arg) {
    double val = std::ceil(arg);
    return Ok(Value::find_int(val));
}

RCALC_FMT_1(Ceil, arg) {
    return "⌈" + arg.input + "⌉";
}


// @RCalcOperator
// Name: Floor
// Parameters: [Real]
// Description: Calculates the nearest integer not greater than the given value.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Floor, Real, arg) {
    double val = std::floor(arg);
    return Ok(Value::find_int(val));
}

RCALC_FMT_1(Floor, arg) {
    return "⌊" + arg.input + "⌋";
}


// @RCalcOperator
// Name: Trunc
// Parameters: [Real]
// Description: Calculates the nearest integer not greater in magnitude than the given value.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Trunc, Real, arg) {
    double val = std::trunc(arg);
    return Ok(Value::find_int(val));
}

RCALC_FMT_1(Trunc, arg) {
    return "trunc(" + arg.input + ")";
}


// @RCalcOperator
// Name: Round
// Parameters: [Real]
// Description: Calculates the nearest integer to the given value, rounding halfway cases away from zero.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Round, Real, arg) {
    double val = std::round(arg);
    return Ok(Value::find_int(val));
}

RCALC_FMT_1(Round, arg) {
    return "round(" + arg.input + ")";
}


// @RCalcOperator
// Name: Sign
// Parameters: [Real]
// Description: Calculates 1 if the value is positive, -1 if the value is negative, and 0 otherwise.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Sign, Real, arg) {
    if (arg > 0) { return Ok(Value((int64_t)1)); }
    if (arg < 0) { return Ok(Value((int64_t)-1)); }
    return Ok(Value((int64_t)0));
}

RCALC_FMT_1(Sign, arg) {
    return "sign(" + arg.input + ")";
}


// @RCalcOperator
// Name: Fact
// Parameters: [Int]
// Description: Calculates the factorial of an integer.
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Fact, Int, arg) {
    if (arg < 0) { return Err(ERR_INVALID_PARAM, "Cannot take the factorial of a negative number!"); }
    if (arg == 0) { return Ok(Value((int64_t)1)); }

    double res = std::tgamma((double)arg);
    return Ok(Value::find_int(res));
}

RCALC_FMT_1(Fact, arg) {
    return arg.get_input_formatted() + "!";
}


// @RCalcOperator
// Name: Gamma
// Parameters: [Real]
// Description: Calculates the gamma function of a given number.
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Gamma, Real, arg) {
    double res = std::tgamma((double)arg);
    return Ok(Value::find_int(res));
}

RCALC_FMT_1(Gamma, arg) {
    return "gamma(" + arg.input + ")";
}


// @RCalcOperator
// Name: Quot
// Parameters: [Int, Int]
// Description: Computes the integer quotient when dividing arg0 by arg1.
// Requires: <cstdlib>
RCALC_OP_2(Quot, Int, Int, arg0, arg1) {
    auto [quot, _rem] = std::div(arg0, arg1);
    return Ok(Value((int64_t)quot));
}

RCALC_FMT_2(Quot, arg0, arg1) {
    return arg0.get_input_formatted() + " //q " + arg1.get_input_formatted();
}


// @RCalcOperator
// Name: Mod
// Parameters: [Int, Int]
// Description: Computes the remainder when dividing arg0 by arg1.
// Requires: <cstdlib>
RCALC_OP_2(Mod, Int, Int, arg0, arg1) {
    auto [_quot, rem] = std::div(arg0, arg1);
    return Ok(Value((int64_t)rem));
}

// @RCalcOperator
// Name: Mod
// Parameters: [Real, Real]
// Tags: [real_cast]
// Requires: <cstdlib>
RCALC_OP_2(Mod, Real, Real, arg0, arg1) {
    return Ok(Value(std::fmod(arg0, arg1)));
}

RCALC_FMT_2(Mod, arg0, arg1) {
    return arg0.get_input_formatted() + " % " + arg1.get_input_formatted();
}