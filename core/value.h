#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace RCalc {

typedef int64_t Int;
typedef double Double;

class Value {
public:
    enum Type: uint8_t {
        TYPE_INT,
        TYPE_BIGINT,
        TYPE_DOUBLE,
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

    operator int64_t() const;
    operator double() const;

    Value(int64_t value);
    Value(double value);

    ~Value();

    // Delete copy constructors, specify move
    Value(const Value&) = delete;
    Value& operator=(const Value&) = delete;

    Value(Value&& value);
    Value& operator=(Value&& value);

private:
    Type type = TYPE_INT;
    uint64_t data = 0;

    static Value parse_numeric(const std::string& str, double value);
};

}