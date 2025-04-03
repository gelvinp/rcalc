#include "snitch/snitch.hpp"
#include "app/variable_map.h"

TEST_CASE("Variables tests", "[app][allocates]") {
    using namespace RCalc;
    VariableMap map;

    map.store("one", Value { (Int)1 });
    map.store("two", Value { (Int)2 });

    REQUIRE(map.load("one") == Value { (Int)1 });
    REQUIRE(map.load("two") == Value { (Int)2 });

    {
        const CowVec<std::pair<std::string, Value>>& values = map.get_values();
        REQUIRE(values.size() == 2);
        REQUIRE(values[0].first == "one");
        REQUIRE(values[0].second == Value { (Int)1 });
        REQUIRE(values[1].first == "two");
        REQUIRE(values[1].second == Value { (Int)2 });
    }

    map.store("one", Value { (Int)111 });

    REQUIRE(map.load("one") == Value { (Int)111 });
    REQUIRE(map.load("two") == Value { (Int)2 });

    {
        const CowVec<std::pair<std::string, Value>>& values = map.get_values();
        REQUIRE(values.size() == 2);
        REQUIRE(values[0].first == "one");
        REQUIRE(values[0].second == Value { (Int)111 });
        REQUIRE(values[1].first == "two");
        REQUIRE(values[1].second == Value { (Int)2 });
    }

    map.remove("one");

    REQUIRE(map.load("one") == std::nullopt);
    REQUIRE(map.load("two") == Value { (Int)2 });

    {
        const CowVec<std::pair<std::string, Value>>& values = map.get_values();
        REQUIRE(values.size() == 1);
        REQUIRE(values[0].first == "two");
        REQUIRE(values[0].second == Value { (Int)2 });
    }

    map.clear();

    REQUIRE(map.load("one") == std::nullopt);
    REQUIRE(map.load("two") == std::nullopt);

    {
        const CowVec<std::pair<std::string, Value>>& values = map.get_values();
        REQUIRE(values.size() == 0);
    }
}
