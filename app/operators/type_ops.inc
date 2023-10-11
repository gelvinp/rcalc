// @RCalcOperator
// Description: Returns a unique value identifying the type.
// Tags: [no_expr]
RCALC_OP_1(Type, Int, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_INT));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, BigInt, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_BIGINT));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Real, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_REAL));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Vec2, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_VEC2));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Vec3, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_VEC3));
}

// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Type, Vec4, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_VEC4));
}

RCALC_FMT_1(Type, arg) {
    return "type(" + arg.input + ")";
}