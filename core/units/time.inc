UNIT_FAMILY(Time, Real, Seconds, s);


UNIT_FROM_BASE(Time, Real, Minutes, min) {
    return Ok(value / 60.0);
}

UNIT_TO_BASE(Time, Real, Minutes, min) {
    return Ok(value * 60);
}

UNIT_FROM_BASE(Time, Real, Hours, hr) {
    return Ok(value / 3600.0);
}

UNIT_TO_BASE(Time, Real, Hours, hr) {
    return Ok(value * 3600);
}

UNIT_FROM_BASE(Time, Real, Nanoseconds, ns) {
    return Ok(value * 1000000000);
}

UNIT_TO_BASE(Time, Real, Nanoseconds, ns) {
    return Ok(value * 0.000000001);
}

UNIT_FROM_BASE(Time, Real, Microseconds, us) {
    return Ok(value * 1000000);
}

UNIT_TO_BASE(Time, Real, Microseconds, us) {
    return Ok(value * 0.000001);
}

UNIT_FROM_BASE(Time, Real, Milliseconds, ms) {
    return Ok(value * 1000);
}

UNIT_TO_BASE(Time, Real, Milliseconds, ms) {
    return Ok(value * 0.001);
}
