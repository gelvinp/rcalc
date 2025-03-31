#pragma once

#include "modules/bigint/bigint.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <string>

namespace RCalc {


typedef int64_t Int;
typedef bigint BigInt;
typedef double Real;
typedef glm::dvec2 Vec2;
typedef glm::dvec3 Vec3;
typedef glm::dvec4 Vec4;
typedef glm::dmat2 Mat2;
typedef glm::dmat3 Mat3;
typedef glm::dmat4 Mat4;
typedef std::string Identifier;

enum Type: uint8_t {
    TYPE_INT,
    TYPE_BIGINT,
    TYPE_REAL,
    TYPE_VEC2,
    TYPE_VEC3,
    TYPE_VEC4,
    TYPE_MAT2,
    TYPE_MAT3,
    TYPE_MAT4,
    TYPE_UNIT,
    TYPE_IDENTIFIER,
    MAX_TYPE
};


bool is_type_castable(Type from, Type to);

}