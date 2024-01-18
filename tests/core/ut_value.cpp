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

TEST_CASE("Parses Ints correctly", "[core][allocates]") {
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

TEST_CASE("Parses BigInts correctly", "[core][allocates]") {
    REQUIRE(Value::parse("18446744073709551615").value().operator BigInt() == BigInt("18446744073709551615"));
    REQUIRE(Value::parse("n18446744073709551615").value().operator BigInt() == BigInt("-18446744073709551615"));
    REQUIRE(Value::parse("0b1111111111111111111111111111111111111111111111111111111111111111").value().operator BigInt() == BigInt("18446744073709551615"));
    REQUIRE(Value::parse("0o1777777777777777777777").value().operator BigInt() == BigInt("18446744073709551615"));
    REQUIRE(Value::parse("0xFFFFFFFFFFFFFFFF").value().operator BigInt() == BigInt("18446744073709551615"));
    REQUIRE(Value::parse("9223372036854775808").value().operator BigInt() == BigInt("9223372036854775808"));
    REQUIRE(Value::parse("n9223372036854775809").value().operator BigInt() == BigInt("-9223372036854775809"));
    REQUIRE(Value::parse("0o12345670765432101234567").value().operator BigInt() == BigInt("96374533217262582135"));
    REQUIRE(Value::parse("0xfedcba9876543210").value().operator BigInt() == BigInt("18364758544493064720"));
}

TEST_CASE("Parses Reals correctly", "[core][allocates]") {
    REQUIRE(compare(Value::parse("1e6").value().operator Real(), 1000000.0));
    REQUIRE(compare(Value::parse("1en6").value().operator Real(), 0.000001));
    REQUIRE(compare(Value::parse("123.456").value().operator Real(), 123.456));
    REQUIRE(compare(Value::parse("0.123000000456").value().operator Real(), 0.123000000456));
}

TEST_CASE("Parses Vecs correctly", "[core][allocates]") {
    REQUIRE(compare(Value::parse("[123456789, n987654321]").value().operator Vec2(), Vec2 { 123456789.0, -987654321.0 }));
    REQUIRE(compare(Value::parse("[123456789, n987654321, 0.123456789]").value().operator Vec3(), Vec3 { 123456789.0, -987654321.0, 0.123456789 }));
    REQUIRE(compare(Value::parse("[123456789, n987654321, 0.123456789, n18446744073709551615]").value().operator Vec4(), Vec4 { 123456789.0, -987654321.0, 0.123456789, -18446744073709551615 }));
}

TEST_CASE("Parses Mats correctly", "[core][allocates]") {
    REQUIRE(compare(Value::parse("{[1, 2], [3, 4]}").value().operator Mat2(), Mat2 { { 1.0, 3.0 }, { 2.0, 4.0 } }));
    REQUIRE(compare(Value::parse("{[1, 2, 3], [4, 5, 6], [7, 8, 9]}").value().operator Mat3(), Mat3 { { 1.0, 4.0, 7.0 }, { 2.0, 5.0, 8.0 }, { 3.0, 6.0, 9.0 } }));
    REQUIRE(compare(Value::parse("{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}").value().operator Mat4(), Mat4 { { 1.0, 5.0, 9.0, 13.0 }, { 2.0, 6.0, 10.0, 14.0 }, { 3.0, 7.0, 11.0, 15.0 }, { 4.0, 8.0, 12.0, 16.0 } }));
}

TEST_CASE("Parses Units correctly", "[core][allocates]") {
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

TEST_CASE("Does not parse non-values", "[core][allocates]") {
    REQUIRE_FALSE(Value::parse("Not a value!").has_value());
    REQUIRE_FALSE(Value::parse("add").has_value());
    REQUIRE_FALSE(Value::parse("\\clear").has_value());
    REQUIRE_FALSE(Value::parse("123 456").has_value());
    REQUIRE_FALSE(Value::parse("123.456 789").has_value());
    REQUIRE_FALSE(Value::parse("123.456.789").has_value());
    REQUIRE_FALSE(Value::parse("0b123").has_value());
    REQUIRE_FALSE(Value::parse("0o128").has_value());
    REQUIRE_FALSE(Value::parse("0x1FG").has_value());
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

TEST_CASE("to_string", "[core][allocates]") {
    REQUIRE(Value::parse("1234")->to_string() == "1234");
    REQUIRE(Value::parse("n1234")->to_string() == "-1234");
    REQUIRE(Value::parse("0b1010")->to_string() == "0b1010");
    REQUIRE(Value::parse("0b0001")->to_string() == "0b1");
    REQUIRE(Value::parse("0o1234")->to_string() == "0o1234");
    REQUIRE(Value::parse("0x1234")->to_string() == "0x1234");

    REQUIRE(Value::parse("18446744073709551615")->to_string() == "18446744073709551615");
    REQUIRE(Value::parse("0b1111111111111111111111111111111111111111111111111111111111111111")->to_string() == "18446744073709551615");
    REQUIRE(Value::parse("0o1777777777777777777777")->to_string() == "18446744073709551615");
    REQUIRE(Value::parse("0xFFFFFFFFFFFFFFFF")->to_string() == "18446744073709551615");

    int old_precision = Value::get_precision();
    Value::set_precision(4);
    REQUIRE(Value::parse("12.34")->to_string() == "12.34");
    REQUIRE(Value::parse("0.123")->to_string() == "0.123");
    REQUIRE(Value::parse("0.1234")->to_string() == "0.1234");
    REQUIRE(Value::parse("0.12344")->to_string() == "0.1234");
    REQUIRE(Value::parse("0.12345")->to_string() == "0.1235");
    REQUIRE(Value::parse("0.0001")->to_string() == "0.0001");
    REQUIRE(Value::parse("0.00001")->to_string() == "1e-05");
    REQUIRE(Value::parse("1000.5")->to_string() == "1000");
    REQUIRE(Value::parse("1000.6")->to_string() == "1001");
    REQUIRE(Value::parse("10000.5")->to_string() == "1e+04");
    Value::set_precision(old_precision);

    REQUIRE(Value::parse("[1, 2]")->to_string() == "[1, 2]");
    REQUIRE(Value::parse("[123, 4]")->to_string() == "[123, 4]");
    REQUIRE(Value::parse("[123, 4]")->to_string(DisplayableTag::COL_VEC) == "| 123 |\n|   4 |");

    REQUIRE(Value::parse("[1, 2, 3]")->to_string() == "[1, 2, 3]");
    REQUIRE(Value::parse("[123, 4, 56]")->to_string() == "[123, 4, 56]");
    REQUIRE(Value::parse("[123, 4, 56]")->to_string(DisplayableTag::COL_VEC) == "| 123 |\n|   4 |\n|  56 |");

    REQUIRE(Value::parse("[1, 2, 3, 4]")->to_string() == "[1, 2, 3, 4]");
    REQUIRE(Value::parse("[123, 4, 56, 7890]")->to_string() == "[123, 4, 56, 7890]");
    REQUIRE(Value::parse("[123, 4, 56, 7890]")->to_string(DisplayableTag::COL_VEC) == "|  123 |\n|    4 |\n|   56 |\n| 7890 |");

    REQUIRE(Value::parse("{[1, 2], [3, 4]}")->to_string() == "| 1, 2 |\n| 3, 4 |");
    REQUIRE(Value::parse("{[1, 2], [3, 4]}")->to_string(DisplayableTag::ONE_LINE) == "{[1, 2], [3, 4]}");
    REQUIRE(Value::parse("{[123, 4], [5, 678]}")->to_string() == "| 123,   4 |\n|   5, 678 |");
    REQUIRE(Value::parse("{[123, 4], [5, 678]}")->to_string(DisplayableTag::ONE_LINE) == "{[123, 4], [5, 678]}");

    REQUIRE(Value::parse("{[1, 2, 3], [4, 5, 6], [7, 8, 9]}")->to_string() == "| 1, 2, 3 |\n| 4, 5, 6 |\n| 7, 8, 9 |");
    REQUIRE(Value::parse("{[1, 2, 3], [4, 5, 6], [7, 8, 9]}")->to_string(DisplayableTag::ONE_LINE) == "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}");
    REQUIRE(Value::parse("{[123, 4, 56], [2, 34, 567], [34, 567, 8]}")->to_string() == "| 123,   4,  56 |\n|   2,  34, 567 |\n|  34, 567,   8 |");
    REQUIRE(Value::parse("{[123, 4, 56], [2, 34, 567], [34, 567, 8]}")->to_string(DisplayableTag::ONE_LINE) == "{[123, 4, 56], [2, 34, 567], [34, 567, 8]}");

    REQUIRE(Value::parse("{[1, 2, 3, 4], [2, 3, 4, 5], [3, 4, 5, 6], [4, 5, 6, 7]}")->to_string() == "| 1, 2, 3, 4 |\n| 2, 3, 4, 5 |\n| 3, 4, 5, 6 |\n| 4, 5, 6, 7 |");
    REQUIRE(Value::parse("{[1, 2, 3, 4], [2, 3, 4, 5], [3, 4, 5, 6], [4, 5, 6, 7]}")->to_string(DisplayableTag::ONE_LINE) == "{[1, 2, 3, 4], [2, 3, 4, 5], [3, 4, 5, 6], [4, 5, 6, 7]}");
    REQUIRE(Value::parse("{[1, 23, 456, 7890], [12, 345, 6789, 0], [123, 4567, 8, 90], [1234, 5, 67, 890]}")->to_string() == "|    1,   23,  456, 7890 |\n|   12,  345, 6789,    0 |\n|  123, 4567,    8,   90 |\n| 1234,    5,   67,  890 |");
    REQUIRE(Value::parse("{[1, 23, 456, 7890], [12, 345, 6789, 0], [123, 4567, 8, 90], [1234, 5, 67, 890]}")->to_string(DisplayableTag::ONE_LINE) == "{[1, 23, 456, 7890], [12, 345, 6789, 0], [123, 4567, 8, 90], [1234, 5, 67, 890]}");

    {
        Value val = *Value::parse("[123, 234]");
        val.repr = REPR_HEX;
        REQUIRE(val.to_string() == "[0x7b, 0xea]");
    }
    {
        Value val = *Value::parse("[123, 234, 345]");
        val.repr = REPR_HEX;
        REQUIRE(val.to_string() == "[0x7b, 0xea, 0x159]");
    }
    {
        Value val = *Value::parse("[123, 234, 345, 456]");
        val.repr = REPR_HEX;
        REQUIRE(val.to_string() == "[0x7b, 0xea, 0x159, 0x1c8]");
    }
    REQUIRE(Value::parse("[0b1, 0o2, 0x3, 4]")->to_string() == "[1, 2, 3, 4]");

    UnitsMap& map = UnitsMap::get_units_map();
    map.build();

    for (UnitFamily const * const p_family : map.get_alphabetical()) {
        CAPTURE(p_family->p_name);
        for (Unit* p_unit : p_family->units) {
            CAPTURE(p_unit->p_name);
            REQUIRE(Value::parse(p_unit->p_usage)->to_string() == p_unit->p_name);
        }
    }
}

namespace RCalc {

class Pool_BigInt {
public:
    static uint64_t allocate(BigInt value);
    static uint64_t get(uint64_t index);
    static void ref(uint64_t index);
    static void unref(uint64_t index);

    static std::vector<BigInt> _pool;
    static std::vector<uint8_t> _refs;
};

TEST_CASE("Reference counting", "[core][allocates]") {
    Value a { BigInt("1234") };
    uint64_t idx_a = a.TEST_GET_DATA();
    
    REQUIRE(Pool_BigInt::_pool[idx_a] == BigInt("1234"));
    REQUIRE(Pool_BigInt::_refs[idx_a] == 1);

    Value b { a };
    uint64_t idx_b = b.TEST_GET_DATA();

    REQUIRE(idx_a == idx_b);
    REQUIRE(Pool_BigInt::_refs[idx_a] == 2);

    Value c { std::move(b) };
    uint64_t idx_c = c.TEST_GET_DATA();

    REQUIRE(idx_a == idx_c);
    REQUIRE(Pool_BigInt::_refs[idx_a] == 2);
    REQUIRE(b == Value());

    c = Value();
    REQUIRE(Pool_BigInt::_refs[idx_a] == 1);
    REQUIRE(c == Value());

    Value d { BigInt("5678") };
    uint64_t idx_d = d.TEST_GET_DATA();

    REQUIRE(idx_a != idx_d);
    REQUIRE(Pool_BigInt::_pool[idx_a] == BigInt("1234"));
    REQUIRE(Pool_BigInt::_pool[idx_d] == BigInt("5678"));
    REQUIRE(Pool_BigInt::_refs[idx_a] == 1);
    REQUIRE(Pool_BigInt::_refs[idx_d] == 1);

    a = d;

    REQUIRE(a.TEST_GET_DATA() == idx_d);
    REQUIRE(Pool_BigInt::_pool[idx_d] == BigInt("5678"));
    REQUIRE(Pool_BigInt::_refs[idx_d] == 2);
}

TEST_CASE("is_ functions", "[core][allocates]") {
    SECTION("Int") {
        Value val { (Int)123 };
        REQUIRE_FALSE(val.is_vec());
        REQUIRE_FALSE(val.is_mat());
        REQUIRE_FALSE(val.is_negative());
        REQUIRE(compare(val.get_real(), 123.0));

        val = Value((Int)-123);
        REQUIRE_FALSE(val.is_vec());
        REQUIRE_FALSE(val.is_mat());
        REQUIRE(val.is_negative());
        REQUIRE(compare(val.get_real(), -123.0));
    }

    SECTION("BigInt") {
        Value val { BigInt("123") };
        REQUIRE_FALSE(val.is_vec());
        REQUIRE_FALSE(val.is_mat());
        REQUIRE_FALSE(val.is_negative());
        REQUIRE(compare(val.get_real(), 123.0));

        val = Value(BigInt("-123"));
        REQUIRE_FALSE(val.is_vec());
        REQUIRE_FALSE(val.is_mat());
        REQUIRE(val.is_negative());
        REQUIRE(compare(val.get_real(), -123.0));
    }

    SECTION("Real") {
        Value val { (Real)123.0 };
        REQUIRE_FALSE(val.is_vec());
        REQUIRE_FALSE(val.is_mat());
        REQUIRE_FALSE(val.is_negative());
        REQUIRE(compare(val.get_real(), 123.0));

        val = Value((Real)-123.0);
        REQUIRE_FALSE(val.is_vec());
        REQUIRE_FALSE(val.is_mat());
        REQUIRE(val.is_negative());
        REQUIRE(compare(val.get_real(), -123.0));
    }

    SECTION("Vec2") {
        Value val { Vec2(1.0) };
        REQUIRE(val.is_vec());
        REQUIRE_FALSE(val.is_mat());
        REQUIRE_FALSE(val.is_negative());
        REQUIRE_THROWS_AS(val.get_real(), std::logic_error);
    }

    SECTION("Vec3") {
        Value val { Vec3(1.0) };
        REQUIRE(val.is_vec());
        REQUIRE_FALSE(val.is_mat());
        REQUIRE_FALSE(val.is_negative());
        REQUIRE_THROWS_AS(val.get_real(), std::logic_error);
    }

    SECTION("Vec4") {
        Value val { Vec4(1.0) };
        REQUIRE(val.is_vec());
        REQUIRE_FALSE(val.is_mat());
        REQUIRE_FALSE(val.is_negative());
        REQUIRE_THROWS_AS(val.get_real(), std::logic_error);
    }

    SECTION("Mat2") {
        Value val { Mat2(1.0) };
        REQUIRE_FALSE(val.is_vec());
        REQUIRE(val.is_mat());
        REQUIRE_FALSE(val.is_negative());
        REQUIRE_THROWS_AS(val.get_real(), std::logic_error);
    }

    SECTION("Mat3") {
        Value val { Mat3(1.0) };
        REQUIRE_FALSE(val.is_vec());
        REQUIRE(val.is_mat());
        REQUIRE_FALSE(val.is_negative());
        REQUIRE_THROWS_AS(val.get_real(), std::logic_error);
    }

    SECTION("Mat4") {
        Value val { Mat4(1.0) };
        REQUIRE_FALSE(val.is_vec());
        REQUIRE(val.is_mat());
        REQUIRE_FALSE(val.is_negative());
        REQUIRE_THROWS_AS(val.get_real(), std::logic_error);
    }

    SECTION("Unit") {
        Value val  = *Value::parse("_ft");
        REQUIRE_FALSE(val.is_vec());
        REQUIRE_FALSE(val.is_mat());
        REQUIRE_FALSE(val.is_negative());
        REQUIRE_THROWS_AS(val.get_real(), std::logic_error);
    }
}

}