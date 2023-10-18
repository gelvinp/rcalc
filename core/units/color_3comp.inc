UNIT_FAMILY(Color3Comp, Vec3, CIE D65 XYZ, ciexyz);


// @RCalcUnit
// Requires: "modules/colorm/colorm.h"
UNIT_FROM_BASE(Color3Comp, Vec3, sRGB, rgb) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < 0.0 || value.g > 1.0 || value.b < 0.0 || value.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }

    using namespace colorm;
    XyzD65 src(value.r, value.g, value.b);
    Rgb rgb(src);

    return Ok(Vec3(rgb.red(), rgb.green(), rgb.blue()));
}

// @RCalcUnit
// Requires: "modules/colorm/colorm.h"
UNIT_TO_BASE(Color3Comp, Vec3, sRGB, rgb) {
    if (value.r < 0.0 || value.r > 255.0 || value.g < 0.0 || value.g > 255.0 || value.b < 0.0 || value.b > 255.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 255])");
    }

    using namespace colorm;
    Rgb src(value.r, value.g, value.b);
    XyzD65 cie(src);

    return Ok(Vec3(cie.x(), cie.y(), cie.z()));
}

// @RCalcUnit
// Requires: "modules/colorm/colorm.h"
UNIT_FROM_BASE(Color3Comp, Vec3, HSL, hsl) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < 0.0 || value.g > 1.0 || value.b < 0.0 || value.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }

    using namespace colorm;
    XyzD65 src(value.r, value.g, value.b);
    Hsl hsl(src);

    return Ok(Vec3(hsl.hue(), hsl.saturation(), hsl.lightness()));
}

// @RCalcUnit
// Requires: "modules/colorm/colorm.h"
UNIT_TO_BASE(Color3Comp, Vec3, HSL, hsl) {
    if (value.r < 0.0 || value.r > 360.0 || value.g < 0.0 || value.g > 100.0 || value.b < 0.0 || value.b > 100.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 360], [0, 100], [0, 100]])");
    }

    using namespace colorm;
    Hsl src(value.r, value.g, value.b);
    XyzD65 cie(src);

    return Ok(Vec3(cie.x(), cie.y(), cie.z()));
}

// @RCalcUnit
// Requires: "modules/colorm/colorm.h"
UNIT_FROM_BASE(Color3Comp, Vec3, OKLab, oklab) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < 0.0 || value.g > 1.0 || value.b < 0.0 || value.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }

    using namespace colorm;
    XyzD65 src(value.r, value.g, value.b);
    Oklab lab(src);

    return Ok(Vec3(lab.lightness(), lab.a(), lab.b()));
}

// @RCalcUnit
// Requires: "modules/colorm/colorm.h"
UNIT_TO_BASE(Color3Comp, Vec3, OKLab, oklab) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < -0.4 || value.g > 0.4 || value.b < -0.4 || value.b > 0.4) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 1], [-0.4, 0.4], [-0.4, 0.4]])");
    }

    using namespace colorm;
    Oklab src(value.r, value.g, value.b);
    XyzD65 cie(src);

    return Ok(Vec3(cie.x(), cie.y(), cie.z()));
}

// @RCalcUnit
// Requires: "modules/colorm/colorm.h"
UNIT_FROM_BASE(Color3Comp, Vec3, OKLCh, oklch) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < 0.0 || value.g > 1.0 || value.b < 0.0 || value.b > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }

    using namespace colorm;
    XyzD65 src(value.r, value.g, value.b);
    Oklch lch(src);

    return Ok(Vec3(lch.lightness(), lch.chroma(), lch.hue()));
}

// @RCalcUnit
// Requires: "modules/colorm/colorm.h"
UNIT_TO_BASE(Color3Comp, Vec3, OKLCh, oklch) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < -0.4 || value.g > 0.4 || value.b < -0.4 || value.b > 0.4) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 1], [-0.4, 0.4], [-0.4, 0.4]])");
    }

    using namespace colorm;
    Oklch src(value.r, value.g, value.b);
    XyzD65 cie(src);

    return Ok(Vec3(cie.x(), cie.y(), cie.z()));
}