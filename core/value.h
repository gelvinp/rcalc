#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "modules/bigint/bigint.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace RCalc {

typedef int64_t Int;
typedef bigint BigInt;
typedef double Real;
typedef glm::dvec2 Vec2;
typedef glm::dvec3 Vec3;
typedef glm::dvec4 Vec4;

class Value {
public:
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
        MAX_TYPE
    };

    static std::optional<Value> parse(const std::string& str);

    static const char* get_type_name(Type type);

    Type get_type() const { return type; }
    const char* get_type_name() const;

    operator Int() const;
    operator BigInt() const;
    operator Real() const;
    operator Vec2() const;
    operator Vec3() const;
    operator Vec4() const;

    Value(Int value);
    Value(BigInt value);
    Value(Real value);
    Value(Vec2 value);
    Value(Vec3 value);
    Value(Vec4 value);

    static Value find_int(Real value, std::optional<const std::string*> source = std::nullopt);

    std::string to_string();

    Value() = default;
    ~Value();

    // Delete copy constructors, specify move
    Value(const Value&) = delete;
    Value& operator=(const Value&) = delete;

    Value(Value&& value);
    Value& operator=(Value&& value);

    // Explicit copy
    Value make_copy() const;

private:
    Type type = TYPE_INT;
    uint64_t data = 0;

    static Value parse_numeric(const std::string& str, Real value);
    static std::optional<Value> parse_vec(const std::string& str);
};

}