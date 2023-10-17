#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "types.h"
#include "core/units/units.h"

struct Unit;

namespace RCalc {

class Value {
public:
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
    operator Mat2() const;
    operator Mat3() const;
    operator Mat4() const;
    operator Unit() const;

    Value(Int value);
    Value(BigInt value);
    Value(Real value);
    Value(Vec2 value);
    Value(Vec3 value);
    Value(Vec4 value);
    Value(Mat2 value);
    Value(Mat3 value);
    Value(Mat4 value);
    Value(Unit value);

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
    static std::optional<Real> parse_real(std::string_view sv);
    static std::optional<Value> parse_vec(std::string_view sv);
    static std::optional<Value> parse_mat(std::string_view sv);

    static std::optional<Value> parse_unit(const std::string& str);
};

}