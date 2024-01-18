#include "checked_math.h"

#include "core/format.h"

#ifdef _MSC_VER
#define ENABLE_INTSAFE_SIGNED_FUNCTIONS
#include <intsafe.h>
#endif

#include <cmath>
#include <limits>
#include <sstream>

namespace RCalc {

#ifdef _MSC_VER

Value CheckedMath::add(Int a, Int b) {
    static_assert(sizeof(Int) == sizeof(INT64));
    Int res;
    if (Int64Add(a, b, reinterpret_cast<INT64*>(&res)) != S_OK) {
        return Value(BigInt(a) + BigInt(b));
    }
    return Value(res);
}

#else
#if __has_builtin(__builtin_add_overflow)

Value CheckedMath::add(Int a, Int b) {
    Int res;
    if (__builtin_add_overflow(a, b, &res)) {
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
#endif

#ifdef _MSC_VER

Value CheckedMath::sub(Int a, Int b) {
    static_assert(sizeof(Int) == sizeof(INT64));
    Int res;
    if (Int64Sub(a, b, reinterpret_cast<INT64*>(&res)) != S_OK) {
        return Value(BigInt(a) - BigInt(b));
    }
    return Value(res);
}

#else
#if __has_builtin(__builtin_sub_overflow)

Value CheckedMath::sub(Int a, Int b) {
    Int res;
    if (__builtin_sub_overflow(a, b, &res)) {
        return Value(BigInt(a) - BigInt(b));
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
#endif

#ifdef _MSC_VER

Value CheckedMath::mul(Int a, Int b) {
    static_assert(sizeof(Int) == sizeof(INT64));
    Int res;
    if (Int64Mult(a, b, reinterpret_cast<INT64*>(&res)) != S_OK) {
        return Value(BigInt(a) * BigInt(b));
    }
    return Value(res);
}

#else
#if __has_builtin(__builtin_mul_overflow)

Value CheckedMath::mul(Int a, Int b) {
    Int res;
    if (__builtin_mul_overflow(a, b, &res)) {
        return Value(BigInt(a) * BigInt(b));
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
#endif

Value CheckedMath::abs(Int a) {
    // The min value will have a magnitude 1 greater than the max value
    // Otherwise, every other value will fit
    if (a == std::numeric_limits<Int>::min()) {
        return Value(BigInt(a) * BigInt("-1"));
    }
    else if (a < 0) {
        return Value(a * -1);
    }
    else {
        return Value(a);
    }
}

Value CheckedMath::neg(Int a) {
    // The min value will have a magnitude 1 greater than the max value
    // Otherwise, every other value will fit
    if (a == std::numeric_limits<Int>::min()) {
        return Value(BigInt(a) * BigInt("-1"));
    }
    else {
        return Value(a * -1);
    }
}

#ifdef _MSC_VER

Value CheckedMath::pow(Int base, Int exp) {
    static_assert(sizeof(Int) == sizeof(INT64));
    if (exp < 0) {
        return Value(std::pow((Real)base, (Real)exp));
    }
    else if (exp == 0) { return Value(Int(1)); }

    Int base_i = base, res_i = 1;
    BigInt base_b { base }, res_b { 1 };
    bool is_bigint = false;

    while (exp > 1) {
        if ((exp % 2) == 1) {
            Int tmp;
            if (!is_bigint) {
                if (Int64Mult(base_i, res_i, &tmp) != S_OK) {
                    is_bigint = true;
                    base_b = BigInt(base_i);
                    res_b = BigInt(res_i);
                }
                else {
                    res_i = tmp;
                }
            }
            if (is_bigint) {
                res_b *= base_b;
            }
            --exp;
        }
        Int tmp;
        if (!is_bigint) {
            if (Int64Mult(base_i, base_i, &tmp) != S_OK) {
                is_bigint = true;
                base_b = BigInt(base_i);
                res_b = BigInt(res_i);
            }
            else {
                base_i = tmp;
            }
        }
        if (is_bigint) {
            base_b *= base_b;
        }
        exp >>= 1;
    }

    Int tmp;
    if (!is_bigint) {
        if (Int64Mult(base_i, res_i, &tmp) != S_OK) {
            is_bigint = true;
            base_b = BigInt(base_i);
            res_b = BigInt(res_i);
        }
        else {
            return tmp;
        }
    }
    
    return base_b * res_b;
}

#else
#if __has_builtin(__builtin_mul_overflow)

Value CheckedMath::pow(Int base, Int exp) {
    if (exp < 0) {
        return Value(std::pow((Real)base, (Real)exp));
    }
    else if (exp == 0) { return Value(Int(1)); }

    Int base_i = base, res_i = 1;
    BigInt base_b { base }, res_b { 1 };
    bool is_bigint = false;

    while (exp > 1) {
        if ((exp % 2) == 1) {
            Int tmp;
            if (!is_bigint) {
                if (__builtin_mul_overflow(base_i, res_i, &tmp)) {
                    is_bigint = true;
                    base_b = BigInt(base_i);
                    res_b = BigInt(res_i);
                }
                else {
                    res_i = tmp;
                }
            }
            if (is_bigint) {
                res_b *= base_b;
            }
            --exp;
        }
        Int tmp;
        if (!is_bigint) {
            if (__builtin_mul_overflow(base_i, base_i, &tmp)) {
                is_bigint = true;
                base_b = BigInt(base_i);
                res_b = BigInt(res_i);
            }
            else {
                base_i = tmp;
            }
        }
        if (is_bigint) {
            base_b *= base_b;
        }
        exp >>= 1;
    }

    Int tmp;
    if (!is_bigint) {
        if (__builtin_mul_overflow(base_i, res_i, &tmp)) {
            is_bigint = true;
            base_b = BigInt(base_i);
            res_b = BigInt(res_i);
        }
        else {
            return tmp;
        }
    }
    
    return base_b * res_b;
}

#else

Value CheckedMath::pow(Int base, Int exp) {
    if (exp < 0) {
        return Value(std::pow((Real)base, (Real)exp));
    }
    else if (exp == 0) { return Value(Int(1)); }

    BigInt ba { a }, bb { b }, res { ba * bb }, ;

    BigInt base_b { base }, res_b { 1 }, min { std::numeric_limits<Int>::min() }, max { std::numeric_limits<Int>::max() };

    while (exp > 1) {
        if ((exp % 2) == 1) {
            res_b *= base_b;
            --exp;
        }
        base_b *= base_b;
        exp >>= 1;
    }
    
    res_b *= base_b;

    if (res_b >= min && res_b <= max) {
        std::stringstream ss;
        ss << str;
        Int res_i;
        ss >> res_i;
        return Value(res_i);
    }

    return Value(res_b);
}

#endif
#endif


Value CheckedMath::discretize(Real a) {
    Real min = std::nextafter((Real)std::numeric_limits<Int>::min(), 0.0); // These numbers might not be representable, so
    Real max = std::nextafter((Real)std::numeric_limits<Int>::max(), 0.0); // find the next representable in the direction of 0 for safety.

    if (a >= min && a <= max) {
        return Value(Int(a));
    }
    return Value(BigInt(fmt("%.0f", a)));
}

}
