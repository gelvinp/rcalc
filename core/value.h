#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "types.h"
#include "core/units/units.h"
#include "display_tags.h"

namespace RCalc {

struct Unit;

enum Representation: uint8_t {
    REPR_NONE,
    REPR_BINARY,
    REPR_OCT,
    REPR_DECIMAL,
    REPR_HEX
};

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

    Value(Int value, Representation repr = REPR_NONE);
    Value(BigInt value, Representation repr = REPR_NONE);
    Value(Real value, Representation repr = REPR_NONE);
    Value(Vec2 value, Representation repr = REPR_NONE);
    Value(Vec3 value, Representation repr = REPR_NONE);
    Value(Vec4 value, Representation repr = REPR_NONE);
    Value(Mat2 value, Representation repr = REPR_NONE);
    Value(Mat3 value, Representation repr = REPR_NONE);
    Value(Mat4 value, Representation repr = REPR_NONE);
    Value(Unit value, Representation repr = REPR_NONE);

    static Value find_int(Real value, std::optional<const std::string*> source = std::nullopt, Representation repr = REPR_NONE);

    std::string to_string(DisplayableTag tags = DisplayableTag::NONE) const;

    bool is_vec() const;
    bool is_mat() const;

    Value() = default;
    ~Value();

    Value(const Value&);
    Value& operator=(const Value&);

    Value(Value&& value);
    Value& operator=(Value&& value);

    Representation repr : 4 = REPR_NONE;

private:
    Type type : 4 = TYPE_INT;
    uint64_t data = 0;

    static Value parse_numeric(const std::string& str, Value&& value, Representation repr = REPR_NONE);
    static std::optional<Value> parse_real(std::string str);
    static std::optional<Value> parse_vec(std::string_view sv);
    static std::optional<Value> parse_mat(std::string_view sv);
    static std::optional<Value> parse_unit(const std::string& str);

    void ref();
    void unref();
};

}