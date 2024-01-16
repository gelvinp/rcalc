#include "snitch/snitch.hpp"
#include "core/types.h"
#include <span>


TEST_CASE("Types are castable", "[core]") {
    using namespace RCalc;
    constexpr const Type ALL_TYPES[MAX_TYPE] = { TYPE_INT, TYPE_BIGINT, TYPE_REAL, TYPE_VEC2, TYPE_VEC3, TYPE_VEC4, TYPE_MAT2, TYPE_MAT3, TYPE_MAT4, TYPE_UNIT };
    const std::span<const Type> all_types { ALL_TYPES };

    SECTION("To themselves") {
        for (Type type : all_types) {
            REQUIRE(is_type_castable(type, type));
        }
    }

    SECTION("Non-scalars to nothing else") {
        for (int index = TYPE_VEC2; index < MAX_TYPE; ++index) {
            Type from_type = ALL_TYPES[index];
            for (Type to_type : all_types) {
                if (from_type == to_type) { continue; }
                REQUIRE_FALSE(is_type_castable(from_type, to_type));
            }
        }
    }

    SECTION("Scalars to larger types only") {
        for (int from_index = TYPE_INT; from_index <= TYPE_REAL; ++from_index) {
            Type from_type = ALL_TYPES[from_index];

            for (int smaller_index = TYPE_INT; smaller_index < from_index; ++smaller_index) {
                REQUIRE_FALSE(is_type_castable(from_type, ALL_TYPES[smaller_index]));
            }

            for (int larger_index = from_index + 1; larger_index <= TYPE_REAL; ++larger_index) {
                REQUIRE(is_type_castable(from_type, ALL_TYPES[larger_index]));
            }
        }
    }
}