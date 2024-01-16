#include "snitch/snitch.hpp"
#include "core/comparison.h"

using namespace RCalc;


TEST_CASE("Non-const T comparison", "[core][allocates]") {
    Type arr_a[3] = { TYPE_INT, TYPE_BIGINT, TYPE_REAL };
    Type arr_b[3] = { TYPE_VEC2, TYPE_VEC3, TYPE_VEC4 };
    Type arr_c[1] = { TYPE_UNIT };
    
    std::span<Type> span_a { arr_a };
    std::span<Type> span_b { arr_b };
    std::span<Type> span_c { arr_c };

    CowVec<Type> vec_a { TYPE_INT, TYPE_BIGINT, TYPE_REAL };
    CowVec<Type> vec_b { TYPE_VEC2, TYPE_VEC3, TYPE_VEC4 };
    CowVec<Type> vec_c { TYPE_UNIT };

    REQUIRE(span_a == vec_a);
    REQUIRE(span_a != vec_b);
    REQUIRE(span_a != vec_c);
    REQUIRE(span_b != vec_a);
    REQUIRE(span_b == vec_b);
    REQUIRE(span_b != vec_c);
    REQUIRE(span_c != vec_a);
    REQUIRE(span_c != vec_b);
    REQUIRE(span_c == vec_c);
}

TEST_CASE("Const T comparison", "[core][allocates]") {
    constexpr static const Type arr_a[3] = { TYPE_INT, TYPE_BIGINT, TYPE_REAL };
    constexpr static const Type arr_b[3] = { TYPE_VEC2, TYPE_VEC3, TYPE_VEC4 };
    constexpr static const Type arr_c[1] = { TYPE_UNIT };
    
    constexpr std::span<const Type> span_a { arr_a };
    constexpr std::span<const Type> span_b { arr_b };
    constexpr std::span<const Type> span_c { arr_c };

    CowVec<Type> vec_a { TYPE_INT, TYPE_BIGINT, TYPE_REAL };
    CowVec<Type> vec_b { TYPE_VEC2, TYPE_VEC3, TYPE_VEC4 };
    CowVec<Type> vec_c { TYPE_UNIT };

    REQUIRE(span_a == vec_a);
    REQUIRE(span_a != vec_b);
    REQUIRE(span_a != vec_c);
    REQUIRE(span_b != vec_a);
    REQUIRE(span_b == vec_b);
    REQUIRE(span_b != vec_c);
    REQUIRE(span_c != vec_a);
    REQUIRE(span_c != vec_b);
    REQUIRE(span_c == vec_c);
}