// @RCalcOperator
// Tags: [bigint_cast]
RCALC_OP_2(Add, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}


// @RCalcOperator
// Tags: [bigint_cast]
RCALC_OP_2(Sub, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}


// @RCalcOperator
// Tags: [bigint_cast]
RCALC_OP_2(Mul, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Tags: [bigint_cast]
RCALC_OP_2(Quot, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}


// @RCalcOperator
// Tags: [bigint_cast]
RCALC_OP_1(Sqr, BigInt, arg0) {
    return Ok(Value(arg0 * arg0));
}


// @RCalcOperator
// Tags: [bigint_cast]
RCALC_OP_1(Cube, BigInt, arg0) {
    BigInt three { "3" };
    return Ok(Value(big_pow(arg0, three)));
}


// @RCalcOperator
// Tags: [bigint_cast]
RCALC_OP_2(Pow, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(big_pow(arg0, arg1)));
}


// @RCalcOperator
// Tags: [bigint_cast]
RCALC_OP_2(Mod, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(arg0 % arg1));
}


// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Abs, BigInt, arg) {
    return Ok(Value(big_abs(arg)));
}


// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Fact, BigInt, arg) {
    return Ok(Value(big_fact(arg)));
}


// @RCalcOperator
// Tags: [no_expr]
RCALC_OP_1(Neg, BigInt, arg) {
    return Ok(Value(arg * BigInt("-1")));
}