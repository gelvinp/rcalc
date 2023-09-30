// @RCalcOperator
// Name: DegToRad
// Parameters: [Real]
// Description: Convert arg0 degrees to radians.
// Requires: <numbers>
// Tags: [no_expr, real_cast]
RCALC_OP_1(DegToRad, Real, arg) {
    return Ok(Value((arg / 360.0) * 2.0 * std::numbers::pi));
}

RCALC_FMT_1(DegToRad, arg) {
    return arg.get_input_formatted() + "°";
}


// @RCalcOperator
// Name: RadToDeg
// Parameters: [Real]
// Description: Convert arg0 radians to degrees.
// Requires: <numbers>
// Tags: [no_expr, real_cast]
RCALC_OP_1(RadToDeg, Real, arg) {
    return Ok(Value((arg * 360.0) / (2.0 * std::numbers::pi)));
}

RCALC_FMT_1(RadToDeg, arg) {
    return arg.get_input_formatted() + " rad";
}