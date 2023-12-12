#pragma once

#ifdef TESTS_ENABLED

#include <cstdint>
#include <optional>
#include <string_view>

namespace RCalc {

struct TestConfig {
    bool run_tests = false;

    bool list_tests = false;
    bool list_tags = false;
    std::optional<const char*> list_test_tag = {};

    std::optional<const char*> command_given = {};

    bool monochrome = false;

    std::string_view filters[8] = {};
    uint8_t filter_count = 0;
};

}

#endif