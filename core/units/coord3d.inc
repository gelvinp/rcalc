UNIT_FAMILY(Coord3D, Vec3, "Cartesian [X, Y, Z]", cartxyz);


// @RCalcUnit
// Requires: <glm/geometric.hpp>
// Requires: <cmath>
UNIT_FROM_BASE(Coord3D, Vec3, "Cylindrical [r, φ, z]", cylindrical) {
    Real r = glm::length(Vec2(value.x, value.y));

    if (r == 0) {
        return Ok(Vec3(0.0, 0.0, value.z));
    }
    
    Real phi = std::atan2(value.y, value.x);
    return Ok(Vec3(r, phi, value.z));
}

// @RCalcUnit
// Requires: <cmath>
UNIT_TO_BASE(Coord3D, Vec3, "Cylindrical [r, φ, z]", cylindrical) {
    Real x = value.x * std::cos(value.y);
    Real y = value.x * std::sin(value.y);
    return Ok(Vec3(x, y, value.z));
}


// @RCalcUnit
// Requires: <glm/geometric.hpp>
// Requires: <cmath>
UNIT_FROM_BASE(Coord3D, Vec3, "Spherical [r, φ, θ]", spherical) {
    Real r = glm::length(value);

    if (r == 0) {
        return Ok(Vec3(0.0, 0.0, 0.0));
    }
    
    Real phi = std::acos(value.z / r);
    Real theta = std::atan2(value.y, value.x);
    return Ok(Vec3(r, phi, theta));
}

// @RCalcUnit
// Requires: <cmath>
UNIT_TO_BASE(Coord3D, Vec3, "Spherical [r, φ, θ]", spherical) {
    Real r_sin_theta = value.x * std::sin(value.z);

    Real x = r_sin_theta * std::cos(value.y);
    Real y = r_sin_theta * std::sin(value.y);
    Real z = value.x * std::cos(value.z);
    return Ok(Vec3(x, y, z));
}