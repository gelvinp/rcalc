UNIT_FAMILY(Color4Comp, Vec4, CIE XYZA, ciexyza);


// @RCalcUnit
// Requires: "modules/colorm/colorm.h"
UNIT_FROM_BASE(Color4Comp, Vec4, sRGBA, rgba) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < 0.0 || value.g > 1.0 || value.b < 0.0 || value.b > 1.0 || value.a < 0.0 || value.a > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }

    using namespace colorm;
    XyzD65 src(value.r, value.g, value.b, value.a);
    Rgb rgb(src);

    return Ok(Vec4(rgb.red(), rgb.green(), rgb.blue(), rgb.alpha()));
}

// @RCalcUnit
// Requires: <cmath>
UNIT_TO_BASE(Color4Comp, Vec4, sRGBA, rgba) {
    if (value.r < 0.0 || value.r > 255.0 || value.g < 0.0 || value.g > 255.0 || value.b < 0.0 || value.b > 255.0 || value.a < 0.0 || value.a > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 255], [0, 255], [0, 255], [0, 1]])");
    }

    using namespace colorm;
    Rgb src(value.r, value.g, value.b, value.a);
    XyzD65 cie(src);

    return Ok(Vec4(cie.x(), cie.y(), cie.z(), cie.alpha()));
}


UNIT_FROM_BASE(Color4Comp, Vec4, HSLA, hsla) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < 0.0 || value.g > 1.0 || value.b < 0.0 || value.b > 1.0 || value.a < 0.0 || value.a > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }

    using namespace colorm;
    XyzD65 src(value.r, value.g, value.b, value.a);
    Hsl hsl(src);

    return Ok(Vec4(hsl.hue(), hsl.saturation(), hsl.lightness(), hsl.alpha()));
}

UNIT_TO_BASE(Color4Comp, Vec4, HSLA, hsla) {
    if (value.r < 0.0 || value.r > 360.0 || value.g < 0.0 || value.g > 100.0 || value.b < 0.0 || value.b > 100.0 || value.a < 0.0 || value.a > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 360], [0, 100], [0, 100], [0, 1]])");
    }

    using namespace colorm;
    Hsl src(value.r, value.g, value.b, value.a);
    XyzD65 cie(src);

    return Ok(Vec4(cie.x(), cie.y(), cie.z(), cie.alpha()));
}


UNIT_FROM_BASE(Color4Comp, Vec4, OKLabA, oklaba) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < 0.0 || value.g > 1.0 || value.b < 0.0 || value.b > 1.0 || value.a < 0.0 || value.a > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }

    using namespace colorm;
    XyzD65 src(value.r, value.g, value.b, value.a);
    Oklab lab(src);

    return Ok(Vec4(lab.lightness(), lab.a(), lab.b(), lab.alpha()));
}

UNIT_TO_BASE(Color4Comp, Vec4, OKLabA, oklaba) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < -0.4 || value.g > 0.4 || value.b < -0.4 || value.b > 0.4 || value.a < 0.0 || value.a > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 1], [-0.4, 0.4], [-0.4, 0.4], [0, 1]])");
    }

    using namespace colorm;
    Oklab src(value.r, value.g, value.b, value.a);
    XyzD65 cie(src);

    return Ok(Vec4(cie.x(), cie.y(), cie.z(), cie.alpha()));
}


// @RCalcUnit
// Requires: <cmath>
UNIT_FROM_BASE(Color4Comp, Vec4, OKLChA, oklcha) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < 0.0 || value.g > 1.0 || value.b < 0.0 || value.b > 1.0 || value.a < 0.0 || value.a > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [0, 1])");
    }

    using namespace colorm;
    XyzD65 src(value.r, value.g, value.b, value.a);
    Oklch lch(src);

    return Ok(Vec4(lch.lightness(), lch.chroma(), lch.hue(), lch.alpha()));
}

// @RCalcUnit
// Requires: <cmath>
UNIT_TO_BASE(Color4Comp, Vec4, OKLChA, oklcha) {
    if (value.r < 0.0 || value.r > 1.0 || value.g < -0.4 || value.g > 0.4 || value.b < -0.4 || value.b > 0.4 || value.a < 0.0 || value.a > 1.0) {
        return Err(ERR_INVALID_PARAM, "Given color is invalid (expected components in range [[0, 1], [-0.4, 0.4], [-0.4, 0.4], [0, 1]])");
    }

    using namespace colorm;
    Oklch src(value.r, value.g, value.b, value.a);
    XyzD65 cie(src);

    return Ok(Vec4(cie.x(), cie.y(), cie.z(), cie.alpha()));
}