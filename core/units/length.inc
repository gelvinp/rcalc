// @RCalcUnit
// SortOrder: 7
UNIT_FAMILY(Length, Real, Meters, m);


UNIT_FROM_BASE(Length, Real, Inches, in) {
    return Ok(value * 39.3701);
}

UNIT_TO_BASE(Length, Real, Inches, in) {
    return Ok(value * 0.0254);
}

UNIT_FROM_BASE(Length, Real, Feet, ft) {
    return Ok(value * 3.28084);
}

UNIT_TO_BASE(Length, Real, Feet, ft) {
    return Ok(value * 0.3048);
}

UNIT_FROM_BASE(Length, Real, Yards, yd) {
    return Ok(value * 1.09361);
}

UNIT_TO_BASE(Length, Real, Yards, yd) {
    return Ok(value * 0.9144);
}

UNIT_FROM_BASE(Length, Real, Statute Miles, mi) {
    return Ok(value * 0.000621371);
}

UNIT_TO_BASE(Length, Real, Statute Miles, mi) {
    return Ok(value * 1609.34);
}

UNIT_FROM_BASE(Length, Real, Nautical Miles, nm) {
    return Ok(value * 0.000539957);
}

UNIT_TO_BASE(Length, Real, Nautical Miles, nm) {
    return Ok(value * 1852);
}

UNIT_FROM_BASE(Length, Real, Millimeters, mm) {
    return Ok(value * 1000);
}

UNIT_TO_BASE(Length, Real, Millimeters, mm) {
    return Ok(value * 0.001);
}

UNIT_FROM_BASE(Length, Real, Centimeters, cm) {
    return Ok(value * 100);
}

UNIT_TO_BASE(Length, Real, Centimeters, cm) {
    return Ok(value * 0.01);
}

UNIT_FROM_BASE(Length, Real, Kilometers, km) {
    return Ok(value * 0.001);
}

UNIT_TO_BASE(Length, Real, Kilometers, km) {
    return Ok(value * 1000);
}