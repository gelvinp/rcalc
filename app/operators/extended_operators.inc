// @RCalcOperator
// Description: Raise arg0 to the power of arg1.
// Requires: <cmath>
// Requires: "core/checked_math.h"
// Example: [3, 5]
// Example: [25, 0.5]
RCALC_OP_2(Pow, Int, Int, arg0, arg1) {
    return Ok(CheckedMath::pow(arg0, arg1));
}


// @RCalcOperator
// Requires: <cmath>
// Tags: [real_cast]
RCALC_OP_2(Pow, Real, Real, arg0, arg1) {
    Real value = std::pow(arg0, arg1);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of pow is NaN!");
    }
    
    return Ok(Value(value));
}

RCALC_FMT_2(Pow, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " ^ ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: Raise arg0 to the power of 2.
// Requires: <cmath>
// Requires: "core/checked_math.h"
// Example: [3]
RCALC_OP_1(Sqr, Int, arg0) {
    return Ok(CheckedMath::mul(arg0, arg0));
}


// @RCalcOperator
// Requires: <cmath>
RCALC_OP_1(Sqr, Real, arg0) {
    return Ok(Value(arg0 * arg0));
}

RCALC_FMT_1(Sqr, arg0) {
    return create_displayables_from(arg0.get_input_formatted(), " ^ 2");
}


// @RCalcOperator
// Description: Raise arg0 to the power of 3.
// Requires: <cmath>
// Requires: "core/checked_math.h"
// Example: [3]
RCALC_OP_1(Cube, Int, arg0) {
    return Ok(CheckedMath::pow(arg0, 3));
}


// @RCalcOperator
// Requires: <cmath>
RCALC_OP_1(Cube, Real, arg0) {
    Real value = std::pow(arg0, 3.0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of cube is NaN!");
    }
    
    return Ok(Value(value));
}

RCALC_FMT_1(Cube, arg0) {
    return create_displayables_from(arg0.get_input_formatted(), " ^ 3");
}


// @RCalcOperator
// Description: Takes the square root of arg0.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [35]
RCALC_OP_1(Sqrt, Real, arg0) {
    Real value = std::sqrt(arg0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of sqrt is NaN!");
    }
    
    return Ok(Value(value));
}

RCALC_FMT_1(Sqrt, arg0) {
    return create_displayables_from("sqrt(", arg0.p_input, ")");
}


// @RCalcOperator
// Description: Takes the cube root of arg0.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [85]
RCALC_OP_1(Cbrt, Real, arg0) {
    Real value = std::cbrt(arg0);

    if (std::isnan(value)) {
        return Err(ERR_INVALID_PARAM, "Result of cbrt is NaN!");
    }
    
    return Ok(Value(value));
}

RCALC_FMT_1(Cbrt, arg0) {
    return create_displayables_from("cbrt(", arg0.p_input, ")");
}


// @RCalcOperator
// Description: Calculates log base arg0 of arg1.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [2, 16]
RCALC_OP_2(Log, Real, Real, base, arg) {
    return Ok(Value(std::log(arg) / std::log(base)));
}

RCALC_FMT_2(Log, base, arg) {
    return create_displayables_from("log_", base.get_input_formatted(), "(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Calculates natural log of arg0.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [35]
RCALC_OP_1(Ln, Real, arg) {
    return Ok(Value(std::log(arg)));
}

RCALC_FMT_1(Ln, arg) {
    return create_displayables_from("ln(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Calculates e to the power of arg0.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [3]
RCALC_OP_1(Exp, Real, arg) {
    return Ok(Value(std::exp(arg)));
}

RCALC_FMT_1(Exp, arg) {
    return create_displayables_from("e^", arg.get_input_formatted());
}


// @RCalcOperator
// Description: Calculates the nearest integer not less than the given value.
// Requires: <cmath>
// Requires: "core/checked_math.h"
// Tags: [no_expr]
// Example: [3.5]
// Example: [-3.5]
RCALC_OP_1(Ceil, Real, arg) {
    Real val = std::ceil(arg);
    return Ok(CheckedMath::discretize(val));
}

RCALC_FMT_1(Ceil, arg) {
    return create_displayables_from("⌈", arg.p_input, "⌉");
}


// @RCalcOperator
// Description: Calculates the nearest integer not greater than the given value.
// Requires: <cmath>
// Requires: "core/checked_math.h"
// Tags: [no_expr]
// Example: [3.5]
// Example: [-3.5]
RCALC_OP_1(Floor, Real, arg) {
    Real val = std::floor(arg);
    return Ok(CheckedMath::discretize(val));
}

RCALC_FMT_1(Floor, arg) {
    return create_displayables_from("⌊", arg.p_input, "⌋");
}


// @RCalcOperator
// Description: Calculates the nearest integer not greater in magnitude than the given value.
// Requires: <cmath>
// Requires: "core/checked_math.h"
// Tags: [no_expr]
// Example: [3.5]
// Example: [-3.5]
RCALC_OP_1(Trunc, Real, arg) {
    Real val = std::trunc(arg);
    return Ok(CheckedMath::discretize(val));
}

RCALC_FMT_1(Trunc, arg) {
    return create_displayables_from("trunc(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Calculates the nearest integer to the given value, rounding halfway cases away from zero.
// Requires: <cmath>
// Requires: "core/checked_math.h"
// Tags: [no_expr]
// Example: [3.5]
// Example: [-3.5]
RCALC_OP_1(Round, Real, arg) {
    Real val = std::round(arg);
    return Ok(CheckedMath::discretize(val));
}

RCALC_FMT_1(Round, arg) {
    return create_displayables_from("round(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Calculates the fractional component with the same sign as arg0.
// Requires: <cmath>
// Tags: [no_expr]
// Example: [3.5]
// Example: [-3.5]
RCALC_OP_1(Frac, Real, arg) {
    Real _integral;
    return Ok(Value(std::modf(arg, &_integral)));
}

RCALC_FMT_1(Frac, arg) {
    return create_displayables_from("frac(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Calculates 1 if the value is positive, -1 if the value is negative, and 0 otherwise.
// Requires: <cmath>
// Tags: [no_expr, real_cast]
// Example: [3.5]
// Example: [0]
// Example: [-3.5]
RCALC_OP_1(Sign, Real, arg) {
    if (arg > 0) { return Ok(Value((Int)1)); }
    if (arg < 0) { return Ok(Value((Int)-1)); }
    return Ok(Value((Int)0));
}

RCALC_FMT_1(Sign, arg) {
    return create_displayables_from("sign(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Calculates the factorial of an integer.
// Requires: <cmath>
// Requires: "core/checked_math.h"
// Tags: [no_expr]
// Example: [3]
RCALC_OP_1(Fact, Int, arg) {
    if (arg < 0) { return Err(ERR_INVALID_PARAM, "Cannot take the factorial of a negative number!"); }
    if (arg == 0) { return Ok(Value((Int)1)); }

    Real res = std::tgamma((Real)arg + 1.0);
    return Ok(CheckedMath::discretize(res + 0.01));
}

RCALC_FMT_1(Fact, arg) {
    return create_displayables_from(arg.get_input_formatted(), "!");
}


// @RCalcOperator
// Description: Calculates the gamma function of a given number.
// Requires: <cmath>
// Requires: "core/checked_math.h"
// Requires: "core/comparison.h"
// Tags: [no_expr, real_cast]
// Example: [3.5]
// Example: [-3.5]
RCALC_OP_1(Gamma, Real, arg) {
    Real _integral;
    if (arg < 0 && TypeComparison::compare(std::modf(arg, &_integral), 0.0)) { return Err(ERR_INVALID_PARAM, "Gamma function is not defined for negative integers!"); }
    if (TypeComparison::compare(arg, 0.0)) { return Err(ERR_INVALID_PARAM, "Gamma function is not defined for zero!"); }
    Real res = std::tgamma((Real)arg);
    return Ok(Value(res));
}

RCALC_FMT_1(Gamma, arg) {
    return create_displayables_from("gamma(", arg.p_input, ")");
}


// @RCalcOperator
// Description: Computes the integer quotient when dividing arg0 by arg1.
// Requires: <cstdlib>
// Example: [5, 3]
RCALC_OP_2(Quot, Int, Int, arg0, arg1) {
    auto [quot, _rem] = std::div(arg0, arg1);
    return Ok(Value((Int)quot));
}

// @RCalcOperator
// Tags: [real_cast]
// Requires: "core/checked_math.h"
RCALC_OP_2(Quot, Real, Real, arg0, arg1) {
    if (std::isnan(arg0 / arg1)) {
        return Err(ERR_INVALID_PARAM, "Result of Quot is NaN!");
    }
    Real integral;
    std::modf((arg0 / arg1), &integral);
    return Ok(CheckedMath::discretize(integral));
}

RCALC_FMT_2(Quot, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " //q ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: Computes the remainder when dividing arg0 by arg1.
// Requires: <cstdlib>
// Example: [5, 3]
RCALC_OP_2(Mod, Int, Int, arg0, arg1) {
    auto [_quot, rem] = std::div(arg0, arg1);
    return Ok(Value((Int)rem));
}

// @RCalcOperator
// Tags: [real_cast]
// Requires: <cstdlib>
RCALC_OP_2(Mod, Real, Real, arg0, arg1) {
    return Ok(Value(std::fmod(arg0, arg1)));
}

RCALC_FMT_2(Mod, arg0, arg1) {
    return create_displayables_from(arg0.get_input_formatted(), " % ", arg1.get_input_formatted());
}


// @RCalcOperator
// Description: The mathematical constant e.
// Category: Constant
// Requires: <numbers>
// Tags: [no_expr]
RCALC_OP_0(e) {
    return Ok(Value(std::numbers::e_v<Real>));
}

RCALC_FMT_0(e) {
    return create_displayables_from("e");
}


// @RCalcOperator
// Description: Generates a range of numbers from 0 to arg0 when arg0 is positive, and from 1 to +arg0 when arg0 is negative.
// Category: Range
// Tags: [no_expr]
// Requires: "core/checked_math.h"
// Example: [3]
// Example: [5]
RCALC_OP_1_S(Range, Int, arg0) {
    // TODO: Implement this better
    if (stack.get_max_size()) {
        Int max_possible_items = *stack.get_max_size() - stack.size();
        if ((arg0 + 1) > max_possible_items || arg0 < -max_possible_items) { return Err(ERR_STACK_FULL, "Unable to create that many items."); }
    }

    Int from = (arg0 < 0) ? 1 : 0;
    Int to = (arg0 < 0) ? -arg0 : arg0;
    stack.try_reserve_items(stack.size() + to);
    for (Int index = from; index < to; index++) {
        Value value = Value(index);
        stack.try_push_item(StackItem{
            create_displayables_from(value),
            std::move(value),
            false
        });
    }
    Value last = Value(to);
    stack.set_message<std::string>(last.to_string());
    return Ok(std::move(last));
}

RCALC_FMT_1_S(Range, arg0) {
    UNUSED(arg0);
    std::string str = stack.get_message<std::string>();
    return create_displayables_from(str);
}