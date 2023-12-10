UNIT_FAMILY(Temperature, Real, Celsius, c);


UNIT_FROM_BASE(Temperature, Real, Fahrenheit, f) {
    return Ok((9.0 / 5.0) * (value - 273.15) + 32.0);
}

UNIT_TO_BASE(Temperature, Real, Fahrenheit, f) {
    return Ok((5.0 / 9.0) * (value - 32.0) + 273.15);
}


UNIT_FROM_BASE(Temperature, Real, Kelvin, k) {
    return Ok(value + 273.15);
}

UNIT_TO_BASE(Temperature, Real, Kelvin, k) {
    return Ok(value - 273.15);
}
