// @RCalcUnit
// SortOrder: 10
UNIT_FAMILY(Volume, Real, Cubic Meters, m3);


UNIT_FROM_BASE(Volume, Real, US Teaspoons, tsp) {
    return Ok(value * 202884);
}

UNIT_TO_BASE(Volume, Real, US Teaspoons, tsp) {
    return Ok(value / 202884);
}

UNIT_FROM_BASE(Volume, Real, US Tablespoons, tbsp) {
    return Ok(value * 67628);
}

UNIT_TO_BASE(Volume, Real, US Tablespoons, tbsp) {
    return Ok(value / 67628);
}

UNIT_FROM_BASE(Volume, Real, US Fluid Ounces, floz) {
    return Ok(value * 33814);
}

UNIT_TO_BASE(Volume, Real, US Fluid Ounces, floz) {
    return Ok(value / 33814);
}

UNIT_FROM_BASE(Volume, Real, US Cups, cup) {
    return Ok(value * 4226.75);
}

UNIT_TO_BASE(Volume, Real, US Cups, cup) {
    return Ok(value / 4226.75);
}

UNIT_FROM_BASE(Volume, Real, US Liquid Pints, pt) {
    return Ok(value * 2113.38);
}

UNIT_TO_BASE(Volume, Real, US Liquid Pints, pt) {
    return Ok(value / 2113.38);
}

UNIT_FROM_BASE(Volume, Real, US Liquid Quarts, qt) {
    return Ok(value * 1056.69);
}

UNIT_TO_BASE(Volume, Real, US Liquid Quarts, qt) {
    return Ok(value / 1056.69);
}

UNIT_FROM_BASE(Volume, Real, US Liquid Gallons, gal) {
    return Ok(value * 264.172);
}

UNIT_TO_BASE(Volume, Real, US Liquid Gallons, gal) {
    return Ok(value / 264.172);
}

UNIT_FROM_BASE(Volume, Real, Milliliters, ml) {
    return Ok(value * 1000000);
}

UNIT_TO_BASE(Volume, Real, Milliliters, ml) {
    return Ok(value * 0.000001);
}

UNIT_FROM_BASE(Volume, Real, Liters, l) {
    return Ok(value * 1000);
}

UNIT_TO_BASE(Volume, Real, Liters, l) {
    return Ok(value * 0.001);
}

UNIT_FROM_BASE(Volume, Real, Cubic Centimeters, cm3) {
    return Ok(value * 1000000);
}

UNIT_TO_BASE(Volume, Real, Cubic Centimeters, cm3) {
    return Ok(value * 0.000001);
}

UNIT_FROM_BASE(Volume, Real, Cubic Kilometers, km3) {
    return Ok(value * 0.000000001);
}

UNIT_TO_BASE(Volume, Real, Cubic Kilometers, km3) {
    return Ok(value * 1000000000);
}