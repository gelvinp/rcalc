#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "types.h"
#include "core/units/units.h"
#include "display_tags.h"
#include "core/coredef.h"

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
    static std::optional<Value> parse(std::string_view str);

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
    operator Identifier() const;

    bool operator==(const Value& other) const;

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
    Value(Identifier value, Representation repr = REPR_NONE);
    
    std::string to_string(DisplayableTag tags = DisplayableTag::NONE, std::optional<int> precision = std::nullopt) const;

    bool is_vec() const;
    bool is_mat() const;

    bool is_negative() const;
    Real get_real() const;

    Value() = default;
    ~Value();

    Value(const Value&);
    Value& operator=(const Value&);

    Value(Value&& value);
    Value& operator=(Value&& value);

    Representation repr : 4 = REPR_NONE;

    static int get_precision() { return _precision; }
    static void set_precision(int precision);

#ifdef TESTS_ENABLED
    uint64_t TEST_GET_DATA() const { return data; }
#endif

private:
    Type type : 4 = TYPE_INT;
    uint64_t data = 0;

    static std::optional<Value> parse_scalar(std::string_view str);
    static std::optional<Value> parse_vec(std::string_view sv);
    static std::optional<Value> parse_mat(std::string_view sv);
    static std::optional<Value> parse_unit(std::string_view str);
    static std::optional<Value> parse_identifier(std::string_view str);

    static std::optional<std::string> str_oct_to_bin(std::string str);
    static std::optional<std::string> str_hex_to_bin(std::string str);
    static std::optional<std::string> str_bin_to_dec(std::string str);

    void ref();
    void unref();

    void swap(RCalc::Value& other) {
        std::swap(data, other.data);

        Representation s_repr = repr;
        repr = other.repr;
        other.repr = s_repr;

        Type s_type = type;
        type = other.type;
        other.type = s_type;
    }

    static int _precision;
    static std::string _precision_fmt;
};

}