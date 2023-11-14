UNIT_FAMILY(Area, Real, Acres, acre);


UNIT_FROM_BASE(Area, Real, Square Feet, sqft) {
    return Ok(value * 43560);
}

UNIT_TO_BASE(Area, Real, Square Feet, sqft) {
    return Ok(value * 0.0000229568);
}


UNIT_FROM_BASE(Area, Real, Square Kilometers, sqkm) {
    return Ok(value * 0.00404686);
}

UNIT_TO_BASE(Area, Real, Square Kilometers, sqkm) {
    return Ok(value * 247.105);
}
