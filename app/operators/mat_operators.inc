// @RCalcOperator
// Description: Creates the Mat2 from row-major Vec2s {vec0, vec1}.
// Requires: <glm/mat2x2.hpp>
// Tags: [no_expr]
RCALC_OP_2(Mat2, Vec2, Vec2, arg0, arg1) {
    Mat2 mat {arg0, arg1};
    return Ok(Value(glm::transpose(mat)));
}

RCALC_FMT_2(Mat2, arg0, arg1) {
    return "{" + arg0.get_input_formatted() + ", " + arg1.get_input_formatted() + "}";
}


// @RCalcOperator
// Description: Creates the Mat3 from row-major Vec3s {vec0, vec1, vec2}.
// Requires: <glm/mat3x3.hpp>
// Tags: [no_expr]
RCALC_OP_3(Mat3, Vec3, Vec3, Vec3, arg0, arg1, arg2) {
    Mat3 mat {arg0, arg1, arg2};
    return Ok(Value(glm::transpose(mat)));
}

RCALC_FMT_3(Mat3, arg0, arg1, arg2) {
    return "{" + arg0.get_input_formatted() + ", " + arg1.get_input_formatted() + "," + arg2.get_input_formatted() + "}";
}


// @RCalcOperator
// Description: Creates the Mat4 from row-major Vec4s {vec0, vec1, vec2, vec3}.
// Requires: <glm/mat4x4.hpp>
// Tags: [no_expr]
RCALC_OP_4(Mat4, Vec4, Vec4, Vec4, Vec4, arg0, arg1, arg2, arg3) {
    Mat4 mat {arg0, arg1, arg2, arg3};
    return Ok(Value(glm::transpose(mat)));
}

RCALC_FMT_4(Mat4, arg0, arg1, arg2, arg3) {
    return "{" + arg0.get_input_formatted() + ", " + arg1.get_input_formatted() + "," + arg2.get_input_formatted() + "," + arg3.get_input_formatted() + "}";
}


// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
RCALC_OP_2(Add, Mat2, Mat2, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
RCALC_OP_2(Add, Mat3, Mat3, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
RCALC_OP_2(Add, Mat4, Mat4, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}


// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
RCALC_OP_2(Sub, Mat2, Mat2, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
RCALC_OP_2(Sub, Mat3, Mat3, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
RCALC_OP_2(Sub, Mat4, Mat4, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}


// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
// Tags: [real_cast, reversable]
RCALC_OP_2(Mul, Mat2, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
// Tags: [real_cast, reversable]
RCALC_OP_2(Mul, Mat3, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
// Tags: [real_cast, reversable]
RCALC_OP_2(Mul, Mat4, Real, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
RCALC_OP_2(Mul, Mat2, Vec2, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
RCALC_OP_2(Mul, Mat3, Vec3, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
RCALC_OP_2(Mul, Mat4, Vec4, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
RCALC_OP_2(Mul, Vec2, Mat2, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
RCALC_OP_2(Mul, Vec3, Mat3, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
RCALC_OP_2(Mul, Vec4, Mat4, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
RCALC_OP_2(Mul, Mat2, Mat2, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
RCALC_OP_2(Mul, Mat3, Mat3, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
RCALC_OP_2(Mul, Mat4, Mat4, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Mat2, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Mat3, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Mat4, Real, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Real, Mat2, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Real, Mat3, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Real, Mat4, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Mat2, Vec2, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Mat3, Vec3, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Mat4, Vec4, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Vec2, Mat2, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Vec3, Mat3, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Vec4, Mat4, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Mat2, Mat2, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Mat3, Mat3, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
// Tags: [real_cast]
RCALC_OP_2(Div, Mat4, Mat4, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}


// @RCalcOperator
// Description: Calculates the inverse matrix of arg0, if the determinate is non-zero
// Category: Matrix
// Requires: <glm/mat2x2.hpp>
RCALC_OP_1(Inv, Mat2, arg0) {
    if (glm::determinant(arg0) == 0) { return Err(ERR_INVALID_PARAM, "Cannot invert matrix with determinant of zero!"); }
    return Ok(Value(glm::inverse(arg0)));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
RCALC_OP_1(Inv, Mat3, arg0) {
    if (glm::determinant(arg0) == 0) { return Err(ERR_INVALID_PARAM, "Cannot invert matrix with determinant of zero!"); }
    return Ok(Value(glm::inverse(arg0)));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
RCALC_OP_1(Inv, Mat4, arg0) {
    if (glm::determinant(arg0) == 0) { return Err(ERR_INVALID_PARAM, "Cannot invert matrix with determinant of zero!"); }
    return Ok(Value(glm::inverse(arg0)));
}

RCALC_FMT_1(Inv, arg0) {
    return arg0.get_input_formatted() + "^-1";
}


// @RCalcOperator
// Description: Flips a matrix over its diagonal
// Category: Matrix
// Requires: <glm/mat2x2.hpp>
RCALC_OP_1(TPose, Mat2, arg0) {
    return Ok(Value(glm::transpose(arg0)));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
RCALC_OP_1(TPose, Mat3, arg0) {
    return Ok(Value(glm::transpose(arg0)));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
RCALC_OP_1(TPose, Mat4, arg0) {
    return Ok(Value(glm::transpose(arg0)));
}

RCALC_FMT_1(TPose, arg0) {
    return arg0.get_input_formatted() + "T";
}


// @RCalcOperator
// Description: Calculates the determinant matrix of arg0
// Category: Matrix
// Requires: <glm/mat2x2.hpp>
RCALC_OP_1(Det, Mat2, arg0) {
    return Ok(Value(glm::determinant(arg0)));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
RCALC_OP_1(Det, Mat3, arg0) {
    return Ok(Value(glm::determinant(arg0)));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
RCALC_OP_1(Det, Mat4, arg0) {
    return Ok(Value(glm::determinant(arg0)));
}

RCALC_FMT_1(Det, arg0) {
    return "|" + arg0.get_input_formatted() + "|";
}


// @RCalcOperator
// Description: Get the zero-indexed column vector of a matrix.
// Category: Matrix
// Requires: <glm/mat2x2.hpp>
// Requires: "core/format.h"
RCALC_OP_2(Col, Mat2, Int, arg0, arg1) {
    if (arg1 >= 2) { return Err(ERR_INVALID_PARAM, fmt("Column index %d is out of bounds for Mat2", arg1)); }
    return Ok(Value(arg0[arg1]));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
// Requires: "core/format.h"
RCALC_OP_2(Col, Mat3, Int, arg0, arg1) {
    if (arg1 >= 3) { return Err(ERR_INVALID_PARAM, fmt("Column index %d is out of bounds for Mat3", arg1)); }
    return Ok(Value(arg0[arg1]));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
// Requires: "core/format.h"
RCALC_OP_2(Col, Mat4, Int, arg0, arg1) {
    if (arg1 >= 4) { return Err(ERR_INVALID_PARAM, fmt("Column index %d is out of bounds for Mat4", arg1)); }
    return Ok(Value(arg0[arg1]));
}

RCALC_FMT_2(Col, arg0, arg1) {
    std::string str0 = arg0.get_input_formatted();
    std::string str1 = arg1.get_input_formatted();
    return fmt("%s[c%s]",  str0.c_str(), str1.c_str());
}


// @RCalcOperator
// Description: Get the zero-indexed row vector of a matrix.
// Category: Matrix
// Requires: <glm/mat2x2.hpp>
// Requires: "core/format.h"
RCALC_OP_2(Row, Mat2, Int, arg0, arg1) {
    if (arg1 >= 2) { return Err(ERR_INVALID_PARAM, fmt("Row index %d is out of bounds for Mat2", arg1)); }
    return Ok(Value(glm::transpose(arg0)[arg1]));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
// Requires: "core/format.h"
RCALC_OP_2(Row, Mat3, Int, arg0, arg1) {
    if (arg1 >= 3) { return Err(ERR_INVALID_PARAM, fmt("Row index %d is out of bounds for Mat3", arg1)); }
    return Ok(Value(glm::transpose(arg0)[arg1]));
}

// @RCalcOperator
// Requires: <glm/mat4x4.hpp>
// Requires: "core/format.h"
RCALC_OP_2(Row, Mat4, Int, arg0, arg1) {
    if (arg1 >= 4) { return Err(ERR_INVALID_PARAM, fmt("Row index %d is out of bounds for Mat4", arg1)); }
    return Ok(Value(glm::transpose(arg0)[arg1]));
}

RCALC_FMT_2(Row, arg0, arg1) {
    std::string str0 = arg0.get_input_formatted();
    std::string str1 = arg1.get_input_formatted();
    return fmt("%s[r%s]",  str0.c_str(), str1.c_str());
}


// @RCalcOperator
// Description: Create a square identity matrix of size arg0 (2, 3, or 4).
// Category: Matrix
// Requires: <glm/mat2x2.hpp>
// Requires: <glm/mat3x3.hpp>
// Requires: <glm/mat4x4.hpp>
// Tags: [no_expr]
RCALC_OP_1(IMat, Int, arg0) {
    switch (arg0) {
        case 2:
            return Ok(Value(Mat2(1.0)));
        case 3:
            return Ok(Value(Mat3(1.0)));
        case 4:
            return Ok(Value(Mat4(1.0)));
        default:
            return Err(ERR_INVALID_PARAM, fmt("Matrix size %d is invalid", arg0));
    }
}

RCALC_FMT_1(IMat, arg0) {
    return "imat(" + arg0.get_input_formatted() + ")";
}


// @RCalcOperator
// Description: Resize the matrix arg0 to be of size arg1 (2, 3, or 4).
// Category: Matrix
// Requires: <glm/mat2x2.hpp>
// Requires: <glm/mat3x3.hpp>
// Requires: <glm/mat4x4.hpp>
// Tags: [no_expr]
RCALC_OP_2(resize, Mat2, Int, arg0, arg1) {
    switch (arg1) {
        case 2:
            return Ok(Value(arg0));
        case 3:
            return Ok(Value(Mat3(arg0)));
        case 4:
            return Ok(Value(Mat4(arg0)));
        default:
            return Err(ERR_INVALID_PARAM, fmt("Matrix size %d is invalid", arg0));
    }
}

// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
// Requires: <glm/mat3x3.hpp>
// Requires: <glm/mat4x4.hpp>
// Tags: [no_expr]
RCALC_OP_2(resize, Mat3, Int, arg0, arg1) {
    switch (arg1) {
        case 2:
            return Ok(Value(Mat2(arg0)));
        case 3:
            return Ok(Value(arg0));
        case 4:
            return Ok(Value(Mat4(arg0)));
        default:
            return Err(ERR_INVALID_PARAM, fmt("Matrix size %d is invalid", arg0));
    }
}

// @RCalcOperator
// Requires: <glm/mat2x2.hpp>
// Requires: <glm/mat3x3.hpp>
// Requires: <glm/mat4x4.hpp>
// Tags: [no_expr]
RCALC_OP_2(resize, Mat4, Int, arg0, arg1) {
    switch (arg1) {
        case 2:
            return Ok(Value(Mat2(arg0)));
        case 3:
            return Ok(Value(Mat3(arg0)));
        case 4:
            return Ok(Value(arg0));
        default:
            return Err(ERR_INVALID_PARAM, fmt("Matrix size %d is invalid", arg0));
    }
}

RCALC_FMT_2(resize, arg0, arg1) {
    return "resize(" + arg0.get_input_formatted() + ", " + arg1.get_input_formatted() + ")";
}


// @RCalcOperator
// Description: Create a matrix representing the translation arg0.
// Category: Matrix
// Requires: <glm/vec2.hpp>
// Requires: <glm/mat3x3.hpp>
// Tags: [no_expr]
RCALC_OP_1(transmat, Vec2, arg0) {
    Mat3 mat { 1.0 };
    mat[2].x = arg0.x;
    mat[2].y = arg0.y;
    return Ok(Value(mat));
}

// @RCalcOperator
// Requires: <glm/vec3.hpp>
// Requires: <glm/mat4x4.hpp>
// Tags: [no_expr]
RCALC_OP_1(transmat, Vec3, arg0) {
    Mat4 mat { 1.0 };
    mat[3].x = arg0.x;
    mat[3].y = arg0.y;
    mat[3].z = arg0.z;
    return Ok(Value(mat));
}

RCALC_FMT_1(transmat, arg0) {
    return "transmat(" + arg0.get_input_formatted() + ")";
}


// @RCalcOperator
// Description: Create a matrix representing the rotation by arg0 radians CCW around the origin.
// Category: Matrix
// Requires: <glm/mat2x2.hpp>
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(rotmat, Real, arg0) {
    Real sin = std::sin(arg0);
    Real cos = std::cos(arg0);
    return Ok(Value(Mat2(cos, sin, -sin, cos)));
}

// @RCalcOperator
// Requires: <glm/mat3x3.hpp>
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(rotmat, Vec3, arg0) {
    Real sin_a = std::sin(arg0.z);
    Real cos_a = std::cos(arg0.z);
    Real sin_b = std::sin(arg0.y);
    Real cos_b = std::cos(arg0.y);
    Real sin_y = std::sin(arg0.x);
    Real cos_y = std::cos(arg0.x);

    Vec3 col_0 { cos_a * cos_b, sin_a * cos_b, -sin_b };
    Vec3 col_1 { cos_a * sin_b * sin_y - sin_a * cos_y, sin_a * sin_b * sin_y + cos_a * cos_y, cos_b * sin_y };
    Vec3 col_2 { cos_a * sin_b * cos_y + sin_a * sin_y, sin_a * sin_b * cos_y - cos_a * sin_y, cos_b * cos_y };

    return Ok(Value(Mat3(col_0, col_1, col_2)));
}

RCALC_FMT_1(rotmat, arg0) {
    return "rotmat(" + arg0.get_input_formatted() + ")";
}


// @RCalcOperator
// Description: Create a matrix representing the scaling by arg0.
// Category: Matrix
// Requires: <glm/vec2.hpp>
// Requires: <glm/mat2x2.hpp>
// Tags: [no_expr]
RCALC_OP_1(scalemat, Vec2, arg0) {
    Mat2 mat { 1.0 };
    mat[0].x = arg0.x;
    mat[1].y = arg0.y;
    return Ok(Value(mat));
}

// @RCalcOperator
// Requires: <glm/vec3.hpp>
// Requires: <glm/mat3x3.hpp>
// Tags: [no_expr]
RCALC_OP_1(scalemat, Vec3, arg0) {
    Mat3 mat { 1.0 };
    mat[0].x = arg0.x;
    mat[1].y = arg0.y;
    mat[2].z = arg0.z;
    return Ok(Value(mat));
}

// @RCalcOperator
// Requires: <glm/vec4.hpp>
// Requires: <glm/mat4x4.hpp>
// Tags: [no_expr]
RCALC_OP_1(scalemat, Vec4, arg0) {
    Mat4 mat { 1.0 };
    mat[0].x = arg0.x;
    mat[1].y = arg0.y;
    mat[2].z = arg0.z;
    mat[3].w = arg0.w;
    return Ok(Value(mat));
}

RCALC_FMT_1(scalemat, arg0) {
    return "scalemat(" + arg0.get_input_formatted() + ")";
}