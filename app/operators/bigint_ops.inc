// @RCalcOperator
// Name: Add
// Parameters: [BigInt, BigInt]
// Tags: [bigint_cast]
RCALC_OP_2(Add, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(arg0 + arg1));
}


// @RCalcOperator
// Name: Sub
// Parameters: [BigInt, BigInt]
// Tags: [bigint_cast]
RCALC_OP_2(Sub, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(arg0 - arg1));
}


// @RCalcOperator
// Name: Mul
// Parameters: [BigInt, BigInt]
// Tags: [bigint_cast]
RCALC_OP_2(Mul, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(arg0 * arg1));
}


// @RCalcOperator
// Name: Quot
// Parameters: [BigInt, BigInt]
// Tags: [bigint_cast]
RCALC_OP_2(Quot, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(arg0 / arg1));
}


// @RCalcOperator
// Name: Pow
// Parameters: [BigInt, BigInt]
// Tags: [bigint_cast]
RCALC_OP_2(Pow, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(big_pow(arg0, arg1)));
}


// @RCalcOperator
// Name: Mod
// Parameters: [BigInt, BigInt]
// Tags: [bigint_cast]
RCALC_OP_2(Mod, BigInt, BigInt, arg0, arg1) {
    return Ok(Value(arg0 % arg1));
}


// @RCalcOperator
// Name: Abs
// Parameters: [BigInt]
RCALC_OP_1(Abs, BigInt, arg) {
    return Ok(Value(big_abs(arg)));
}


// @RCalcOperator
// Name: Fact
// Parameters: [BigInt]
RCALC_OP_1(Fact, BigInt, arg) {
    return Ok(Value(big_fact(arg)));
}


// @RCalcOperator
// Name: Neg
// Parameters: [BigInt]
RCALC_OP_1(Neg, BigInt, arg) {
    return Ok(Value(arg * BigInt("-1")));
}