UNIT_FAMILY(Storage, Real, Bytes, byte);


UNIT_FROM_BASE(Storage, Real, Bits, bit) {
    return Ok(value * 8.0);
}

UNIT_TO_BASE(Storage, Real, Bits, bit) {
    return Ok(value / 8.0);
}


UNIT_FROM_BASE(Storage, Real, Kilobytes, kb) {
    return Ok(value / 1000);
}

UNIT_TO_BASE(Storage, Real, Kilobytes, kb) {
    return Ok(value * 1000);
}


UNIT_FROM_BASE(Storage, Real, Kibibytes, kib) {
    return Ok(value / 1024);
}

UNIT_TO_BASE(Storage, Real, Kibibytes, kib) {
    return Ok(value * 1024);
}


UNIT_FROM_BASE(Storage, Real, Megabytes, mb) {
    return Ok(value / 1000000);
}

UNIT_TO_BASE(Storage, Real, Megabytes, mb) {
    return Ok(value * 1000000);
}


UNIT_FROM_BASE(Storage, Real, Mebibytes, mib) {
    return Ok(value / 1048576);
}

UNIT_TO_BASE(Storage, Real, Mebibytes, mib) {
    return Ok(value * 1048576);
}


UNIT_FROM_BASE(Storage, Real, Gigabytes, gb) {
    return Ok(value / 1000000000);
}

UNIT_TO_BASE(Storage, Real, Gigabytes, gb) {
    return Ok(value * 1000000000);
}


UNIT_FROM_BASE(Storage, Real, Gibibytes, gib) {
    return Ok(value / 1073741824);
}

UNIT_TO_BASE(Storage, Real, Gibibytes, gib) {
    return Ok(value * 1073741824);
}


UNIT_FROM_BASE(Storage, Real, Terabytes, tb) {
    return Ok(value / 1000000000000);
}

UNIT_TO_BASE(Storage, Real, Terabytes, tb) {
    return Ok(value * 1000000000000);
}


UNIT_FROM_BASE(Storage, Real, Tebibytes, tib) {
    return Ok(value / 1099511627776);
}

UNIT_TO_BASE(Storage, Real, Tebibytes, tib) {
    return Ok(value * 1099511627776);
}


UNIT_FROM_BASE(Storage, Real, Petabytes, pb) {
    return Ok(value / 1000000000000000);
}

UNIT_TO_BASE(Storage, Real, Petabytes, pb) {
    return Ok(value * 1000000000000000);
}


UNIT_FROM_BASE(Storage, Real, Pebibytes, pib) {
    return Ok(value / 1125899906842624);
}

UNIT_TO_BASE(Storage, Real, Pebibytes, pib) {
    return Ok(value * 1125899906842624);
}