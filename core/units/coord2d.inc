UNIT_FAMILY(Coord2D, Vec2, "Cartesian [X, Y]", cartxy);


// @RCalcUnit
// Requires: <glm/geometric.hpp>
// Requires: <cmath>
UNIT_FROM_BASE(Coord2D, Vec2, "Polar [r, θ]", polar) {
    Real r = glm::length(value);

    if (r == 0) {
        return Ok(Vec2(0.0, 0.0));
    }
    
    Real theta = std::atan2(value.y, value.x);
    return Ok(Vec2(r, theta));
}

// @RCalcUnit
// Requires: <cmath>
// Requires: <numbers>
UNIT_TO_BASE(Coord2D, Vec2, "Polar [r, θ]", polar) {
    if (value.x < 0.0) {
        value.x *= -1;
        value.y += std::numbers::pi_v<Real>;
    }

    Real x = value.x * std::cos(value.y);
    Real y = value.x * std::sin(value.y);
    return Ok(Vec2(x, y));
}