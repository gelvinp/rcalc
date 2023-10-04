// @RCalcOperator
// Name: Vec2
// Parameters: [Real, Real]
// Description: Creates the Vec2 [arg0, arg1].
// Requires: <glm/vec2.hpp>
// Tags: [real_cast, no_expr]
RCALC_OP_2(Vec2, Real, Real, arg0, arg1) {
    return Ok(Value(Vec2(arg0, arg1)));
}

RCALC_FMT_2(Vec2, arg0, arg1) {
    return "[" + arg0.get_input_formatted() + ", " + arg1.get_input_formatted() + "]";
}


// @RCalcOperator
// Name: Vec3
// Parameters: [Real, Real, Real]
// Description: Creates the Vec3 [arg0, arg1, arg2].
// Requires: <glm/vec3.hpp>
// Tags: [real_cast, no_expr]
RCALC_OP_3(Vec3, Real, Real, Real, arg0, arg1, arg2) {
    return Ok(Value(Vec3(arg0, arg1, arg2)));
}

RCALC_FMT_3(Vec3, arg0, arg1, arg2) {
    return "[" + arg0.get_input_formatted() + ", " + arg1.get_input_formatted() + ", " + arg2.get_input_formatted() + "]";
}


// @RCalcOperator
// Name: Vec4
// Parameters: [Real, Real, Real, Real]
// Description: Creates the Vec4 [arg0, arg1, arg2, arg3].
// Requires: <glm/vec4.hpp>
// Tags: [real_cast, no_expr]
RCALC_OP_4(Vec4, Real, Real, Real, Real, arg0, arg1, arg2, arg3) {
    return Ok(Value(Vec4(arg0, arg1, arg2, arg3)));
}

RCALC_FMT_4(Vec4, arg0, arg1, arg2, arg3) {
    return "[" + arg0.get_input_formatted() + ", " + arg1.get_input_formatted() + ", " + arg2.get_input_formatted() + ", " + arg3.get_input_formatted() + "]";
}


// @RCalcOperator
// Name: Add
// Parameters: [Vec2, Vec2]
// Requires: <glm/vec2.hpp>
RCALC_OP_2(Add, Vec2, Vec2, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

// @RCalcOperator
// Name: Add
// Parameters: [Vec3, Vec3]
// Requires: <glm/vec3.hpp>
RCALC_OP_2(Add, Vec3, Vec3, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

// @RCalcOperator
// Name: Add
// Parameters: [Vec4, Vec4]
// Requires: <glm/vec4.hpp>
RCALC_OP_2(Add, Vec4, Vec4, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}


// @RCalcOperator
// Name: Sub
// Parameters: [Vec2, Vec2]
// Requires: <glm/vec2.hpp>
RCALC_OP_2(Sub, Vec2, Vec2, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}

// @RCalcOperator
// Name: Sub
// Parameters: [Vec3, Vec3]
// Requires: <glm/vec3.hpp>
RCALC_OP_2(Sub, Vec3, Vec3, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

// @RCalcOperator
// Name: Sub
// Parameters: [Vec4, Vec4]
// Requires: <glm/vec4.hpp>
RCALC_OP_2(Sub, Vec4, Vec4, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}


// @RCalcOperator
// Name: Neg
// Parameters: [Vec2]
// Requires: <glm/vec2.hpp>
RCALC_OP_1(Neg, Vec2, arg0) {
    return Ok(Value(arg0 * -1.0));
}


// @RCalcOperator
// Name: Mul
// Parameters: [Vec2, Real]
// Requires: <glm/vec2.hpp>
// Tags: [real_cast, reversable]
RCALC_OP_2(Mul, Vec2, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Name: Mul
// Parameters: [Vec3, Real]
// Requires: <glm/vec3.hpp>
// Tags: [real_cast, reversable]
RCALC_OP_2(Mul, Vec3, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Name: Mul
// Parameters: [Vec4, Real]
// Requires: <glm/vec4.hpp>
// Tags: [real_cast, reversable]
RCALC_OP_2(Mul, Vec4, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Name: Div
// Parameters: [Vec2, Real]
// Requires: <glm/vec2.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Vec2, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}


// @RCalcOperator
// Name: Div
// Parameters: [Vec3, Real]
// Requires: <glm/vec3.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Vec3, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}


// @RCalcOperator
// Name: Div
// Parameters: [Vec4, Real]
// Requires: <glm/vec4.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Vec4, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}


// @RCalcOperator
// Name: Dot
// Parameters: [Vec2, Vec2]
// Description: Calculates the dot product between vectors arg0 and arg1.
// Requires: <glm/geometric.hpp>
RCALC_OP_2(Dot, Vec2, Vec2, arg0, arg1) {
    return Ok(Value(glm::dot(arg0, arg1)));
}

// @RCalcOperator
// Name: Dot
// Parameters: [Vec3, Vec3]
// Requires: <glm/geometric.hpp>
RCALC_OP_2(Dot, Vec3, Vec3, arg0, arg1) {
    return Ok(Value(glm::dot(arg0, arg1)));
}

// @RCalcOperator
// Name: Dot
// Parameters: [Vec4, Vec4]
// Requires: <glm/geometric.hpp>
RCALC_OP_2(Dot, Vec4, Vec4, arg0, arg1) {
    return Ok(Value(glm::dot(arg0, arg1)));
}

RCALC_FMT_2(Dot, arg0, arg1) {
    return arg0.input + " · " + arg1.input;
}


// @RCalcOperator
// Name: Cross
// Parameters: [Vec3, Vec3]
// Description: Calculates the cross product between Vec3s arg0 and arg1.
// Requires: <glm/geometric.hpp>
RCALC_OP_2(Cross, Vec3, Vec3, arg0, arg1) {
    return Ok(Value(glm::cross(arg0, arg1)));
}

RCALC_FMT_2(Cross, arg0, arg1) {
    return arg0.input + " × " + arg1.input;
}


// @RCalcOperator
// Name: Len
// Parameters: [Vec2]
// Description: Calculates the length of a vector.
// Requires: <glm/geometric.hpp>
RCALC_OP_1(Len, Vec2, arg0) {
    return Ok(Value(glm::length(arg0)));
}

// @RCalcOperator
// Name: Len
// Parameters: [Vec3]
// Requires: <glm/geometric.hpp>
RCALC_OP_1(Len, Vec3, arg0) {
    return Ok(Value(glm::length(arg0)));
}

// @RCalcOperator
// Name: Len
// Parameters: [Vec4]
// Requires: <glm/geometric.hpp>
RCALC_OP_1(Len, Vec4, arg0) {
    return Ok(Value(glm::length(arg0)));
}

RCALC_FMT_1(Len, arg0) {
    return "len(" + arg0.input + ")";
}


// @RCalcOperator
// Name: Lensqr
// Parameters: [Vec2]
// Description: Calculates the length squared of a vector.
// Requires: <glm/geometric.hpp>
RCALC_OP_1(Lensqr, Vec2, arg0) {
    return Ok(Value(arg0.x * arg0.x + arg0.y * arg0.y));
}

// @RCalcOperator
// Name: Lensqr
// Parameters: [Vec3]
// Requires: <glm/geometric.hpp>
RCALC_OP_1(Lensqr, Vec3, arg0) {
    return Ok(Value(arg0.x * arg0.x + arg0.y * arg0.y + arg0.z * arg0.z));
}

// @RCalcOperator
// Name: Lensqr
// Parameters: [Vec4]
// Requires: <glm/geometric.hpp>
RCALC_OP_1(Lensqr, Vec4, arg0) {
    return Ok(Value(arg0.x * arg0.x + arg0.y * arg0.y + arg0.z * arg0.z + arg0.w * arg0.w));
}

RCALC_FMT_1(Lensqr, arg0) {
    return "lensqr(" + arg0.input + ")";
}


// @RCalcOperator
// Name: Norm
// Parameters: [Vec2]
// Description: Normalizes a vector.
// Requires: <glm/geometric.hpp>
RCALC_OP_1(Norm, Vec2, arg0) {
    return Ok(Value(glm::normalize(arg0)));
}

// @RCalcOperator
// Name: Norm
// Parameters: [Vec3]
// Requires: <glm/geometric.hpp>
RCALC_OP_1(Norm, Vec3, arg0) {
    return Ok(Value(glm::normalize(arg0)));
}

// @RCalcOperator
// Name: Norm
// Parameters: [Vec4]
// Requires: <glm/geometric.hpp>
RCALC_OP_1(Norm, Vec4, arg0) {
    return Ok(Value(glm::normalize(arg0)));
}

RCALC_FMT_1(Norm, arg0) {
    return "norm(" + arg0.input + ")";
}


// @RCalcOperator
// Name: Concat
// Description: Joins two values together.
// Parameters: [Real, Real]
// Requires: <glm/vec2.hpp>
// Tags: [real_cast, stack_ref]
RCALC_OP_2_S(Concat, Real, Real, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g | %g]", arg0, arg1));
    return Ok(Value(Vec2(arg0, arg1)));
}

// @RCalcOperator
// Name: Concat
// Parameters: [Vec2, Real]
// Requires: <glm/vec3.hpp>
// Requires: "core/format.h"
// Tags: [real_cast, stack_ref, fmt_stack_ref]
RCALC_OP_2_S(Concat, Vec2, Real, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g, %g | %g]", arg0.x, arg0.y, arg1));
    return Ok(Value(Vec3(arg0.x, arg0.y, arg1)));
}

// @RCalcOperator
// Name: Concat
// Parameters: [Real, Vec2]
// Requires: <glm/vec3.hpp>
// Requires: "core/format.h"
// Tags: [real_cast, stack_ref, fmt_stack_ref]
RCALC_OP_2_S(Concat, Real, Vec2, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g | %g, %g]", arg0, arg1.x, arg1.y));
    return Ok(Value(Vec3(arg0, arg1.x, arg1.y)));
}

// @RCalcOperator
// Name: Concat
// Parameters: [Vec3, Real]
// Requires: <glm/vec4.hpp>
// Requires: "core/format.h"
// Tags: [real_cast, stack_ref, fmt_stack_ref]
RCALC_OP_2_S(Concat, Vec3, Real, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g, %g, %g | %g]", arg0.x, arg0.y, arg0.z, arg1));
    return Ok(Value(Vec4(arg0.x, arg0.y, arg0.z, arg1)));
}

// @RCalcOperator
// Name: Concat
// Parameters: [Real, Vec3]
// Requires: <glm/vec4.hpp>
// Requires: "core/format.h"
// Tags: [real_cast, stack_ref, fmt_stack_ref]
RCALC_OP_2_S(Concat, Real, Vec3, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g | %g, %g, %g]", arg0, arg1.x, arg1.y, arg1.z));
    return Ok(Value(Vec4(arg0, arg1.x, arg1.y, arg1.z)));
}

// @RCalcOperator
// Name: Concat
// Parameters: [Vec2, Vec2]
// Requires: <glm/vec4.hpp>
// Requires: "core/format.h"
// Tags: [real_cast, stack_ref, fmt_stack_ref]
RCALC_OP_2_S(Concat, Vec2, Vec2, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g, %g | %g, %g]", arg0.x, arg0.y, arg1.x, arg1.y));
    return Ok(Value(Vec4(arg0.x, arg0.y, arg1.x, arg1.y)));
}

RCALC_FMT_2_S(Concat, arg0, arg1) {
    UNUSED(arg0);
    UNUSED(arg1);

    std::string str = stack.get_message<std::string>();
    stack.clear_message();
    return str;
}


// @RCalcOperator
// Name: Rev
// Parameters: [Vec2]
// Description: Reverses a vector.
// Requires: <glm/vec2.hpp>
RCALC_OP_1(Rev, Vec2, arg0) {
    return Ok(Value(Vec2(arg0.y, arg0.x)));
}

// @RCalcOperator
// Name: Rev
// Parameters: [Vec3]
// Requires: <glm/vec3.hpp>
RCALC_OP_1(Rev, Vec3, arg0) {
    return Ok(Value(Vec3(arg0.z, arg0.y, arg0.x)));
}

// @RCalcOperator
// Name: Rev
// Parameters: [Vec4]
// Requires: <glm/vec4.hpp>
RCALC_OP_1(Rev, Vec4, arg0) {
    return Ok(Value(Vec4(arg0.w, arg0.z, arg0.y, arg0.x)));
}

RCALC_FMT_1(Rev, arg0) {
    return "rev(" + arg0.input + ")";
}