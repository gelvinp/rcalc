#include "comparison.h"

#include "core/value.h"

namespace RCalc::TypeComparison {

bool compare(std::string_view a, std::string_view b) {
    Value parsed_a = *Value::parse(a), parsed_b = *Value::parse(b);
    return parsed_a == parsed_b;
}

bool compare(Real a, Real b) {
    if (a == b) { return true; }

    Real max_diff = fmax(1e-5, fabs(a) * 1e-5);
    if (fabs(a - b) < max_diff) { return true; }

    // Might be angles
    a = fmod(a, 2.0 * std::numbers::pi_v<Real>);
    b = fmod(b, 2.0 * std::numbers::pi_v<Real>);

    if (a < 0.0) { a += (2.0 * std::numbers::pi_v<Real>); }
    if (b < 0.0) { b += (2.0 * std::numbers::pi_v<Real>); }

    if (fabs(a - b) < max_diff) { return true; }

    Logger::log_err("%f does not equal %f!", a, b);

    return false;
}

bool compare(Vec2 a, Vec2 b) {
    if (compare(a.x, b.x) && compare(a.y, b.y)) { return true; };

    // Might be polar coords
    if (a.x < 0.0) {
        a.x *= -1;
        a.y += std::numbers::pi_v<Real>;
    }

    a.y = fmod(a.y, 2.0 * std::numbers::pi_v<Real>);
    b.y = fmod(b.y, 2.0 * std::numbers::pi_v<Real>);

    if (a.y < 0.0) { a += (2.0 * std::numbers::pi_v<Real>); }
    if (b.y < 0.0) { b += (2.0 * std::numbers::pi_v<Real>); }

    if (compare(a.x, b.x) && compare(a.y, b.y)) { return true; }

    return false;
}

bool compare(Vec3 a, Vec3 b) {
    if (compare(a.x, b.x) && compare(a.y, b.y) && compare(a.z, b.z)) { return true; }

    Vec3 _a = a, _b = b;

    // Might be polar coords
    if (a.x < 0.0) {
        a.x *= -1;
        a.y += std::numbers::pi_v<Real>;
    }

    a.y = fmod(a.y, 2.0 * std::numbers::pi_v<Real>);
    b.y = fmod(b.y, 2.0 * std::numbers::pi_v<Real>);

    if (a.y < 0.0) { a += (2.0 * std::numbers::pi_v<Real>); }
    if (b.y < 0.0) { b += (2.0 * std::numbers::pi_v<Real>); }

    if (compare(a.x, b.x) && compare(a.y, b.y) && compare(a.z, b.z)) { return true; }

    a = _a; b = _b;

    // Might be spherical coords
    if (a.x < 0.0) {
        a.x *= -1;
        a.y *= -1;
    }

    a.y = fmod(a.y, 2.0 * std::numbers::pi_v<Real>);
    b.y = fmod(b.y, 2.0 * std::numbers::pi_v<Real>);

    if (a.y < 0.0) { a += (2.0 * std::numbers::pi_v<Real>); }
    if (b.y < 0.0) { b += (2.0 * std::numbers::pi_v<Real>); }

    if (a.y > std::numbers::pi_v<Real>) {
        a.y -= (2.0 * std::numbers::pi_v<Real>);
        a.y *= -1;
        a.z += std::numbers::pi_v<Real>;
    }

    a.z = fmod(a.y, 2.0 * std::numbers::pi_v<Real>);
    b.z = fmod(b.y, 2.0 * std::numbers::pi_v<Real>);
    if (a.z < 0.0) { a += (2.0 * std::numbers::pi_v<Real>); }
    if (b.z < 0.0) { b += (2.0 * std::numbers::pi_v<Real>); }

    if (compare(a.x, b.x) && compare(a.y, b.y) && compare(a.z, b.z)) { return true; }

    return false;
}

bool compare(Vec4 a, Vec4 b) {
    return compare(a.x, b.x) && compare(a.y, b.y) && compare(a.z, b.z) && compare(a.w, b.w);
}

bool compare(Mat2 a, Mat2 b) {
    return compare(a[0], b[0]) && compare(a[1], b[1]);
}

bool compare(Mat3 a, Mat3 b) {
    return compare(a[0], b[0]) && compare(a[1], b[1]) && compare(a[2], b[2]);
}

bool compare(Mat4 a, Mat4 b) {
    return compare(a[0], b[0]) && compare(a[1], b[1]) && compare(a[2], b[2]) && compare(a[3], b[3]);
}

}