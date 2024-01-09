#pragma once

#include "core/value.h"

#include <string>

namespace RCalc {

constexpr const int RealFmtContainerSize =
        std::numeric_limits<Real>::max_digits10 + 7;
using RealFmtContainer = std::span<char, RealFmtContainerSize>;
void fmt_real(Real value, int precision, RealFmtContainer buf);

std::string fmt(const char* p_format, ...);

std::string fmt_col_vec2(Vec2 value, int precision);
std::string fmt_col_vec3(Vec3 value, int precision);
std::string fmt_col_vec4(Vec4 value, int precision);

std::string fmt_mat2(Mat2 value, int precision, bool one_line = false);
std::string fmt_mat3(Mat3 value, int precision, bool one_line = false);
std::string fmt_mat4(Mat4 value, int precision, bool one_line = false);

}