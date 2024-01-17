#include "snitch/snitch.hpp"
#include "core/anycase_str.h"

TEST_CASE("Comparisons are case-insensitive", "[core][allocates]") {
    anycase_string a = "the quick brown fox jumps over the lazy dog!";
    anycase_string b = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG!";
    anycase_string c = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG$";

    REQUIRE(a == b);
    REQUIRE(a != c);
    REQUIRE(b != c);

    REQUIRE(c > a);
}
