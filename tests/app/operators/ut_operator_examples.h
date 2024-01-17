#pragma once

#include <span>
#include <initializer_list>
#include "core/memory/cowvec.h"


namespace {

struct OpTestCase {
    RCalc::CowVec<const char *> params;
    const char * result; // When calling to_string with ONE_LINE tag
};

struct OpTest {
    const char * op_name;
    RCalc::CowVec<OpTestCase> test_cases;

    OpTest(const char * op_name) : op_name(op_name) {}
    OpTest& operator=(const OpTest& other) { op_name = other.op_name; test_cases = other.test_cases; return *this; }
};

// EXPECTS: Precision set to 8
static RCalc::CowVec<OpTest> get_op_tests() {
    RCalc::CowVec<OpTest> op_tests;
    auto add_op_test = [&op_tests](const char * op_name, std::initializer_list<std::pair<std::initializer_list<const char *>, const char *>> test_cases) {
        OpTest test { op_name };
        test.test_cases.reserve(test_cases.size());

        for (auto [params, result] : test_cases) {
            RCalc::CowVec<const char *> example_params;
            example_params.reserve(params.size());

            for (const char* param : params) {
                example_params.push_back(param);
            }

            test.test_cases.emplace_back(example_params, result);
        }

        op_tests.push_back(test);
    };

    add_op_test(
        "Add",
        {
            {{ "123", "456" }, "579" },
            {{ "12.3", "45.6" }, "57.9" },
            {{ "18446744073709551615", "123" }, "18446744073709551738" }
        }
    );
    add_op_test(
        "Sub",
        {
            {{ "123", "456" }, "-333" },
            {{ "12.3", "45.6" }, "-33.3" },
            {{ "18446744073709551615", "123" }, "18446744073709551492" }
        }
    );
    add_op_test(
        "Mul",
        {
            {{ "123", "456" }, "56088" },
            {{ "12.3", "45.6" }, "560.88" },
            {{ "18446744073709551615", "123" }, "2268949521066274848645" }
        }
    );
    add_op_test(
        "Div",
        {
            {{ "123", "456" }, "2.6973684e-01" },
            {{ "18446744073709551615", "123" }, "1.4997353e+17" }
        }
    );
    add_op_test(
        "Abs",
        {
            {{ "123" }, "123" },
            {{ "-123" }, "123" },
            {{ "12.3" }, "12.3" },
            {{ "-12.3" }, "12.3" },
            {{ "18446744073709551615" }, "18446744073709551615" },
            {{ "-18446744073709551615" }, "18446744073709551615" }
        }
    );
    add_op_test(
        "Neg",
        {
            {{ "123" }, "-123" },
            {{ "-123" }, "123" },
            {{ "12.3" }, "-12.3" },
            {{ "-12.3" }, "12.3" },
            {{ "18446744073709551615" }, "-18446744073709551615" },
            {{ "-18446744073709551615" }, "18446744073709551615" }
        }
    );

    return op_tests;
}

}
