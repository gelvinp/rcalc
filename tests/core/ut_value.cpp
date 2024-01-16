#include "snitch/snitch.hpp"
#include "core/value.h"
#include "core/comparison.h"

#include <cmath>
#include <cfloat>

using namespace RCalc;
using namespace RCalc::TypeComparison;


TEST_CASE("Value is constructed correctly", "[core][allocates]") {
    SECTION("Default initialized") {
        Value value;
        REQUIRE(value.get_type() == TYPE_INT);
        REQUIRE(value.get_type_name() == Value::get_type_name(TYPE_INT));
        REQUIRE(value.operator Int() == 0);
    }

#define TypeTest(type, type_id, source_values...) \
    SECTION(#type) { \
        type source { source_values }; \
        Value value { source }; \
        REQUIRE(value.get_type() == type_id); \
        REQUIRE(value.get_type_name() == Value::get_type_name(type_id)); \
        REQUIRE(value.operator type() == source); \
    }

    TypeTest(Int, TYPE_INT, 42);
    TypeTest(BigInt, TYPE_BIGINT, "1234567898765432123456789");
    TypeTest(Real, TYPE_REAL, 12.34);
    TypeTest(Vec2, TYPE_VEC2, 1.1, 2.2);
    TypeTest(Vec3, TYPE_VEC3, 1.1, 2.2, 3.3);
    TypeTest(Vec4, TYPE_VEC4, 1.1, 2.2, 3.3, 4.4);
    TypeTest(Mat2, TYPE_MAT2, 1.1, 2.2, 3.3, 4.4);
    TypeTest(Mat3, TYPE_MAT3, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9);
    TypeTest(Mat4, TYPE_MAT4, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15, 16.16);
    TypeTest(Unit, TYPE_UNIT, *UnitsMap::get_units_map().find_unit("_ft").value());

#undef TypeTest
}

TEST_CASE("Small enough Reals are rounded to zero", "[core][allocates]") {
    Value value { (Real)1e-16 };
    REQUIRE(value.operator Real() == 0.0);
}

TEST_CASE("Parses Ints correctly", "[core]") {
    REQUIRE(Value::parse("0").value().operator Int() == 0);
    REQUIRE(Value::parse("123456789").value().operator Int() == 123456789);
    REQUIRE(Value::parse("-123456789").value().operator Int() == -123456789);
    REQUIRE(Value::parse("n123456789").value().operator Int() == -123456789);
    REQUIRE(Value::parse("0b101010").value().operator Int() == 0b101010);
    REQUIRE(Value::parse("0o1234567").value().operator Int() == 342391);
    REQUIRE(Value::parse("0xABCDEF").value().operator Int() == 0xABCDEF);
    REQUIRE(Value::parse("0x123456789").value().operator Int() == 0x123456789);
    REQUIRE(Value::parse("9223372036854775807").value().operator Int() == std::numeric_limits<Int>::max());
    REQUIRE(Value::parse("n9223372036854775808").value().operator Int() == std::numeric_limits<Int>::min());
}

TEST_CASE("Parses BigInts correctly", "[core]") {
    REQUIRE(Value::parse("18446744073709551615").value().operator BigInt() == BigInt("18446744073709551615"));
    REQUIRE(Value::parse("n18446744073709551615").value().operator BigInt() == BigInt("-18446744073709551615"));
    REQUIRE(Value::parse("0b1111111111111111111111111111111111111111111111111111111111111111").value().operator BigInt() == BigInt("18446744073709551615"));
    REQUIRE(Value::parse("0o1777777777777777777777").value().operator BigInt() == BigInt("18446744073709551615"));
    REQUIRE(Value::parse("0xFFFFFFFFFFFFFFFF").value().operator BigInt() == BigInt("18446744073709551615"));
    REQUIRE(Value::parse("9223372036854775808").value().operator BigInt() == BigInt("9223372036854775808"));
    REQUIRE(Value::parse("n9223372036854775809").value().operator BigInt() == BigInt("-9223372036854775809"));
}

TEST_CASE("Parses Reals correctly", "[core]") {
    REQUIRE(compare(Value::parse("1e6").value().operator Real(), 1000000.0));
    REQUIRE(compare(Value::parse("1en6").value().operator Real(), 0.000001));
    REQUIRE(compare(Value::parse("123.456").value().operator Real(), 123.456));
    REQUIRE(compare(Value::parse("0.123000000456").value().operator Real(), 0.123000000456));
}

TEST_CASE("Parses Vecs correctly", "[core]") {
    REQUIRE(compare(Value::parse("[123456789, n987654321]").value().operator Vec2(), Vec2 { 123456789.0, -987654321.0 }));
    REQUIRE(compare(Value::parse("[123456789, n987654321, 0.123456789]").value().operator Vec3(), Vec3 { 123456789.0, -987654321.0, 0.123456789 }));
    REQUIRE(compare(Value::parse("[123456789, n987654321, 0.123456789, n18446744073709551615]").value().operator Vec4(), Vec4 { 123456789.0, -987654321.0, 0.123456789, -18446744073709551615 }));
}

TEST_CASE("Parses Mats correctly", "[core]") {
    REQUIRE(compare(Value::parse("{[1, 2], [3, 4]}").value().operator Mat2(), Mat2 { { 1.0, 3.0 }, { 2.0, 4.0 } }));
    REQUIRE(compare(Value::parse("{[1, 2, 3], [4, 5, 6], [7, 8, 9]}").value().operator Mat3(), Mat3 { { 1.0, 4.0, 7.0 }, { 2.0, 5.0, 8.0 }, { 3.0, 6.0, 9.0 } }));
    REQUIRE(compare(Value::parse("{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}").value().operator Mat4(), Mat4 { { 1.0, 5.0, 9.0, 13.0 }, { 2.0, 6.0, 10.0, 14.0 }, { 3.0, 7.0, 11.0, 15.0 }, { 4.0, 8.0, 12.0, 16.0 } }));
}

TEST_CASE("Parses Units correctly", "[core]") {
    UnitsMap& map = UnitsMap::get_units_map();
    map.build();

    for (UnitFamily const * const p_family : map.get_alphabetical()) {
        CAPTURE(p_family->p_name);
        for (Unit* p_unit : p_family->units) {
            CAPTURE(p_unit->p_name);
            REQUIRE(Value::parse(p_unit->p_usage).value().operator Unit().p_impl == p_unit);
        }
    }
}

TEST_CASE("Does not parse non-values", "[core]") {
    REQUIRE_FALSE(Value::parse("Not a value!").has_value());
    REQUIRE_FALSE(Value::parse("add").has_value());
    REQUIRE_FALSE(Value::parse("\\clear").has_value());
    REQUIRE_FALSE(Value::parse("123 456").has_value());
    REQUIRE_FALSE(Value::parse("123.456 789").has_value());
    REQUIRE_FALSE(Value::parse("123.456.789").has_value());
    REQUIRE_FALSE(Value::parse("[not a value, 123456789]").has_value());
    REQUIRE_FALSE(Value::parse("[123456789, not a value]").has_value());
    REQUIRE_FALSE(Value::parse("[123456789, 987654321, not a value]").has_value());
    REQUIRE_FALSE(Value::parse("[]").has_value());
    REQUIRE_FALSE(Value::parse("[1]").has_value());
    REQUIRE_FALSE(Value::parse("[1, 2, 3, 4, 5]").has_value());
    REQUIRE_FALSE(Value::parse("{}").has_value());
    REQUIRE_FALSE(Value::parse("{[]}").has_value());
    REQUIRE_FALSE(Value::parse("{[1]}").has_value());
    REQUIRE_FALSE(Value::parse("{[1, 2], [3, 4, 5]}").has_value());
    REQUIRE_FALSE(Value::parse("{[1, 2], [3, 4], [5, 6]}").has_value());
    REQUIRE_FALSE(Value::parse("{[1, 2, 3, 4], [2, 3, 4, 5], [3, 4, 5, 6], [4, 5, 6]}").has_value());
    REQUIRE_FALSE(Value::parse("{[1, 2, 3, 4], [2, 3, 4, 5], [3, 4, 5, 6], [4, 5, 6, 7], [5, 6, 7, 8]}").has_value());
    REQUIRE_FALSE(Value::parse("_").has_value());
    REQUIRE_FALSE(Value::parse("_notarealunit").has_value());
}
