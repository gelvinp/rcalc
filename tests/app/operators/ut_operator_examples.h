#pragma once

#include <span>
#include <initializer_list>
#include "core/memory/cowvec.h"


namespace {

struct OpTestCase {
    RCalc::CowVec<const char *> params;
    const char * result; // When calling to_string with ONE_LINE tag

    OpTestCase(RCalc::CowVec<const char *> params, const char* result)
        : params(params), result(result) {}
};

struct OpErrTestCase {
    RCalc::CowVec<const char *> params;

    OpErrTestCase(RCalc::CowVec<const char *> params)
        : params(params) {}
};

struct OpTest {
    const char * op_name;
    RCalc::CowVec<OpTestCase> test_cases;

    OpTest(const char * op_name) : op_name(op_name) {}
    OpTest(const OpTest& other) : op_name(other.op_name), test_cases(other.test_cases) {}
    OpTest& operator=(const OpTest& other) { op_name = other.op_name; test_cases = other.test_cases; return *this; }
};

struct OpErrTest {
    const char * op_name;
    RCalc::CowVec<OpErrTestCase> err_test_cases;

    OpErrTest(const char * op_name) : op_name(op_name) {}
    OpErrTest(const OpErrTest& other) : op_name(other.op_name), err_test_cases(other.err_test_cases) {}
    OpErrTest& operator=(const OpErrTest& other) { op_name = other.op_name; err_test_cases = other.err_test_cases; return *this; }
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
            {{ "18446744073709551615", "123" }, "18446744073709551738" },
            {{ "[1, 2]", "[3, 4]" }, "[4, 6]" },
            {{ "[1, 2, 3]", "[4, 5, 6]" }, "[5, 7, 9]" },
            {{ "[1, 2, 3, 4]", "[5, 6, 7, 8]" }, "[6, 8, 10, 12]" },
            {{ "{[1, 2], [3, 4]}", "{[5, 6], [7, 8]}" }, "{[6, 8], [10, 12]}"},
            {{ "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}", "{[10, 11, 12], [13, 14, 15], [16, 17, 18]}" }, "{[11, 13, 15], [17, 19, 21], [23, 25, 27]}"},
            {{ "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}", "{[17, 18, 19, 20], [21, 22, 23, 24], [25, 26, 27, 28], [29, 30, 31, 32]}" }, "{[18, 20, 22, 24], [26, 28, 30, 32], [34, 36, 38, 40], [42, 44, 46, 48]}"}
        }
    );
    add_op_test(
        "Sub",
        {
            {{ "123", "456" }, "-333" },
            {{ "12.3", "45.6" }, "-33.3" },
            {{ "18446744073709551615", "123" }, "18446744073709551492" },
            {{ "[1, 2]", "[3, 4]" }, "[-2, -2]" },
            {{ "[1, 2, 3]", "[4, 5, 6]" }, "[-3, -3, -3]" },
            {{ "[1, 2, 3, 4]", "[5, 6, 7, 8]" }, "[-4, -4, -4, -4]" },
            {{ "{[1, 2], [3, 4]}", "{[5, 6], [7, 8]}" }, "{[-4, -4], [-4, -4]}"},
            {{ "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}", "{[10, 11, 12], [13, 14, 15], [16, 17, 18]}" }, "{[-9, -9, -9], [-9, -9, -9], [-9, -9, -9]}"},
            {{ "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}", "{[17, 18, 19, 20], [21, 22, 23, 24], [25, 26, 27, 28], [29, 30, 31, 32]}" }, "{[-16, -16, -16, -16], [-16, -16, -16, -16], [-16, -16, -16, -16], [-16, -16, -16, -16]}"}
        }
    );
    add_op_test(
        "Mul",
        {
            {{ "123", "456" }, "56088" },
            {{ "12.3", "45.6" }, "560.88" },
            {{ "18446744073709551615", "123" }, "2268949521066274848645" },
            {{ "[1, 2]", "3" }, "[3, 6]" },
            {{ "[1, 2, 3]", "4" }, "[4, 8, 12]" },
            {{ "[1, 2, 3, 4]", "5" }, "[5, 10, 15, 20]" },
            {{ "{[1, 2], [3, 4]}", "2" }, "{[2, 4], [6, 8]}"},
            {{ "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}", "2" }, "{[2, 4, 6], [8, 10, 12], [14, 16, 18]}"},
            {{ "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}", "2" }, "{[2, 4, 6, 8], [10, 12, 14, 16], [18, 20, 22, 24], [26, 28, 30, 32]}"},
            {{ "{[1, 2], [3, 4]}", "[1, 2]" }, "[5, 11]"},
            {{ "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}", "[1, 2, 3]" }, "[14, 32, 50]"},
            {{ "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}", "[1, 2, 3, 4]" }, "[30, 70, 110, 150]"},
            {{ "[1, 2]", "{[1, 2], [3, 4]}" }, "[7, 10]"},
            {{ "[1, 2, 3]", "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}" }, "[30, 36, 42]"},
            {{ "[1, 2, 3, 4]", "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}" }, "[90, 100, 110, 120]"},
            {{ "{[1, 2], [3, 4]}", "{[2, 3], [4, 5]}" }, "{[10, 13], [22, 29]}"},
            {{ "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}", "{[2, 3, 4], [5, 6, 7], [8, 9, 10]}" }, "{[36, 42, 48], [81, 96, 111], [126, 150, 174]}"},
            {{ "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}", "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}" }, "{[90, 100, 110, 120], [202, 228, 254, 280], [314, 356, 398, 440], [426, 484, 542, 600]}"}
        }
    );
    add_op_test(
        "Div",
        {
            {{ "123", "456" }, "0.26973684" },
            {{ "18446744073709551615", "123" }, "1.4997353e+17" },
            {{ "[1, 2]", "3" }, "[0.33333333, 0.66666667]" },
            {{ "[1, 2, 3]", "4" }, "[0.25, 0.5, 0.75]" },
            {{ "[1, 2, 3, 4]", "5" }, "[0.2, 0.4, 0.6, 0.8]" },
            {{ "{[1, 2], [3, 4]}", "2" }, "{[0.5, 1], [1.5, 2]}"},
            {{ "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}", "2" }, "{[0.5, 1, 1.5], [2, 2.5, 3], [3.5, 4, 4.5]}"},
            {{ "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}", "2" }, "{[0.5, 1, 1.5, 2], [2.5, 3, 3.5, 4], [4.5, 5, 5.5, 6], [6.5, 7, 7.5, 8]}"},
            {{ "16", "{[1, 2], [3, 4]}" }, "{[16, 8], [5.3333333, 4]}"},
            {{ "16", "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}" }, "{[16, 8, 5.3333333], [4, 3.2, 2.6666667], [2.2857143, 2, 1.7777778]}"},
            {{ "16", "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}" }, "{[16, 8, 5.3333333, 4], [3.2, 2.6666667, 2.2857143, 2], [1.7777778, 1.6, 1.4545455, 1.3333333], [1.2307692, 1.1428571, 1.0666667, 1]}"},
            {{ "{[1, 2], [3, 4]}", "[5, 6]" }, "[-4, 4.5]"},
            {{ "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "[10, 11, 12]" }, "[1.6, 2.2, 1.3333333]"},
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "[17, 18, 19, 20]" }, "[-16, -16, -16, 28.25]"},
            {{ "[5, 6]", "{[1, 2], [3, 4]}" }, "[-1, 2]"},
            {{ "[10, 11, 12]", "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}" }, "[10, -1.8, -2]"},
            {{ "[17, 18, 19, 20]", "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}" }, "[-36.3333333, 12, 4, 1.3333333]"},
            {{ "{[1, 2], [3, 4]}", "{[2, 3], [4, 5]}" }, "{[1.5, -0.5], [0.5, 0.5]}"},
            {{ "{[2, 3, 4], [5, 6, 7], [8, 9, 10]}", "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}" }, "{[2, -0.2, -0.22222222], [5, -0.8, -0.88888889], [8, -1.4, -1.5555556]}"},
            {{ "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}", "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}" }, "{[1, 0, 0, 0], [-8.3333333, 3, 1, 0.33333333], [-17.6666667, 6, 2, 0.66666667], [-27, 9, 3, 1]}"}
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
            {{ "-18446744073709551615" }, "18446744073709551615" },
            {{ "[1, 2]" }, "[-1, -2]" },
            {{ "[1, 2, 3]" }, "[-1, -2, -3]" },
            {{ "[1, 2, 3, 4]" }, "[-1, -2, -3, -4]" }
        }
    );
    add_op_test(
        "Pow",
        {
            {{ "12", "3" }, "1728" },
            {{ "25", "0.5" }, "5" },
            {{ "12.3", "4" }, "22888.664" },
            {{ "18446744073709551615", "3" }, "6277101735386680762814942322444851025767571854389858533375" }
        }
    );
    add_op_test(
        "Sqr",
        {
            {{ "12" }, "144" },
            {{ "12.3" }, "151.29" },
            {{ "18446744073709551615" }, "340282366920938463426481119284349108225" }
        }
    );
    add_op_test(
        "Cube",
        {
            {{ "12" }, "1728" },
            {{ "12.3" }, "1860.867" },
            {{ "18446744073709551615" }, "6277101735386680762814942322444851025767571854389858533375" }
        }
    );
    add_op_test(
        "Sqrt",
        {
            {{ "144" }, "12" },
            {{ "151.29" }, "12.3" },
            {{ "340282366920938463426481119284349108225" }, "1.8446744e+19" }
        }
    );
    add_op_test(
        "Cbrt",
        {
            {{ "1728" }, "12" },
            {{ "1860.867" }, "12.3" },
            {{ "6277101735386680762814942322444851025767571854389858533375" }, "1.8446744e+19" }
        }
    );
    add_op_test(
        "Log",
        {
            {{ "8", "123" }, "2.3141715" }
        }
    );
    add_op_test(
        "Ln",
        {
            {{ "123" }, "4.8121844" }
        }
    );
    add_op_test(
        "Exp",
        {
            {{ "4.81218435537" }, "123" }
        }
    );
    add_op_test(
        "Ceil",
        {
            {{ "1.4" }, "2" },
            {{ "1.5" }, "2" },
            {{ "1.6" }, "2" },
            {{ "n1.4" }, "-1" },
            {{ "n1.5" }, "-1" },
            {{ "n1.6" }, "-1" }
        }
    );
    add_op_test(
        "Floor",
        {
            {{ "1.4" }, "1" },
            {{ "1.5" }, "1" },
            {{ "1.6" }, "1" },
            {{ "n1.4" }, "-2" },
            {{ "n1.5" }, "-2" },
            {{ "n1.6" }, "-2" }
        }
    );
    add_op_test(
        "Trunc",
        {
            {{ "1.4" }, "1" },
            {{ "1.5" }, "1" },
            {{ "1.6" }, "1" },
            {{ "n1.4" }, "-1" },
            {{ "n1.5" }, "-1" },
            {{ "n1.6" }, "-1" }
        }
    );
    add_op_test(
        "Round",
        {
            {{ "1.4" }, "1" },
            {{ "1.5" }, "2" },
            {{ "1.6" }, "2" },
            {{ "n1.4" }, "-1" },
            {{ "n1.5" }, "-2" },
            {{ "n1.6" }, "-2" }
        }
    );
    add_op_test(
        "Frac",
        {
            {{ "1.4" }, "0.4" },
            {{ "1.5" }, "0.5" },
            {{ "1.6" }, "0.6" },
            {{ "n1.4" }, "-0.4" },
            {{ "n1.5" }, "-0.5" },
            {{ "n1.6" }, "-0.6" }
        }
    );
    add_op_test(
        "Sign",
        {
            {{ "123" }, "1" },
            {{ "n123" }, "-1" },
            {{ "0" }, "0" }
        }
    );
    add_op_test(
        "Fact",
        {
            {{ "0" }, "1" },
            {{ "3" }, "6" },
            {{ "10" }, "3628800" }
        }
    );
    add_op_test(
        "Gamma",
        {
            {{ "0.5" }, "1.7724539" },
            {{ "1" }, "1" },
            {{ "1.5" }, "0.88622693" },
            {{ "2" }, "1" },
            {{ "2.5" }, "1.3293404" },
            {{ "n0.5" }, "-3.5449077" },
            {{ "n1.5" }, "2.3632718" },
            {{ "n2.5" }, "-0.94530872" }
        }
    );
    add_op_test(
        "Quot",
        {
            {{ "123", "456" }, "0" },
            {{ "10", "3" }, "3" },
            {{ "12.3", "4.5" }, "2" },
            {{ "18446744073709551615", "123" }, "149973529054549200" }
        }
    );
    add_op_test(
        "Mod",
        {
            {{ "123", "456" }, "123" },
            {{ "10", "3" }, "1" },
            {{ "12.3", "4.5" }, "3.3" },
            {{ "18446744073709551615", "123" }, "15" }
        }
    );
    add_op_test(
        "e",
        {
            {{}, "2.7182818" }
        }
    );
    add_op_test(
        "Bin",
        {
            {{ "123" }, "0b1111011" },
            {{ "0o173" }, "0b1111011" },
            {{ "0x7b" }, "0b1111011" }
        }
    );
    add_op_test(
        "Oct",
        {
            {{ "0b1111011" }, "0o173" },
            {{ "123" }, "0o173" },
            {{ "0x7b" }, "0o173" }
        }
    );
    add_op_test(
        "Dec",
        {
            {{ "0b1111011" }, "123" },
            {{ "0o173" }, "123" },
            {{ "0x7b" }, "123" }
        }
    );
    add_op_test(
        "Hex",
        {
            {{ "0b1111011" }, "0x7b" },
            {{ "0o173" }, "0x7b" },
            {{ "123" }, "0x7b" },
            {{ "[123, 234]" }, "[0x7b, 0xea]"},
            {{ "[123, 234, 345]" }, "[0x7b, 0xea, 0x159]"},
            {{ "[123, 234, 345, 456]" }, "[0x7b, 0xea, 0x159, 0x1c8]"}
        }
    );
    add_op_test(
        "And",
        {
            {{ "0b1111011", "0b10101" }, "0b10001" }
        }
    );
    add_op_test(
        "Or",
        {
            {{ "0b1111011", "0b10101" }, "0b1111111" }
        }
    );
    add_op_test(
        "Xor",
        {
            {{ "0b1111011", "0b10101" }, "0b1101110" }
        }
    );
    add_op_test(
        "Not",
        {
            {{ "0b1111011" }, "0b100" },
            {{ "0b1011010" }, "0b100101" }
        }
    );
    add_op_test(
        "Sll",
        {
            {{ "0b1111011", "3" }, "0b1111011000" },
            {{ "0b1011010", "3" }, "0b1011010000" }
        }
    );
    add_op_test(
        "Srl",
        {
            {{ "0b1111011", "3" }, "0b1111" },
            {{ "0b1011010", "3" }, "0b1011" }
        }
    );
    add_op_test(
        "Pi",
        {
            {{}, "3.1415927" }
        }
    );
    add_op_test(
        "Tau",
        {
            {{}, "6.2831853" }
        }
    );
    add_op_test(
        "Sin",
        {
            {{ "0" }, "0" },
            {{ "1.5707963268" }, "1" },
            {{ "3.14159265359" }, "0.0" },
            {{ "4.71238898038" }, "-1" },
            {{ "6.28318530718" }, "0.0" },
            {{ "1.9198622" }, "0.93969261" }
        }
    );
    add_op_test(
        "Cos",
        {
            {{ "0" }, "1" },
            {{ "1.5707963268" }, "0.0" },
            {{ "3.14159265359" }, "-1" },
            {{ "4.71238898038" }, "0.0" },
            {{ "6.28318530718" }, "1" },
            {{ "1.9198622" }, "-0.34202016" }
        }
    );
    add_op_test(
        "Tan",
        {
            {{ "0.785398163398" }, "1" },
            {{ "2.35619449019" }, "-1" },
            {{ "3.92699081699" }, "1" },
            {{ "4.712389898038" }, "-1.0" },
            {{ "1.9198622" }, "-2.7474772" }
        }
    );
    add_op_test(
        "Asin",
        {
            {{ "0" }, "0" },
            {{ "1" }, "1.5707963268" },
            {{ "n1" }, "n1.5707963268" }
        }
    );
    add_op_test(
        "Acos",
        {
            {{ "0" }, "1.5707963268" },
            {{ "1" }, "0" },
            {{ "n1" }, "3.14159265359" }
        }
    );
    add_op_test(
        "Atan",
        {
            {{ "0" }, "0" },
            {{ "1" }, "0.78539816" },
            {{ "n1" }, "n0.78539816" }
        }
    );
    add_op_test(
        "Sinh",
        {
            {{ "0" }, "0" },
            {{ "1.5707963268" }, "2.3012989" },
            {{ "3.14159265359" }, "11.5487394" },
            {{ "n1.5707963268" }, "-2.3012989" },
            {{ "n3.14159265359" }, "-11.5487394" }
        }
    );
    add_op_test(
        "Cosh",
        {
            {{ "0" }, "1" },
            {{ "1.5707963268" }, "2.5091785" },
            {{ "3.14159265359" }, "11.5919533" },
            {{ "n1.5707963268" }, "2.5091785" },
            {{ "n3.14159265359" }, "11.5919533" }
        }
    );
    add_op_test(
        "Tanh",
        {
            {{ "0" }, "0" },
            {{ "1.5707963268" }, "0.91715234" },
            {{ "3.14159265359" }, "0.99627208" },
            {{ "n1.5707963268" }, "-0.91715234" },
            {{ "n3.14159265359" }, "-0.99627208" }
        }
    );
    add_op_test(
        "Asinh",
        {
            {{ "0" }, "0" },
            {{ "2.30129890232" }, "1.5707963" },
            {{ "11.5487393573" }, "3.1415927" },
            {{ "n2.30129890232" }, "-1.5707963" },
            {{ "n11.5487393573" }, "-3.1415927" }
        }
    );
    add_op_test(
        "Acosh",
        {
            {{ "1" }, "0" },
            {{ "2.50917847867" }, "1.5707963" },
            {{ "11.5919532755" }, "3.1415927" }
        }
    );
    add_op_test(
        "Atanh",
        {
            {{ "0" }, "0" },
            {{ "0.917152335668" }, "1.5707963" },
            {{ "0.996272076221" }, "3.1415927" },
            {{ "n0.917152335668" }, "-1.5707963" },
            {{ "n0.996272076221" }, "-3.1415927" }
        }
    );
    add_op_test(
        "Min",
        {
            {{ "1", "2", "3", "4", "5", "5" }, "1" },
            {{ "1", "2", "n3", "4", "5", "5" }, "-3" },
            {{ "1", "2", "n18446744073709551615", "4", "5", "5" }, "-18446744073709551615" },
            {{ "1", "2", "0.5", "4", "18446744073709551615", "5" }, "0.5" },
        }
    );
    add_op_test(
        "Max",
        {
            {{ "1", "2", "3", "4", "5", "5" }, "5" },
            {{ "1", "2", "15", "4", "5", "5" }, "15" },
            {{ "1", "2", "18446744073709551615", "4", "5", "5" }, "18446744073709551615" },
            {{ "1", "2", "6.7", "4", "n18446744073709551615", "5" }, "6.7" },
        }
    );
    add_op_test(
        "Avg",
        {
            {{ "1", "2", "3", "4", "5", "5" }, "3" },
            {{ "1", "2", "15", "4", "5", "5" }, "5.4" },
            {{ "1", "2", "18446744073709551615", "4", "5", "5" }, "3.6893488e+18" },
            {{ "1", "2", "6.7", "4", "n18446744073709551615", "5" }, "-3.6893488e+18" },
        }
    );
    add_op_test(
        "Median",
        {
            {{ "1", "2", "3", "4", "5", "5" }, "3" },
            {{ "1", "2", "15", "4", "5", "5" }, "4" },
            {{ "1", "2", "4", "5", "4" }, "3" }
        }
    );
    add_op_test(
        "StdDev",
        {
            {{ "1", "2", "3", "4", "5", "5" }, "1.4142136" },
            {{ "10", "20", "30", "40", "50", "5" }, "14.1421356" },
            {{ "2", "4", "4", "4", "5", "5", "7", "9", "8" }, "2" },
            {{ "1", "2", "15", "4", "5", "5" }, "5.0039984" }
        }
    );
    add_op_test(
        "SampStdDev",
        {
            {{ "1", "2", "3", "4", "5", "5" }, "1.5811388" },
            {{ "10", "20", "30", "40", "50", "5" }, "15.8113883" },
            {{ "2", "4", "4", "4", "5", "5", "7", "9", "8" }, "2.1380899" },
            {{ "1", "2", "15", "4", "5", "5" }, "5.5946403" }
        }
    );
    add_op_test(
        "NormDist",
        {
            {{ "1", "2", "3", "4", "5", "5" }, "[3, 1.4142136]" },
            {{ "10", "20", "30", "40", "50", "5" }, "[30, 14.1421356]" },
            {{ "2", "4", "4", "4", "5", "5", "7", "9", "8" }, "[5, 2]" },
            {{ "1", "2", "15", "4", "5", "5" }, "[5.4, 5.0039984]" }
        }
    );
    add_op_test(
        "SampNormDist",
        {
            {{ "1", "2", "3", "4", "5", "5" }, "[3, 1.5811388]" },
            {{ "10", "20", "30", "40", "50", "5" }, "[30, 15.8113883]" },
            {{ "2", "4", "4", "4", "5", "5", "7", "9", "8" }, "[5, 2.1380899]" },
            {{ "1", "2", "15", "4", "5", "5" }, "[5.4, 5.5946403]" }
        }
    );
    add_op_test(
        "Sum",
        {
            {{ "1", "2", "3", "4", "5", "5" }, "15" },
            {{ "1", "2", "3", "4", "18446744073709551615", "5" }, "18446744073709551625" },
            {{ "1", "2", "3.5", "4", "18446744073709551615", "5" }, "18446744073709551625.5" },
            {{ "1", "2", "3", "4", "5.5", "5" }, "15.5" },
            {{ "10", "20", "30", "40", "50", "5" }, "150" },
            {{ "2", "4", "4", "4", "5", "5", "7", "9", "8" }, "40" },
            {{ "1", "2", "15", "4", "5", "5" }, "27" }
        }
    );
    add_op_test(
        "NPV",
        {
            {{ "n100000", "10000", "10000", "10000", "10000", "10000", "10000", "10000", "10000", "10000", "10000", "10000", "10000", "0.1", "13" }, "-31863.09" },
        }
    );
    add_op_test(
        "Vec2",
        {
            {{ "1", "2" }, "[1, 2]" },
        }
    );
    add_op_test(
        "Vec3",
        {
            {{ "1", "2", "3" }, "[1, 2, 3]" },
        }
    );
    add_op_test(
        "Vec4",
        {
            {{ "1", "2", "3", "4" }, "[1, 2, 3, 4]" },
        }
    );
    add_op_test(
        "Dot",
        {
            {{ "[1, 0]", "[0, 1]" }, "0" },
            {{ "[1, 0]", "[1, 0]" }, "1" },
            {{ "[1, 0]", "[n0.5, n0.5]" }, "n0.5" },
            {{ "[1, 0, 2]", "[0, 1, 0]" }, "0" },
            {{ "[1, 0, 2]", "[1, 0, 0.5]" }, "2" },
            {{ "[1, 0, 2]", "[n0.5, n0.5, n0.5]" }, "n1.5" },
            {{ "[1, 0, 2, n1]", "[0, 1, 0, 0]" }, "0" },
            {{ "[1, 0, 2, n1]", "[1, 0, 0.5, 1]" }, "1" },
            {{ "[1, 0, 2, n1]", "[n0.5, n0.5, n0.5, n1]" }, "n0.5" },
        }
    );
    add_op_test(
        "Cross",
        {
            {{ "[1, 0, 0]", "[0, 1, 0]" }, "[0, 0, 1]" },
            {{ "[1, 2, 3]", "[1, 5, 7]" }, "[-1, -4, 3]" },
            {{ "[n1, n2, 3]", "[4, 0, n8]" }, "[16, 4, 8]" },
        }
    );
    add_op_test(
        "Len",
        {
            {{ "[1, 0]" }, "1" },
            {{ "[1, 2]" }, "2.2360680" },
            {{ "[n1, n4]" }, "4.1231056" },
            {{ "[16, 4]" }, "16.4924225" },
            {{ "[1, 0, 0]" }, "1" },
            {{ "[1, 2, 3]" }, "3.7416574" },
            {{ "[n1, n4, 3]" }, "5.0990195" },
            {{ "[16, 4, 8]" }, "18.3303028" },
            {{ "[1, 0, 0, 0]" }, "1" },
            {{ "[1, 2, 3, 4]" }, "5.4772256" },
            {{ "[n1, n4, 3, 9]" }, "10.3440804" },
            {{ "[16, 4, 8, n5]" }, "19" },
        }
    );
    add_op_test(
        "LenSqr",
        {
            {{ "[1, 0]" }, "1" },
            {{ "[1, 2]" }, "5" },
            {{ "[n1, n4]" }, "17" },
            {{ "[16, 4]" }, "272" },
            {{ "[1, 0, 0]" }, "1" },
            {{ "[1, 2, 3]" }, "14" },
            {{ "[n1, n4, 3]" }, "26" },
            {{ "[16, 4, 8]" }, "336" },
            {{ "[1, 0, 0, 0]" }, "1" },
            {{ "[1, 2, 3, 4]" }, "30" },
            {{ "[n1, n4, 3, 9]" }, "107" },
            {{ "[16, 4, 8, n5]" }, "361" },
        }
    );
    add_op_test(
        "Norm",
        {
            {{ "[1, 0]" }, "[1, 0]" },
            {{ "[1, 2]" }, "[0.44721360, 0.89442719]" },
            {{ "[n1, n4]" }, "[-0.24253563, -0.97014250]" },
            {{ "[16, 4]" }, "[0.97014250, 0.24253563]" },
            {{ "[1, 0, 0]" }, "[1, 0, 0]" },
            {{ "[1, 2, 3]" }, "[0.26726124, 0.53452248, 0.80178373]" },
            {{ "[n1, n4, 3]" }, "[-0.19611614, -0.78446454, 0.58834841]" },
            {{ "[16, 4, 8]" }, "[0.87287156, 0.21821789, 0.43643578]" },
            {{ "[1, 0, 0, 0]" }, "[1, 0, 0, 0]" },
            {{ "[1, 2, 3, 4]" }, "[0.18257419, 0.36514837, 0.54772256, 0.73029674]" },
            {{ "[n1, n4, 3, 9]" }, "[-0.09667365, -0.38669460, 0.29002095, 0.87006284]" },
            {{ "[16, 4, 8, n5]" }, "[0.84210526, 0.21052632, 0.42105263, -0.26315789]" },
        }
    );
    add_op_test(
        "Concat",
        {
            {{ "1", "2" }, "[1, 2]" },
            {{ "[1, 2]", "3" }, "[1, 2, 3]" },
            {{ "1", "[2, 3]" }, "[1, 2, 3]" },
            {{ "[1, 2, 3]", "4" }, "[1, 2, 3, 4]" },
            {{ "1", "[2, 3, 4]" }, "[1, 2, 3, 4]" },
            {{ "[1, 2]", "[3, 4]" }, "[1, 2, 3, 4]" },
        }
    );
    add_op_test(
        "Rev",
        {
            {{ "[1, 0]" }, "[0, 1]" },
            {{ "[1, 2]" }, "[2, 1]" },
            {{ "[n1, n4]" }, "[-4, -1]" },
            {{ "[16, 4]" }, "[4, 16]" },
            {{ "[1, 0, 0]" }, "[0, 0, 1]" },
            {{ "[1, 2, 3]" }, "[3, 2, 1]" },
            {{ "[n1, n4, 3]" }, "[3, -4, -1]" },
            {{ "[16, 4, 8]" }, "[8, 4, 16]" },
            {{ "[1, 0, 0, 0]" }, "[0, 0, 0, 1]" },
            {{ "[1, 2, 3, 4]" }, "[4, 3, 2, 1]" },
            {{ "[n1, n4, 3, 9]" }, "[9, 3, -4, -1]" },
            {{ "[16, 4, 8, n5]" }, "[-5, 8, 4, 16]" },
        }
    );
    add_op_test(
        "Mat2",
        {
            {{ "[1, 2]", "[3, 4]" }, "{[1, 2], [3, 4]}" }
        }
    );
    add_op_test(
        "Mat3",
        {
            {{ "[1, 2, 3]", "[4, 5, 6]", "[7, 8, 9]" }, "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}" }
        }
    );
    add_op_test(
        "Mat4",
        {
            {{ "[1, 2, 3, 4]", "[5, 6, 7, 8]", "[9, 10, 11, 12]", "[13, 14, 15, 16]" }, "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}" }
        }
    );
    add_op_test(
        "Inv",
        {
            {{ "{[1, 2], [3, 4]}" }, "{[-2, 1], [1.5, -0.5]}" },
            {{ "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}" }, "{[1, -0.4, -0.33333333], [0, 0.2, 0], [0, 0, 0.11111111]}" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}" }, "{[-1.6666667, 0.4, 0.2, 0.06666667], [-1.7666667, 0.6, 0.1, 0.06666667], [-1.8, 0.6, 0.2, 0], [2.9, -0.85, -0.25, -0.05]}" }
        }
    );
    add_op_test(
        "Det",
        {
            {{ "{[1, 2], [3, 4]}" }, "-2" },
            {{ "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}" }, "45" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}" }, "-600" }
        }
    );
    add_op_test(
        "TPose",
        {
            {{ "{[1, 2], [3, 4]}" }, "{[1, 3], [2, 4]}" },
            {{ "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}" }, "{[1, 0, 0], [2, 5, 0], [3, 0, 9]}" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}" }, "{[1, 0, 9, 13], [2, 6, 0, 14], [3, 7, 11, 0], [4, 8, 12, 16]}" }
        }
    );
    add_op_test(
        "Col",
        {
            {{ "{[1, 2], [3, 4]}", "0" }, "[1, 3]" },
            {{ "{[1, 2], [3, 4]}", "1" }, "[2, 4]" },
            {{ "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "0" }, "[1, 0, 0]" },
            {{ "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "1" }, "[2, 5, 0]" },
            {{ "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "2" }, "[3, 0, 9]" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "0" }, "[1, 0, 9, 13]" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "1" }, "[2, 6, 0, 14]" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "2" }, "[3, 7, 11, 0]" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "3" }, "[4, 8, 12, 16]" }
        }
    );
    add_op_test(
        "Row",
        {
            {{ "{[1, 2], [3, 4]}", "0" }, "[1, 2]" },
            {{ "{[1, 2], [3, 4]}", "1" }, "[3, 4]" },
            {{ "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "0" }, "[1, 2, 3]" },
            {{ "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "1" }, "[0, 5, 0]" },
            {{ "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "2" }, "[0, 0, 9]" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "0" }, "[1, 2, 3, 4]" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "1" }, "[0, 6, 7, 8]" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "2" }, "[9, 0, 11, 12]" },
            {{ "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "3" }, "[13, 14, 0, 16]" }
        }
    );
    add_op_test(
        "IMat",
        {
            {{ "2" }, "{[1, 0], [0, 1]}" },
            {{ "3" }, "{[1, 0, 0], [0, 1, 0], [0, 0, 1]}" },
            {{ "4" }, "{[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]}" },
        }
    );
    add_op_test(
        "Resize",
        {
            {{ "{[1, 2], [3, 4]}", "2" }, "{[1, 2], [3, 4]}" },
            {{ "{[1, 2], [3, 4]}", "3" }, "{[1, 2, 0], [3, 4, 0], [0, 0, 1]}" },
            {{ "{[1, 2], [3, 4]}", "4" }, "{[1, 2, 0, 0], [3, 4, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]}" },
            {{ "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}", "2" }, "{[1, 2], [4, 5]}" },
            {{ "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}", "3" }, "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}" },
            {{ "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}", "4" }, "{[1, 2, 3, 0], [4, 5, 6, 0], [7, 8, 9, 0], [0, 0, 0, 1]}" },
            {{ "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}", "2" }, "{[1, 2], [5, 6]}" },
            {{ "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}", "3" }, "{[1, 2, 3], [5, 6, 7], [9, 10, 11]}" },
            {{ "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}", "4" }, "{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}" },
        }
    );
    add_op_test(
        "Transmat",
        {
            {{ "[1, 2]" }, "{[1, 0, 1], [0, 1, 2], [0, 0, 1]}" },
            {{ "[1, 2, 3]" }, "{[1, 0, 0, 1], [0, 1, 0, 2], [0, 0, 1, 3], [0, 0, 0, 1]}" },
        }
    );
    add_op_test(
        "Rotmat",
        {
            {{ "1.25663706144" }, "{[0.30901699, -0.95105652], [0.95105652, 0.30901699]}" },
            {{ "[1.25663706144, 0, 3.14159265359]" }, "{[-1, 0, 0], [0, -0.30901699, 0.95105652], [0, 0.95105652, 0.30901699]}" },
        }
    );
    add_op_test(
        "Scalemat",
        {
            {{ "[1, 2]" }, "{[1, 0], [0, 2]}" },
            {{ "[1, 2, 3]" }, "{[1, 0, 0], [0, 2, 0], [0, 0, 3]}" },
            {{ "[1, 2, 3, 4]" }, "{[1, 0, 0, 0], [0, 2, 0, 0], [0, 0, 3, 0], [0, 0, 0, 4]}" },
        }
    );

    return op_tests;
}

// EXPECTS: Precision set to 8
static RCalc::CowVec<OpErrTest> get_op_err_tests() {
    RCalc::CowVec<OpErrTest> op_tests;
    auto add_op_err_test = [&op_tests](const char * op_name, std::initializer_list<std::initializer_list<const char *>> test_cases) {
        OpErrTest test { op_name };
        test.err_test_cases.reserve(test_cases.size());

        for (auto params : test_cases) {
            RCalc::CowVec<const char *> example_params;
            example_params.reserve(params.size());

            for (const char* param : params) {
                example_params.push_back(param);
            }

            test.err_test_cases.emplace_back(example_params);
        }

        op_tests.push_back(test);
    };

    add_op_err_test(
        "Pow",
        {
            { "n4", "0.5" },
        }
    );
    add_op_err_test(
        "Sqrt",
        {
            { "n4" },
        }
    );
    add_op_err_test(
        "Min",
        {
            { "5" },
            { "[1, 2]", "1" }
        }
    );
    add_op_err_test(
        "Max",
        {
            { "5" },
            { "[1, 2]", "1" }
        }
    );
    add_op_err_test(
        "Avg",
        {
            { "5" },
            { "[1, 2]", "1" }
        }
    );
    add_op_err_test(
        "Median",
        {
            { "5" },
            { "[1, 2]", "1" }
        }
    );
    add_op_err_test(
        "StdDev",
        {
            { "5" },
            { "[1, 2]", "1" }
        }
    );
    add_op_err_test(
        "SampStdDev",
        {
            { "5" },
            { "[1, 2]", "1" }
        }
    );
    add_op_err_test(
        "NormDist",
        {
            { "5" },
            { "[1, 2]", "1" }
        }
    );
    add_op_err_test(
        "SampNormDist",
        {
            { "5" },
            { "[1, 2]", "1" }
        }
    );
    add_op_err_test(
        "Sum",
        {
            { "5" },
            { "[1, 2]", "1" }
        }
    );
    add_op_err_test(
        "NPV",
        {
            { "0.1", "5" },
            { "[1, 2]", "0.1", "1" }
        }
    );
    add_op_err_test(
        "Inv",
        {
            { "{[1, 2, 3], [4, 5, 6], [7, 8, 9]}" },
        }
    );
    add_op_err_test(
        "Col",
        {
            { "{[1, 2], [3, 4]}", "n1" },
            { "{[1, 2], [3, 4]}", "2" },
            { "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "n1" },
            { "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "3" },
            { "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "n1" },
            { "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "4" }
        }
    );
    add_op_err_test(
        "Row",
        {
            { "{[1, 2], [3, 4]}", "n1" },
            { "{[1, 2], [3, 4]}", "2" },
            { "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "n1" },
            { "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "3" },
            { "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "n1" },
            { "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "4" }
        }
    );
    add_op_err_test(
        "IMat",
        {
            { "1" },
            { "5" },
        }
    );
    add_op_err_test(
        "Resize",
        {
            { "{[1, 2], [3, 4]}", "1" },
            { "{[1, 2], [3, 4]}", "5" },
            { "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "1" },
            { "{[1, 2, 3], [0, 5, 0], [0, 0, 9]}", "5" },
            { "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "1" },
            { "{[1, 2, 3, 4], [0, 6, 7, 8], [9, 0, 11, 12], [13, 14, 0, 16]}", "5" }
        }
    );

    return op_tests;
}

}
