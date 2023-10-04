// @RCalcOperator
// Name: DegToRad
// Parameters: [Real]
// Description: Convert arg0 degrees to radians.
// Requires: <numbers>
// Tags: [no_expr, real_cast]
RCALC_OP_1(DegToRad, Real, arg) {
    return Ok(Value((arg / 360.0) * 2.0 * std::numbers::pi_v<Real>));
}

RCALC_FMT_1(DegToRad, arg) {
    return arg.get_input_formatted() + "°";
}


// @RCalcOperator
// Name: RadToDeg
// Parameters: [Real]
// Description: Convert arg0 radians to degrees.
// Requires: <numbers>
// Tags: [no_expr, real_cast]
RCALC_OP_1(RadToDeg, Real, arg) {
    return Ok(Value((arg * 360.0) / (2.0 * std::numbers::pi_v<Real>)));
}

RCALC_FMT_1(RadToDeg, arg) {
    return arg.get_input_formatted() + " rad";
}


// @RCalcOperator
// Name: CartToPolar
// Parameters: [Vec2]
// Description: Convert [x, y] to [r, θ].
// Requires: <glm/geometric.hpp>
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(CartToPolar, Vec2, arg) {
    Real r = glm::length(arg);

    if (r == 0) {
        return Ok(Value(Vec2(0.0, 0.0)));
    }
    
    Real theta = std::atan2(arg.y, arg.x);
    return Ok(Value(Vec2(r, theta)));
}

RCALC_FMT_1(CartToPolar, arg) {
    return "CartToPolar(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: PolarToCart
// Parameters: [Vec2]
// Description: Convert [r, θ] to [x, y].
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(PolarToCart, Vec2, arg) {
    Real x = arg.x * std::cos(arg.y);
    Real y = arg.x * std::sin(arg.y);
    return Ok(Value(Vec2(x, y)));
}

RCALC_FMT_1(PolarToCart, arg) {
    return "PolarToCart(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: CartToCylindrical
// Parameters: [Vec3]
// Description: Convert [x, y, z] to [r, φ, z].
// Requires: <glm/geometric.hpp>
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(CartToCylindrical, Vec3, arg) {
    Real r = glm::length(arg);

    if (r == 0) {
        return Ok(Value(Vec3(0.0, 0.0, arg.z)));
    }
    
    Real phi = std::atan2(arg.y, arg.x);
    return Ok(Value(Vec3(r, phi, arg.z)));
}

RCALC_FMT_1(CartToCylindrical, arg) {
    return "CartToCylindrical(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: CylindricalToCart
// Parameters: [Vec3]
// Description: Convert [r, φ, z] to [x, y, z].
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(CylindricalToCart, Vec3, arg) {
    Real x = arg.x * std::cos(arg.y);
    Real y = arg.x * std::sin(arg.y);
    return Ok(Value(Vec3(x, y, arg.z)));
}

RCALC_FMT_1(CylindricalToCart, arg) {
    return "CylindricalToCart(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: CartToSpherical
// Parameters: [Vec3]
// Description: Convert [x, y, z] to [r, φ, θ].
// Requires: <glm/geometric.hpp>
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(CartToSpherical, Vec3, arg) {
    Real r = glm::length(arg);

    if (r == 0) {
        return Ok(Value(Vec3(0.0, 0.0, 0.0)));
    }
    
    Real phi = std::acos(arg.z / r);
    Real theta = std::atan2(arg.y, arg.x);
    return Ok(Value(Vec3(r, phi, theta)));
}

RCALC_FMT_1(CartToSpherical, arg) {
    return "CartToSpherical(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: SphericalToCart
// Parameters: [Vec3]
// Description: Convert [r, φ, θ] to [x, y, z].
// Requires: <cmath>
// Tags: [no_expr, real_cast]
RCALC_OP_1(SphericalToCart, Vec3, arg) {
    Real r_sin_theta = arg.x * std::sin(arg.z);

    Real x = r_sin_theta * std::cos(arg.y);
    Real y = r_sin_theta * std::sin(arg.y);
    Real z = arg.x * std::cos(arg.z);

    return Ok(Value(Vec3(x, y, z)));
}

RCALC_FMT_1(SphericalToCart, arg) {
    return "SphericalToCart(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: RGBToHSL
// Parameters: [Vec3]
// Description: Convert linear [r, g, b] to [h, s, l].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(RGBToHSL, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 1.0 || arg.g < 0.0 || arg.g > 1.0 || arg.b < 0.0 || arg.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }
    
    Real max_chroma = std::max(arg.r, std::max(arg.g, arg.b));
    Real min_chroma = std::min(arg.r, std::min(arg.g, arg.b));
    Real chroma = max_chroma - min_chroma;

    Real hue;
    Real saturation;
    Real lightness = 0.5 * (max_chroma + min_chroma);

    if (chroma == 0.0) {
        hue = 0.0;
        saturation = 0.0;
    }
    else {
        if (max_chroma == arg.r) {
            hue = std::fmod(((arg.g - arg.b) / chroma), 6);
        }
        else if (max_chroma == arg.g) {
            hue = ((arg.b - arg.r) / chroma) + 2;
        }
        else /* max_chroma == arg.b */ {
            hue = ((arg.r - arg.g) / chroma) + 4;
        }
        hue *= 60.0;

        if (lightness == 0.0 || lightness == 1.0) {
            saturation = 0.0;
        }
        else {
            saturation = chroma / (1 - std::abs(2 * lightness - 1));
        }
    }

    return Ok(Value(Vec3(hue, saturation, lightness)));
}

RCALC_FMT_1(RGBToHSL, arg) {
    return "RGBToHSL(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: RGBToHSV
// Parameters: [Vec3]
// Description: Convert linear [r, g, b] to [h, s, v].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(RGBToHSV, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 1.0 || arg.g < 0.0 || arg.g > 1.0 || arg.b < 0.0 || arg.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }

    Real max_chroma = std::max(arg.r, std::max(arg.g, arg.b));
    Real min_chroma = std::min(arg.r, std::min(arg.g, arg.b));
    Real chroma = max_chroma - min_chroma;

    Real hue;
    Real saturation;
    Real value = max_chroma;

    if (chroma == 0.0) {
        hue = 0.0;
        saturation = 0.0;
    }
    else {
        if (max_chroma == arg.r) {
            hue = std::fmod(((arg.g - arg.b) / chroma), 6);
        }
        else if (max_chroma == arg.g) {
            hue = ((arg.b - arg.r) / chroma) + 2;
        }
        else /* max_chroma == arg.b */ {
            hue = ((arg.r - arg.g) / chroma) + 4;
        }
        hue *= 60.0;

        if (value == 0.0) {
            saturation = 0.0;
        }
        else {
            saturation = chroma / value;
        }
    }

    return Ok(Value(Vec3(hue, saturation, value)));
}

RCALC_FMT_1(RGBToHSV, arg) {
    return "RGBToHSV(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: RGBToOkLab
// Parameters: [Vec3]
// Description: Convert linear [r, g, b] to [L, a, b].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(RGBToOkLab, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 1.0 || arg.g < 0.0 || arg.g > 1.0 || arg.b < 0.0 || arg.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }
    
    Real l = 0.4122214708f * arg.r + 0.5363325363f * arg.g + 0.0514459929f * arg.b;
	Real m = 0.2119034982f * arg.r + 0.6806995451f * arg.g + 0.1073969566f * arg.b;
	Real s = 0.0883024619f * arg.r + 0.2817188376f * arg.g + 0.6299787005f * arg.b;

    Real l_ = std::cbrt(l);
    Real m_ = std::cbrt(m);
    Real s_ = std::cbrt(s);

    Vec3 color {
        0.2104542553f*l_ + 0.7936177850f*m_ - 0.0040720468f*s_,
        1.9779984951f*l_ - 2.4285922050f*m_ + 0.4505937099f*s_,
        0.0259040371f*l_ + 0.7827717662f*m_ - 0.8086757660f*s_,
    };

    return Ok(Value(color));
}

RCALC_FMT_1(RGBToOkLab, arg) {
    return "RGBToOkLab(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: HSLToRGB
// Parameters: [Vec3]
// Description: Convert [h, s, l] to linear [r, g, b].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(HSLToRGB, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 360.0 || arg.g < 0.0 || arg.g > 1.0 || arg.b < 0.0 || arg.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 360], [0, 1], [0, 1]])");
    }

    auto f = [&arg](Real n) {
        Real k = std::fmod((n + (arg.x / 30.0)), 12.0);
        Real a = arg.y * std::min(arg.z, 1.0 - arg.z);
        return arg.z - a * std::max(-1.0, std::min(k - 3.0, std::min(9.0 - k, 1.0)));
    };

    Real r = f(0);
    Real g = f(8);
    Real b = f(4);

    return Ok(Value(Vec3(r, g, b)));
}

RCALC_FMT_1(HSLToRGB, arg) {
    return "HSLToRGB(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: HSLToHSV
// Parameters: [Vec3]
// Description: Convert [h, s, l] to [h, s, v].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(HSLToHSV, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 360.0 || arg.g < 0.0 || arg.g > 1.0 || arg.b < 0.0 || arg.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 360], [0, 1], [0, 1]])");
    }
    
    Real value = arg.z + arg.y * std::min(arg.z, 1.0 - arg.z);
    Real saturation = (value == 0.0) ? 0.0 : (2.0 * (1.0 - (arg.z / value)));

    return Ok(Value(Vec3(arg.x, saturation, value)));
}

RCALC_FMT_1(HSLToHSV, arg) {
    return "HSLToHSV(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: HSLToOklab
// Parameters: [Vec3]
// Description: Convert [h, s, l] to [L, a, b].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(HSLToOklab, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 360.0 || arg.g < 0.0 || arg.g > 1.0 || arg.b < 0.0 || arg.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 360], [0, 1], [0, 1]])");
    }

    auto f = [&arg](Real n) {
        Real k = std::fmod((n + (arg.x / 30.0)), 12.0);
        Real a = arg.y * std::min(arg.z, 1.0 - arg.z);
        return arg.z - a * std::max(-1.0, std::min(k - 3.0, std::min(9.0 - k, 1.0)));
    };

    Real r = f(0);
    Real g = f(8);
    Real b = f(4);
    
    Real l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
	Real m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
	Real s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

    Real l_ = std::cbrt(l);
    Real m_ = std::cbrt(m);
    Real s_ = std::cbrt(s);

    Vec3 color {
        0.2104542553f*l_ + 0.7936177850f*m_ - 0.0040720468f*s_,
        1.9779984951f*l_ - 2.4285922050f*m_ + 0.4505937099f*s_,
        0.0259040371f*l_ + 0.7827717662f*m_ - 0.8086757660f*s_,
    };

    return Ok(Value(color));
}

RCALC_FMT_1(HSLToOklab, arg) {
    return "HSLToOklab(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: HSVToRGB
// Parameters: [Vec3]
// Description: Convert [h, s, v] to linear [r, g, b].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(HSVToRGB, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 360.0 || arg.g < 0.0 || arg.g > 1.0 || arg.b < 0.0 || arg.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 360], [0, 1], [0, 1]])");
    }

    auto f = [&arg](Real n) {
        Real k = std::fmod((n + (arg.x / 60.0)), 6.0);
        return arg.z - arg.z * arg.y * std::max(0.0, std::min(k, std::min(4.0 - k, 1.0)));
    };

    Real r = f(5);
    Real g = f(3);
    Real b = f(1);

    return Ok(Value(Vec3(r, g, b)));
}

RCALC_FMT_1(HSVToRGB, arg) {
    return "HSVToRGB(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: HSVToHSL
// Parameters: [Vec3]
// Description: Convert [h, s, v] to [h, s, l].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(HSVToHSL, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 360.0 || arg.g < 0.0 || arg.g > 1.0 || arg.b < 0.0 || arg.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 360], [0, 1], [0, 1]])");
    }
    
    Real lightness = arg.z * (1.0 - (arg.y / 2.0));
    Real saturation = (lightness == 0.0) ? 0.0 : ((arg.z - lightness) / std::min(lightness, 1.0 - lightness));

    return Ok(Value(Vec3(arg.x, saturation, lightness)));
}

RCALC_FMT_1(HSVToHSL, arg) {
    return "HSVToHSL(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: HSVToOklab
// Parameters: [Vec3]
// Description: Convert [h, s, v] to [L, a, b].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(HSVToOklab, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 360.0 || arg.g < 0.0 || arg.g > 1.0 || arg.b < 0.0 || arg.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 360], [0, 1], [0, 1]])");
    }

    auto f = [&arg](Real n) {
        Real k = std::fmod((n + (arg.x / 60.0)), 6.0);
        return arg.z - arg.z * arg.y * std::max(0.0, std::min(k, std::min(4.0 - k, 1.0)));
    };

    Real r = f(5);
    Real g = f(3);
    Real b = f(1);
    
    Real l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
	Real m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
	Real s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

    Real l_ = std::cbrt(l);
    Real m_ = std::cbrt(m);
    Real s_ = std::cbrt(s);

    Vec3 color {
        0.2104542553f*l_ + 0.7936177850f*m_ - 0.0040720468f*s_,
        1.9779984951f*l_ - 2.4285922050f*m_ + 0.4505937099f*s_,
        0.0259040371f*l_ + 0.7827717662f*m_ - 0.8086757660f*s_,
    };

    return Ok(Value(color));
}

RCALC_FMT_1(HSVToOklab, arg) {
    return "HSVToOklab(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: OklabToRGB
// Parameters: [Vec3]
// Description: Convert [L, a, b] to linear [r, g, b].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(OklabToRGB, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 1.0 || arg.g < -0.4 || arg.g > 0.4 || arg.b < -0.4 || arg.b > 0.4) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 1], [-0.4, 0.4], [-0.4, 0.4]])");
    }
    
    Real l_ = arg.r + 0.3963377774f * arg.g + 0.2158037573f * arg.b;
    Real m_ = arg.r - 0.1055613458f * arg.g - 0.0638541728f * arg.b;
    Real s_ = arg.r - 0.0894841775f * arg.g - 1.2914855480f * arg.b;

    Real l = l_*l_*l_;
    Real m = m_*m_*m_;
    Real s = s_*s_*s_;

    Vec3 color {
		+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
		-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
		-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
    };

    return Ok(Value(color));
}

RCALC_FMT_1(OklabToRGB, arg) {
    return "OklabToRGB(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: OklabToHSL
// Parameters: [Vec3]
// Description: Convert [L, a, b] to [h, s, l].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(OklabToHSL, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 1.0 || arg.g < -0.4 || arg.g > 0.4 || arg.b < -0.4 || arg.b > 0.4) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 1], [-0.4, 0.4], [-0.4, 0.4]])");
    }
    
    Real l_ = arg.r + 0.3963377774f * arg.g + 0.2158037573f * arg.b;
    Real m_ = arg.r - 0.1055613458f * arg.g - 0.0638541728f * arg.b;
    Real s_ = arg.r - 0.0894841775f * arg.g - 1.2914855480f * arg.b;

    Real l = l_*l_*l_;
    Real m = m_*m_*m_;
    Real s = s_*s_*s_;

    Vec3 color {
		+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
		-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
		-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
    };
    
    Real max_chroma = std::max(color.r, std::max(color.g, color.b));
    Real min_chroma = std::min(color.r, std::min(color.g, color.b));
    Real chroma = max_chroma - min_chroma;

    Real hue;
    Real saturation;
    Real lightness = 0.5 * (max_chroma + min_chroma);

    if (chroma == 0.0) {
        hue = 0.0;
        saturation = 0.0;
    }
    else {
        if (max_chroma == color.r) {
            hue = std::fmod(((color.g - color.b) / chroma), 6);
        }
        else if (max_chroma == color.g) {
            hue = ((color.b - color.r) / chroma) + 2;
        }
        else /* max_chroma == color.b */ {
            hue = ((color.r - color.g) / chroma) + 4;
        }
        hue *= 60.0;

        if (lightness == 0.0 || lightness == 1.0) {
            saturation = 0.0;
        }
        else {
            saturation = chroma / (1 - std::abs(2 * lightness - 1));
        }
    }

    return Ok(Value(Vec3(hue, saturation, lightness)));
}

RCALC_FMT_1(OklabToHSL, arg) {
    return "OklabToHSL(" + arg.get_input_formatted() + ")";
}


// @RCalcOperator
// Name: OklabToHSV
// Parameters: [Vec3]
// Description: Convert [L, a, b] to [h, s, v].
// Requires: <cmath>
// Tags: [no_expr]
RCALC_OP_1(OklabToHSV, Vec3, arg) {
    if (arg.r < 0.0 || arg.r > 1.0 || arg.g < -0.4 || arg.g > 0.4 || arg.b < -0.4 || arg.b > 0.4) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 1], [-0.4, 0.4], [-0.4, 0.4]])");
    }
    
    Real l_ = arg.r + 0.3963377774f * arg.g + 0.2158037573f * arg.b;
    Real m_ = arg.r - 0.1055613458f * arg.g - 0.0638541728f * arg.b;
    Real s_ = arg.r - 0.0894841775f * arg.g - 1.2914855480f * arg.b;

    Real l = l_*l_*l_;
    Real m = m_*m_*m_;
    Real s = s_*s_*s_;

    Vec3 color {
		+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
		-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
		-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
    };

    Real max_chroma = std::max(color.r, std::max(color.g, color.b));
    Real min_chroma = std::min(color.r, std::min(color.g, color.b));
    Real chroma = max_chroma - min_chroma;

    Real hue;
    Real saturation;
    Real value = max_chroma;

    if (chroma == 0.0) {
        hue = 0.0;
        saturation = 0.0;
    }
    else {
        if (max_chroma == color.r) {
            hue = std::fmod(((color.g - color.b) / chroma), 6);
        }
        else if (max_chroma == color.g) {
            hue = ((color.b - color.r) / chroma) + 2;
        }
        else /* max_chroma == color.b */ {
            hue = ((color.r - color.g) / chroma) + 4;
        }
        hue *= 60.0;

        if (value == 0.0) {
            saturation = 0.0;
        }
        else {
            saturation = chroma / value;
        }
    }

    return Ok(Value(Vec3(hue, saturation, value)));
}

RCALC_FMT_1(OklabToHSV, arg) {
    return "OklabToHSV(" + arg.get_input_formatted() + ")";
}