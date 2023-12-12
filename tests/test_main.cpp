#include "test_main.h"

#ifdef TESTS_ENABLED

#include "snitch/snitch.hpp"

namespace RCalc {

int test_main(TestConfig config) {
    const char* snitch_argv[12] = {};

    snitch_argv[0] = "rcalc";
    int snitch_argc = 1;

    #define ADD_ARG(arg) snitch_argv[snitch_argc++] = arg

    if (config.list_tests) {
        if (config.list_test_tag.has_value()) {
            ADD_ARG("--list-tests-with-tag");
            ADD_ARG(config.list_test_tag.value());
        }
        else {
            ADD_ARG("--list-tests");
        }
    }
    else if (config.list_tags) {
        ADD_ARG("--list-tags");
    }

    if (config.monochrome) {
        ADD_ARG("--color");
        ADD_ARG("never");
    }

    for (size_t index = 0; index < config.filter_count; ++index) {
        ADD_ARG(config.filters[index].data());
    }

    snitch::cli::input args = snitch::cli::parse_arguments(snitch_argc, snitch_argv).value();
    snitch::tests.configure(args);

    return snitch::tests.run_tests(args) ? 0 : 255;
}

}

#endif