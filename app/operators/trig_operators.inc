// @RCalcOperator
// Description: The mathematical constant Pi.
// Category: Constant
// Requires: <numbers>
// Tags: [no_expr]
RCALC_OP_0(Pi) {
    return Ok(Value(std::numbers::pi_v<Real>));
}

RCALC_FMT_0(Pi) {
    return create_displayables_from("π");
}


// @RCalcOperator
// Description: The mathematical constant Tau.
// Category: Constant
// Requires: <numbers>
// Tags: [no_expr]
RCALC_OP_0(Tau) {
    return Ok(Value(std::numbers::pi_v<Real> * 2.0));
}

RCALC_FMT_0(Tau) {
    return create_displayables_from("τ");
}


// @RCalcOperator
// Description: Computes the sine of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [0.785398]
RCALC_OP_1(Sin, Real, arg) {
    return Ok(Value(std::sin(arg)));
}

RCALC_FMT_1(Sin, arg) {
    return create_displayables_from("sin(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the cosine of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [0.785398]
RCALC_OP_1(Cos, Real, arg) {
    return Ok(Value(std::cos(arg)));
}

RCALC_FMT_1(Cos, arg) {
    return create_displayables_from("cos(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the tangent of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [0.785398]
RCALC_OP_1(Tan, Real, arg) {
    return Ok(Value(std::tan(arg)));
}

RCALC_FMT_1(Tan, arg) {
    return create_displayables_from("tan(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the inverse sine of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [0.707107]
RCALC_OP_1(Asin, Real, arg) {
    return Ok(Value(std::asin(arg)));
}

RCALC_FMT_1(Asin, arg) {
    return create_displayables_from("asin(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the inverse cosine of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [0.707107]
RCALC_OP_1(Acos, Real, arg) {
    return Ok(Value(std::acos(arg)));
}

RCALC_FMT_1(Acos, arg) {
    return create_displayables_from("acos(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the inverse tangent of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [1]
RCALC_OP_1(Atan, Real, arg) {
    return Ok(Value(std::atan(arg)));
}

RCALC_FMT_1(Atan, arg) {
    return create_displayables_from("atan(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the hyperbolic sine of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [0.785398]
RCALC_OP_1(Sinh, Real, arg) {
    return Ok(Value(std::sinh(arg)));
}

RCALC_FMT_1(Sinh, arg) {
    return create_displayables_from("sinh(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the hyperbolic cosine of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [0.785398]
RCALC_OP_1(Cosh, Real, arg) {
    return Ok(Value(std::cosh(arg)));
}

RCALC_FMT_1(Cosh, arg) {
    return create_displayables_from("cosh(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the hyperbolic tangent of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [0.785398]
RCALC_OP_1(Tanh, Real, arg) {
    return Ok(Value(std::tanh(arg)));
}

RCALC_FMT_1(Tanh, arg) {
    return create_displayables_from("tanh(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the inverse hyperbolic sine of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [0.868671]
RCALC_OP_1(Asinh, Real, arg) {
    return Ok(Value(std::asinh(arg)));
}

RCALC_FMT_1(Asinh, arg) {
    return create_displayables_from("asinh(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the inverse hyperbolic cosine of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [1.32461]
RCALC_OP_1(Acosh, Real, arg) {
    return Ok(Value(std::acosh(arg)));
}

RCALC_FMT_1(Acosh, arg) {
    return create_displayables_from("acosh(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the inverse hyperbolic tangent of arg0 (in radians).
// Category: Trig
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [0.655794]
RCALC_OP_1(Atanh, Real, arg) {
    return Ok(Value(std::atanh(arg)));
}

RCALC_FMT_1(Atanh, arg) {
    return create_displayables_from("atanh(", arg.p_input, ")");
}