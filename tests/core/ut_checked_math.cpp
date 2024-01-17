#include "snitch/snitch.hpp"
#include "core/checked_math.h"

#include <limits>

using namespace RCalc;


TEST_CASE("Non-overflowing returns Ints", "[core]") {
    REQUIRE(CheckedMath::add(5, 10).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::add(std::numeric_limits<Int>::max(), -5).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::add(std::numeric_limits<Int>::min(), 5).get_type() == TYPE_INT);

    REQUIRE(CheckedMath::sub(5, 10).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::sub(std::numeric_limits<Int>::max(), 5).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::sub(std::numeric_limits<Int>::min(), -5).get_type() == TYPE_INT);

    REQUIRE(CheckedMath::mul(5, 10).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::mul(std::numeric_limits<Int>::max(), 1).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::mul(std::numeric_limits<Int>::min(), 1).get_type() == TYPE_INT);

    REQUIRE(CheckedMath::abs(5).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::abs(-5).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::abs(std::numeric_limits<Int>::max()).get_type() == TYPE_INT);

    REQUIRE(CheckedMath::neg(5).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::neg(-5).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::neg(std::numeric_limits<Int>::max()).get_type() == TYPE_INT);

    REQUIRE(CheckedMath::pow(5, 10).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::pow(std::numeric_limits<Int>::max(), 1).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::pow(std::numeric_limits<Int>::min(), 1).get_type() == TYPE_INT);

    REQUIRE(CheckedMath::discretize(5.0).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::discretize(std::nextafter((Real)std::numeric_limits<Int>::min(), 0.0)).get_type() == TYPE_INT);
    REQUIRE(CheckedMath::discretize(std::nextafter((Real)std::numeric_limits<Int>::max(), 0.0)).get_type() == TYPE_INT);
}

TEST_CASE("Overflowing returns BigInts or Reals", "[core][allocates]") {
    REQUIRE(CheckedMath::add(std::numeric_limits<Int>::max(), 5).get_type() == TYPE_BIGINT);
    REQUIRE(CheckedMath::add(std::numeric_limits<Int>::min(), -5).get_type() == TYPE_BIGINT);

    REQUIRE(CheckedMath::sub(std::numeric_limits<Int>::max(), -5).get_type() == TYPE_BIGINT);
    REQUIRE(CheckedMath::sub(std::numeric_limits<Int>::min(), 5).get_type() == TYPE_BIGINT);

    REQUIRE(CheckedMath::mul(std::numeric_limits<Int>::max(), 5).get_type() == TYPE_BIGINT);
    REQUIRE(CheckedMath::mul(std::numeric_limits<Int>::min(), 5).get_type() == TYPE_BIGINT);

    REQUIRE(CheckedMath::abs(std::numeric_limits<Int>::min()).get_type() == TYPE_BIGINT);

    REQUIRE(CheckedMath::neg(std::numeric_limits<Int>::min()).get_type() == TYPE_BIGINT);

    REQUIRE(CheckedMath::pow(std::numeric_limits<Int>::max(), -5).get_type() == TYPE_REAL);
    REQUIRE(CheckedMath::pow(std::numeric_limits<Int>::max(), 6).get_type() == TYPE_BIGINT);
    REQUIRE(CheckedMath::pow(std::numeric_limits<Int>::min(), 5).get_type() == TYPE_BIGINT);
    REQUIRE(CheckedMath::pow(4294967296, 2).get_type() == TYPE_BIGINT);
    REQUIRE(CheckedMath::pow(2642246, 3).get_type() == TYPE_BIGINT);

    REQUIRE(CheckedMath::discretize(std::nextafter((Real)std::numeric_limits<Int>::min(), -INFINITY)).get_type() == TYPE_BIGINT);
    REQUIRE(CheckedMath::discretize(std::nextafter((Real)std::numeric_limits<Int>::max(), INFINITY)).get_type() == TYPE_BIGINT);
}