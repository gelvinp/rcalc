// @RCalcOperator
// Description: Returns a unique value identifying the type.
// Tags: [no_expr]
// Example: [1]
RCALC_OP_1(Type, Int, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_INT));
}

// @RCalcOperator
// Tags: [no_expr]
// Example: [100000000000000000000000000000]
RCALC_OP_1(Type, BigInt, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_BIGINT));
}

// @RCalcOperator
// Tags: [no_expr]
// Example: [1.0]
RCALC_OP_1(Type, Real, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_REAL));
}

// @RCalcOperator
// Tags: [no_expr]
// Example: [[1, 2]]
RCALC_OP_1(Type, Vec2, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_VEC2));
}

// @RCalcOperator
// Tags: [no_expr]
// Example: [[1, 2, 3]]
RCALC_OP_1(Type, Vec3, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_VEC3));
}

// @RCalcOperator
// Tags: [no_expr]
// Example: [[1, 2, 3, 4]]
RCALC_OP_1(Type, Vec4, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_VEC4));
}

// @RCalcOperator
// Tags: [no_expr]
// Example: [{[1, 2], [3, 4]}]
RCALC_OP_1(Type, Mat2, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_MAT2));
}

// @RCalcOperator
// Tags: [no_expr]
// Example: [{[1, 2, 3], [4, 5, 6], [7, 8, 9]}]
RCALC_OP_1(Type, Mat3, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_MAT3));
}

// @RCalcOperator
// Tags: [no_expr]
// Example: [{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}]
RCALC_OP_1(Type, Mat4, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_MAT4));
}

// @RCalcOperator
// Tags: [no_expr]
// Example: [_ft]
RCALC_OP_1(Type, Unit, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_UNIT));
}

RCALC_FMT_1(Type, arg) {
    return create_displayables_from("type(", arg.result, ")");
}