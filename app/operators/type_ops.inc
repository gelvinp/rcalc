// @RCalcOperator
// Description: Returns a unique value identifying the type.
// Tags: [no_expr]
RCALC_OP_1(Type, Int, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_INT));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, BigInt, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_BIGINT));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Real, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_REAL));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Vec2, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_VEC2));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Vec3, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_VEC3));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Vec4, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_VEC4));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Mat2, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_MAT2));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Mat3, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_MAT3));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Mat4, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_MAT4));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Unit, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)TYPE_UNIT));
}

RCALC_FMT_1(Type, arg) {
    return "type(" + arg.input + ")";
}