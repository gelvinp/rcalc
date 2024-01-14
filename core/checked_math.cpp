#include "checked_math.h"

#ifdef MSVC
#include <intsafe.h>
#endif

#include <limits>

namespace RCalc {

#if __has_builtin(__builtin_add_overflow)

Value CheckedMath::add(Int a, Int b) {
    Int res;
    if (__builtin_add_overflow(a, b, &res)) {
        return Value(BigInt(a) + BigInt(b));
    }
    return Value(res);
}

#elif defined(MSVC)

Value CheckedMath::add(Int a, Int b) {
    static_assert(sizeof(Int) == sizeof(LONG));
    Int res;
    if (LongAdd(reinterpret_cast<LONG>(a), reinterpret_cast<LONG>(b), reinterpret_cast<LONG*>(&res)) != S_OK) {
        return Value(BigInt(a) + BigInt(b));
    }
    return Value(res);
}

#else

Value CheckedMath::add(Int a, Int b) {
    BigInt ba { a }, bb { b }, res { ba + bb }, min { std::numeric_limits<Int>::min() }, max { std::numeric_limits<Int>::max() };

    if (res >= min && res <= max) { return Value(a + b); }
    return Value(res);
}

#endif

#if __has_builtin(__builtin_sub_overflow)

Value CheckedMath::sub(Int a, Int b) {
    Int res;
    if (__builtin_sub_overflow(a, b, &res)) {
        return Value(BigInt(a) - BigInt(b));
    }
    return Value(res);
}

#elif defined(MSVC)

Value CheckedMath::sub(Int a, Int b) {
    static_assert(sizeof(Int) == sizeof(LONG));
    Int res;
    if (LongSub(reinterpret_cast<LONG>(a), reinterpret_cast<LONG>(b), reinterpret_cast<LONG*>(&res)) != S_OK) {
        return Value(BigInt(a) + BigInt(b));
    }
    return Value(res);
}

#else

Value CheckedMath::sub(Int a, Int b) {
    BigInt ba { a }, bb { b }, res { ba - bb }, min { std::numeric_limits<Int>::min() }, max { std::numeric_limits<Int>::max() };

    if (res >= min && res <= max) { return Value(a - b); }
    return Value(res);
}

#endif

#if __has_builtin(__builtin_mul_overflow)

Value CheckedMath::mul(Int a, Int b) {
    Int res;
    if (__builtin_mul_overflow(a, b, &res)) {
        return Value(BigInt(a) * BigInt(b));
    }
    return Value(res);
}

#elif defined(MSVC)

Value CheckedMath::mul(Int a, Int b) {
    static_assert(sizeof(Int) == sizeof(LONG));
    Int res;
    if (LongMult(reinterpret_cast<LONG>(a), reinterpret_cast<LONG>(b), reinterpret_cast<LONG*>(&res)) != S_OK) {
        return Value(BigInt(a) + BigInt(b));
    }
    return Value(res);
}

#else

Value CheckedMath::mul(Int a, Int b) {
    BigInt ba { a }, bb { b }, res { ba * bb }, min { std::numeric_limits<Int>::min() }, max { std::numeric_limits<Int>::max() };

    if (res >= min && res <= max) { return Value(a * b); }
    return Value(res);
}

#endif

}
