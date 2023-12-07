// @RCalcOperator
// Description: Creates the Vec2 [arg0, arg1].
// Requires: <glm/vec2.hpp>
// Tags: [real_cast, no_expr]
// Example: [1, 2]
RCALC_OP_2(Vec2, Real, Real, arg0, arg1) {
    return Ok(Value(Vec2(arg0, arg1)));
}

RCALC_FMT_2(Vec2, arg0, arg1) {
    return create_displayables_from("[", arg0.get_input_formatted(), ", ", arg1.get_input_formatted(), "]");
}


// @RCalcOperator
// Description: Creates the Vec3 [arg0, arg1, arg2].
// Requires: <glm/vec3.hpp>
// Tags: [real_cast, no_expr]
// Example: [1, 2, 3]
RCALC_OP_3(Vec3, Real, Real, Real, arg0, arg1, arg2) {
    return Ok(Value(Vec3(arg0, arg1, arg2)));
}

RCALC_FMT_3(Vec3, arg0, arg1, arg2) {
    return create_displayables_from("[", arg0.get_input_formatted(), ", ", arg1.get_input_formatted(), ", ", arg2.get_input_formatted(), "]");
}


// @RCalcOperator
// Description: Creates the Vec4 [arg0, arg1, arg2, arg3].
// Requires: <glm/vec4.hpp>
// Tags: [real_cast, no_expr]
// Example: [1, 2, 3, 4]
RCALC_OP_4(Vec4, Real, Real, Real, Real, arg0, arg1, arg2, arg3) {
    return Ok(Value(Vec4(arg0, arg1, arg2, arg3)));
}

RCALC_FMT_4(Vec4, arg0, arg1, arg2, arg3) {
    return create_displayables_from("[", arg0.get_input_formatted(), ", ", arg1.get_input_formatted(), ", ", arg2.get_input_formatted(), ", ", arg3.get_input_formatted(), "]");
}


// @RCalcOperator
// Requires: <glm/vec2.hpp>
// Example: [[1, 2], [3, 4]]
RCALC_OP_2(Add, Vec2, Vec2, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

// @RCalcOperator
// Requires: <glm/vec3.hpp>
RCALC_OP_2(Add, Vec3, Vec3, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

// @RCalcOperator
// Requires: <glm/vec4.hpp>
RCALC_OP_2(Add, Vec4, Vec4, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}


// @RCalcOperator
// Requires: <glm/vec2.hpp>
// Example: [[8, 6], [4, 2]]
RCALC_OP_2(Sub, Vec2, Vec2, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}

// @RCalcOperator
// Requires: <glm/vec3.hpp>
RCALC_OP_2(Sub, Vec3, Vec3, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

// @RCalcOperator
// Requires: <glm/vec4.hpp>
RCALC_OP_2(Sub, Vec4, Vec4, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}


// @RCalcOperator
// Requires: <glm/vec2.hpp>
// Example: [[1, 2]]
RCALC_OP_1(Neg, Vec2, arg0) {
    return Ok(Value(arg0 * -1.0));
}


// @RCalcOperator
// Requires: <glm/vec2.hpp>
// Tags: [real_cast, reversable]
// Example: [[1, 2], 3]
RCALC_OP_2(Mul, Vec2, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Requires: <glm/vec3.hpp>
// Tags: [real_cast, reversable]
RCALC_OP_2(Mul, Vec3, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Requires: <glm/vec4.hpp>
// Tags: [real_cast, reversable]
RCALC_OP_2(Mul, Vec4, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Requires: <glm/vec2.hpp>
// Tags: [real_cast]
// Example: [[1, 2], 3]
RCALC_OP_2(Div, Vec2, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}


// @RCalcOperator
// Requires: <glm/vec3.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Vec3, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}


// @RCalcOperator
// Requires: <glm/vec4.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Vec4, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}


// @RCalcOperator
// Description: Calculates the dot product between vectors arg0 and arg1.
// Category: Vector
// Requires: <glm/geometric.hpp>
// Example: [[1, 2], [2, 1]]
RCALC_OP_2(Dot, Vec2, Vec2, arg0, arg1) {
    return Ok(Value(glm::dot(arg0, arg1)));
}

// @RCalcOperator
// Requires: <glm/geometric.hpp>
RCALC_OP_2(Dot, Vec3, Vec3, arg0, arg1) {
    return Ok(Value(glm::dot(arg0, arg1)));
}

// @RCalcOperator
// Requires: <glm/geometric.hpp>
RCALC_OP_2(Dot, Vec4, Vec4, arg0, arg1) {
    return Ok(Value(glm::dot(arg0, arg1)));
}

RCALC_FMT_2(Dot, arg0, arg1) {
    return create_displayables_from(arg0.p_input, " · ", arg1.p_input);
}


// @RCalcOperator
// Description: Calculates the cross product between Vec3s arg0 and arg1.
// Category: Vector
// Requires: <glm/geometric.hpp>
// Example: [[1, 2, 0], [2, 0, 1]]
RCALC_OP_2(Cross, Vec3, Vec3, arg0, arg1) {
    return Ok(Value(glm::cross(arg0, arg1)));
}

RCALC_FMT_2(Cross, arg0, arg1) {
    return create_displayables_from(arg0.p_input, " × ", arg1.p_input);
}


// @RCalcOperator
// Description: Calculates the length of a vector.
// Category: Vector
// Requires: <glm/geometric.hpp>
// Tags: [no_expr]
// Example: [[1, 2]]
RCALC_OP_1(Len, Vec2, arg0) {
    return Ok(Value(glm::length(arg0)));
}

// @RCalcOperator
// Requires: <glm/geometric.hpp>
// Tags: [no_expr]
RCALC_OP_1(Len, Vec3, arg0) {
    return Ok(Value(glm::length(arg0)));
}

// @RCalcOperator
// Requires: <glm/geometric.hpp>
// Tags: [no_expr]
RCALC_OP_1(Len, Vec4, arg0) {
    return Ok(Value(glm::length(arg0)));
}

RCALC_FMT_1(Len, arg0) {
    return create_displayables_from("len(", arg0.p_input, ")");
}


// @RCalcOperator
// Description: Calculates the length squared of a vector.
// Category: Vector
// Tags: [no_expr]
// Example: [[1, 2]]
RCALC_OP_1(Lensqr, Vec2, arg0) {
    return Ok(Value(arg0.x * arg0.x + arg0.y * arg0.y));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Lensqr, Vec3, arg0) {
    return Ok(Value(arg0.x * arg0.x + arg0.y * arg0.y + arg0.z * arg0.z));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Lensqr, Vec4, arg0) {
    return Ok(Value(arg0.x * arg0.x + arg0.y * arg0.y + arg0.z * arg0.z + arg0.w * arg0.w));
}

RCALC_FMT_1(Lensqr, arg0) {
    return create_displayables_from("lensqr(", arg0.p_input, ")");
}


// @RCalcOperator
// Description: Normalizes a vector.
// Category: Vector
// Requires: <glm/geometric.hpp>
// Tags: [no_expr]
// Example: [[1, 2]]
RCALC_OP_1(Norm, Vec2, arg0) {
    return Ok(Value(glm::normalize(arg0)));
}

// @RCalcOperator
// Requires: <glm/geometric.hpp>
// Tags: [no_expr]
RCALC_OP_1(Norm, Vec3, arg0) {
    return Ok(Value(glm::normalize(arg0)));
}

// @RCalcOperator
// Requires: <glm/geometric.hpp>
// Tags: [no_expr]
RCALC_OP_1(Norm, Vec4, arg0) {
    return Ok(Value(glm::normalize(arg0)));
}

RCALC_FMT_1(Norm, arg0) {
    return create_displayables_from("norm(", arg0.p_input, ")");
}


// @RCalcOperator
// Description: Joins two values together.
// Requires: <glm/vec2.hpp>
// Tags: [real_cast, no_expr]
// Example: [1, 2]
RCALC_OP_2_S(Concat, Real, Real, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g | %g]", arg0, arg1));
    return Ok(Value(Vec2(arg0, arg1)));
}

// @RCalcOperator
// Requires: <glm/vec3.hpp>
// Requires: "core/format.h"
// Tags: [real_cast, no_expr]
// Example: [[1, 2], 3]
RCALC_OP_2_S(Concat, Vec2, Real, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g, %g | %g]", arg0.x, arg0.y, arg1));
    return Ok(Value(Vec3(arg0.x, arg0.y, arg1)));
}

// @RCalcOperator
// Requires: <glm/vec3.hpp>
// Requires: "core/format.h"
// Tags: [real_cast, no_expr]
// Example: [3, [1, 2]]
RCALC_OP_2_S(Concat, Real, Vec2, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g | %g, %g]", arg0, arg1.x, arg1.y));
    return Ok(Value(Vec3(arg0, arg1.x, arg1.y)));
}

// @RCalcOperator
// Requires: <glm/vec4.hpp>
// Requires: "core/format.h"
// Tags: [real_cast, no_expr]
// Example: [[1, 2, 3], 4]
RCALC_OP_2_S(Concat, Vec3, Real, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g, %g, %g | %g]", arg0.x, arg0.y, arg0.z, arg1));
    return Ok(Value(Vec4(arg0.x, arg0.y, arg0.z, arg1)));
}

// @RCalcOperator
// Requires: <glm/vec4.hpp>
// Requires: "core/format.h"
// Tags: [real_cast, no_expr]
// Example: [4, [1, 2, 3]]
RCALC_OP_2_S(Concat, Real, Vec3, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g | %g, %g, %g]", arg0, arg1.x, arg1.y, arg1.z));
    return Ok(Value(Vec4(arg0, arg1.x, arg1.y, arg1.z)));
}

// @RCalcOperator
// Requires: <glm/vec4.hpp>
// Requires: "core/format.h"
// Tags: [real_cast, no_expr]
// Example: [[1, 2], [3, 4]]
RCALC_OP_2_S(Concat, Vec2, Vec2, arg0, arg1) {
    stack.set_message<std::string>(fmt("[%g, %g | %g, %g]", arg0.x, arg0.y, arg1.x, arg1.y));
    return Ok(Value(Vec4(arg0.x, arg0.y, arg1.x, arg1.y)));
}

RCALC_FMT_2_S(Concat, arg0, arg1) {
    UNUSED(arg0);
    UNUSED(arg1);

    std::string str = stack.get_message<std::string>();
    return create_displayables_from(str);
}


// @RCalcOperator
// Description: Reverses a vector.
// Category: Vector
// Requires: <glm/vec2.hpp>
// Tags: [no_expr]
// Example: [[1, 2]]
RCALC_OP_1(Rev, Vec2, arg0) {
    return Ok(Value(Vec2(arg0.y, arg0.x)));
}

// @RCalcOperator
// Requires: <glm/vec3.hpp>
RCALC_OP_1(Rev, Vec3, arg0) {
    return Ok(Value(Vec3(arg0.z, arg0.y, arg0.x)));
}

// @RCalcOperator
// Requires: <glm/vec4.hpp>
RCALC_OP_1(Rev, Vec4, arg0) {
    return Ok(Value(Vec4(arg0.w, arg0.z, arg0.y, arg0.x)));
}

RCALC_FMT_1(Rev, arg0) {
    return create_displayables_from("rev(", arg0.p_input, ")");
}