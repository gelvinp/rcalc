#pragma once

#include "core/value.h"

#include <string>

namespace RCalc {

std::string fmt(const char* p_format, ...);

std::string fmt_mat2(Mat2 value);
std::string fmt_mat3(Mat3 value);
std::string fmt_mat4(Mat4 value);

}