// @RCalcOperator
// Name: Type
// Parameters: [Int]
// Description: Returns a unique value identifying the type.
// Tags: [no_expr]
RCALC_OP_1(Type, Int, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_INT));
}

// @RCalcOperator
// Name: Type
// Parameters: [BigInt]
// Tags: [no_expr]
RCALC_OP_1(Type, BigInt, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_BIGINT));
}

// @RCalcOperator
// Name: Type
// Parameters: [Real]
// Tags: [no_expr]
RCALC_OP_1(Type, Real, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_REAL));
}

// @RCalcOperator
// Name: Type
// Parameters: [Vec2]
// Tags: [no_expr]
RCALC_OP_1(Type, Vec2, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_VEC2));
}

// @RCalcOperator
// Name: Type
// Parameters: [Vec3]
// Tags: [no_expr]
RCALC_OP_1(Type, Vec3, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_VEC3));
}

// @RCalcOperator
// Name: Type
// Parameters: [Vec4]
// Tags: [no_expr]
RCALC_OP_1(Type, Vec4, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_VEC4));
}

RCALC_FMT_1(Type, arg) {
    return "type(" + arg.input + ")";
}