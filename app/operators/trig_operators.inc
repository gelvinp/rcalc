// @RCalcOperator
// Name: Rad
// Parameters: [Real]
// Description: Convert arg0 degrees to radians.
// Requires: <numbers>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Rad, Real, arg) {
    return Ok(Value((arg / 360.0) * 2.0 * std::numbers::pi));
}

RCALC_FMT_1(Rad, arg) {
    return arg.get_input_formatted() + "°";
}


// @RCalcOperator
// Name: Deg
// Parameters: [Real]
// Description: Convert arg0 radians to degrees.
// Requires: <numbers>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Deg, Real, arg) {
    return Ok(Value((arg * 360.0) / (2.0 * std::numbers::pi)));
}

RCALC_FMT_1(Deg, arg) {
    return arg.get_input_formatted() + " rad";
}


// @RCalcOperator
// Name: Pi
// Description: The mathematical constant Pi.
// Requires: <numbers>
// Tags: [no_expr]
RCALC_OP_0(Pi) {
    return Ok(Value(std::numbers::pi_v<Real>));
}

RCALC_FMT_0(Pi) {
    return "π";
}


// @RCalcOperator
// Name: Tau
// Description: The mathematical constant Tau.
// Requires: <numbers>
// Tags: [no_expr]
RCALC_OP_0(Tau) {
    return Ok(Value(std::numbers::pi_v<Real> * 2.0));
}

RCALC_FMT_0(Tau) {
    return "τ";
}


// @RCalcOperator
// Name: Sin
// Parameters: [Real]
// Description: Computes the sine of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Sin, Real, arg) {
    return Ok(Value(std::sin(arg)));
}

RCALC_FMT_1(Sin, arg) {
    return "sin(" + arg.input + ")";
}


// @RCalcOperator
// Name: Cos
// Parameters: [Real]
// Description: Computes the cosine of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Cos, Real, arg) {
    return Ok(Value(std::cos(arg)));
}

RCALC_FMT_1(Cos, arg) {
    return "cos(" + arg.input + ")";
}


// @RCalcOperator
// Name: Tan
// Parameters: [Real]
// Description: Computes the tangent of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Tan, Real, arg) {
    return Ok(Value(std::tan(arg)));
}

RCALC_FMT_1(Tan, arg) {
    return "tan(" + arg.input + ")";
}


// @RCalcOperator
// Name: Asin
// Parameters: [Real]
// Description: Computes the inverse sine of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Asin, Real, arg) {
    return Ok(Value(std::asin(arg)));
}

RCALC_FMT_1(Asin, arg) {
    return "asin(" + arg.input + ")";
}


// @RCalcOperator
// Name: Acos
// Parameters: [Real]
// Description: Computes the inverse cosine of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Acos, Real, arg) {
    return Ok(Value(std::acos(arg)));
}

RCALC_FMT_1(Acos, arg) {
    return "acos(" + arg.input + ")";
}


// @RCalcOperator
// Name: Atan
// Parameters: [Real]
// Description: Computes the inverse tangent of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Atan, Real, arg) {
    return Ok(Value(std::atan(arg)));
}

RCALC_FMT_1(Atan, arg) {
    return "atan(" + arg.input + ")";
}


// @RCalcOperator
// Name: Sinh
// Parameters: [Real]
// Description: Computes the hyperbolic sine of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Sinh, Real, arg) {
    return Ok(Value(std::sinh(arg)));
}

RCALC_FMT_1(Sinh, arg) {
    return "sinh(" + arg.input + ")";
}


// @RCalcOperator
// Name: Cosh
// Parameters: [Real]
// Description: Computes the hyperbolic cosine of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Cosh, Real, arg) {
    return Ok(Value(std::cosh(arg)));
}

RCALC_FMT_1(Cosh, arg) {
    return "cosh(" + arg.input + ")";
}


// @RCalcOperator
// Name: Tanh
// Parameters: [Real]
// Description: Computes the hyperbolic tangent of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Tanh, Real, arg) {
    return Ok(Value(std::tanh(arg)));
}

RCALC_FMT_1(Tanh, arg) {
    return "tanh(" + arg.input + ")";
}


// @RCalcOperator
// Name: Asinh
// Parameters: [Real]
// Description: Computes the inverse hyperbolic sine of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Asinh, Real, arg) {
    return Ok(Value(std::asinh(arg)));
}

RCALC_FMT_1(Asinh, arg) {
    return "asinh(" + arg.input + ")";
}


// @RCalcOperator
// Name: Acosh
// Parameters: [Real]
// Description: Computes the inverse hyperbolic cosine of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Acosh, Real, arg) {
    return Ok(Value(std::acosh(arg)));
}

RCALC_FMT_1(Acosh, arg) {
    return "acosh(" + arg.input + ")";
}


// @RCalcOperator
// Name: Atanh
// Parameters: [Real]
// Description: Computes the inverse hyperbolic tangent of arg0 (in radians).
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(Atanh, Real, arg) {
    return Ok(Value(std::atanh(arg)));
}

RCALC_FMT_1(Atanh, arg) {
    return "atanh(" + arg.input + ")";
}