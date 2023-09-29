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
// Parameters: [Real]
// Tags: [no_expr]
RCALC_OP_1(Type, Real, _arg) {
    UNUSED(_arg);
    return Ok(Value((Int)Value::TYPE_REAL));
}

RCALC_FMT_1(Type, arg) {
    return "type(" + arg.input + ")";
}