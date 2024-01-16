// @RCalcOperator
// Description: Adds two values together.
// Example: [3, 5]
// Requires: <limits>
// Requires: "core/checked_math.h"
RCALC_OP_2(Add, Int, Int, arg0, arg1) {
    return Ok(CheckedMath::add(arg0, arg1));
}

// @RCalcOperator
// Tags: [real_cast]
RCALC_OP_2(Add, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

RCALC_FMT_2(Add, arg0, arg1) {
    std::shared_ptr<Displayable> p_left;
    std::shared_ptr<Displayable> p_right;

    if (
        (arg0.p_input->get_type() == Displayable::Type::VALUE) ||
        (arg0.p_input->tags == DisplayableTag::OP_ADD)
    ) {
        p_left = arg0.p_input;
    }
    else { p_left = arg0.get_input_formatted(); }

    if (
        (arg1.p_input->get_type() == Displayable::Type::VALUE) ||
        (arg1.p_input->tags == DisplayableTag::OP_ADD)
    ) {
        p_right = arg1.p_input;
    }
    else { p_right = arg1.get_input_formatted(); }

    return create_tagged_displayables_from(DisplayableTag::OP_ADD, p_left, " + ", p_right);
}


// @RCalcOperator
// Description: Subtracts two values.
// Example: [3, 5]
// Requires: "core/checked_math.h"
RCALC_OP_2(Sub, Int, Int, arg0, arg1) {
    return Ok(CheckedMath::sub(arg0, arg1));
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
// Requires: "core/checked_math.h"
RCALC_OP_2(Mul, Int, Int, arg0, arg1) {
    return Ok(CheckedMath::mul(arg0, arg1));
}

// @RCalcOperator
// Tags: [real_cast]
RCALC_OP_2(Mul, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

RCALC_FMT_2(Mul, arg0, arg1) {
    std::shared_ptr<Displayable> p_left;
    std::shared_ptr<Displayable> p_right;

    if (
        (arg0.p_input->get_type() == Displayable::Type::VALUE) ||
        (arg0.p_input->tags == DisplayableTag::OP_MUL)
    ) {
        p_left = arg0.p_input;
    }
    else { p_left = arg0.get_input_formatted(); }

    if (
        (arg1.p_input->get_type() == Displayable::Type::VALUE) ||
        (arg1.p_input->tags == DisplayableTag::OP_MUL)
    ) {
        if (arg0.result.is_mat() && arg1.result.is_vec()) {
            p_right = create_tagged_displayables_from(DisplayableTag::COL_VEC, arg1.p_input);
        }
        else { p_right = arg1.p_input; }
    }
    else { p_right = arg1.get_input_formatted(); }

    return create_tagged_displayables_from(DisplayableTag::OP_MUL, p_left, " × ", p_right);
}


// @RCalcOperator
// Description: Divides two values.
// Tags: [real_cast]
// Example: [3, 5]
RCALC_OP_2(Div, Real, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

RCALC_FMT_2(Div, arg0, arg1) {
    std::shared_ptr<Displayable> p_right;

    if (arg0.result.is_mat() && arg1.result.is_vec()) {
        p_right = create_tagged_displayables_from(DisplayableTag::COL_VEC, arg1.p_input);
    }
    else { p_right = arg1.get_input_formatted(); }

    return create_displayables_from(arg0.get_input_formatted(), " / ", p_right);
}


// @RCalcOperator
// Description: Computes the absolute value of arg0.
// Requires: <cmath>
// Requires: "core/checked_math.h"
// Tags: [no_expr]
// Example: [3]
// Example: [-5]
RCALC_OP_1(Abs, Int, arg0) {
    return Ok(CheckedMath::abs(arg0));
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
// Requires: "core/checked_math.h"
// Tags: [no_expr]
// Example: [3]
// Example: [-5]
RCALC_OP_1(Neg, Int, arg0) {
    return Ok(CheckedMath::neg(arg0));
}

// @RCalcOperator
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(Neg, Real, arg0) {
    return Ok(Value(-arg0));
}

RCALC_FMT_1(Neg, arg0) {
    if ((uint8_t)arg0.get_input_formatted()->tags & (uint8_t)DisplayableTag::OP_NEG) {
        return arg0.p_input->p_next;
    }

    std::shared_ptr<Displayable> disp = create_displayables_from("-", arg0.get_input_formatted());
    disp->tags = DisplayableTag::OP_NEG;
    return disp;
}