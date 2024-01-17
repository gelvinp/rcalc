#include "snitch/snitch.hpp"
#include "core/filter.h"

TEST_CASE("Valid strings are unmodified", "[core][allocates]") {
    std::string str = "this_is_a_valid_string_123";

    REQUIRE(RCalc::filter_name(str.data()) == str);
}

TEST_CASE("Invalid strings are filtered", "[core][allocates]") {
    std::string str = "This is 🙅🏻‍♂️ NOT 🙅🏻‍♂️ a 𝓋𝒶𝓁𝒾𝒹 🌟 String!!!!!! 123 4";

    REQUIRE(RCalc::filter_name(str.data()) == "thisisnotastring1234");
}
