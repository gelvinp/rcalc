UNIT_FAMILY(Distance, "Meters", Real, m);

UNIT_FROM_BASE(Distance, "Feet", Real, ft) {
    return value * 3.28084;
}

UNIT_TO_BASE(Distance, "Feet", Real, ft) {
    return value * 0.3048;
}