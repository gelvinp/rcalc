UNIT_FAMILY(Mass, Real, Kilograms, kg);


UNIT_FROM_BASE(Mass, Real, Ounces, oz) {
    return Ok(value * 35.274);
}

UNIT_TO_BASE(Mass, Real, Ounces, oz) {
    return Ok(value * 0.0283495);
}

UNIT_FROM_BASE(Mass, Real, Pounds, lbs) {
    return Ok(value * 2.20462);
}

UNIT_TO_BASE(Mass, Real, Pounds, lbs) {
    return Ok(value * 0.453592);
}

UNIT_FROM_BASE(Mass, Real, Short Tons, stn) {
    return Ok(value * 0.00110231);
}

UNIT_TO_BASE(Mass, Real, Short Tons, stn) {
    return Ok(value * 907.185);
}

UNIT_FROM_BASE(Mass, Real, Long Tons, ltn) {
    return Ok(value * 0.000984207);
}

UNIT_TO_BASE(Mass, Real, Long Tons, ltn) {
    return Ok(value * 1016.05);
}

UNIT_FROM_BASE(Mass, Real, Metric Tonnes, mtn) {
    return Ok(value * 0.001);
}

UNIT_TO_BASE(Mass, Real, Metric Tonnes, mtn) {
    return Ok(value * 1000);
}

UNIT_FROM_BASE(Mass, Real, Milligrams, mg) {
    return Ok(value * 1000000);
}

UNIT_TO_BASE(Mass, Real, Milligrams, mg) {
    return Ok(value * 0.000001);
}

UNIT_FROM_BASE(Mass, Real, Grams, g) {
    return Ok(value * 1000);
}

UNIT_TO_BASE(Mass, Real, Grams, g) {
    return Ok(value * 0.001);
}

UNIT_FROM_BASE(Mass, Real, Stones, st) {
    return Ok(value * 0.157473);
}

UNIT_TO_BASE(Mass, Real, Stones, st) {
    return Ok(value * 6.35029);
}