UNIT_FAMILY(Angle, Real, Radians, rad);


// @RCalcUnit
// Requires: <numbers>
UNIT_FROM_BASE(Angle, Real, Degrees, deg) {
    return Ok((value * 360.0) / (2.0 * std::numbers::pi_v<Real>));
}

// @RCalcUnit
// Requires: <numbers>
UNIT_TO_BASE(Angle, Real, Degrees, deg) {
    return Ok((value / 360.0) * (2.0 * std::numbers::pi_v<Real>));
}
