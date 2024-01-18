#pragma once

#include <string>
#include <optional>

#ifdef TESTS_ENABLED
#include "tests/test_config.h"
#endif


namespace RCalc {

struct AppConfig {
    bool quiet = false;
    bool verbose = false;
    std::string renderer_name;
    std::optional<std::string> logfile_path;

    #ifdef TESTS_ENABLED
    std::optional<TestConfig> test_config;
    #endif
};

}