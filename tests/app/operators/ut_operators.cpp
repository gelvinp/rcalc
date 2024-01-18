#include "snitch/snitch.hpp"
#include "app/operators/operators.h"
#include "core/filter.h"
#include "ut_operator_examples.h"
#include "tests/test_serializers.h"
#include "core/comparison.h"

using namespace RCalc;
using namespace RCalc::TypeComparison;


// TEST_CASE("Examples do not return an error", "[core][allocates]") {
//     RPNStack stack;
//     OperatorMap& op_map = OperatorMap::get_operator_map();

//     for (const OperatorCategory* category : op_map.get_alphabetical()) {
//         for (const Operator* op : category->category_ops) {
//             REQUIRE(op_map.has_operator(filter_name(op->name)));
//             for (const std::span<const char * const>& example_params : op->examples) {
//                 stack.clear();

//                 for (const char* param : example_params) {
//                     Value value = Value::parse(param).value();
//                     stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });
//                 }

//                 std::string op_name = filter_name(op->name);
//                 Result<std::optional<size_t>> err = op_map.evaluate(op_name, stack);
//                 REQUIRE(err.operator bool());
//             }
//         }
//     }
// }

TEST_CASE("Operator test cases", "[core][allocates]") {
    RPNStack stack;
    OperatorMap& op_map = OperatorMap::get_operator_map();
    CowVec<OpTest> op_tests = get_op_tests();
    CowVec<OpErrTest> op_err_tests = get_op_err_tests();

    int old_precision = Value::get_precision();
    Value::set_precision(8);

    SECTION("Positive tests") {
        for (const OpTest& test : op_tests) {
            CAPTURE(test.op_name);

            for (const OpTestCase& test_case : test.test_cases) {
                stack.clear();

                CAPTURE(test_case.params);

                for (const char* param : test_case.params) {
                    Value value = Value::parse(param).value();
                    stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });
                }

                std::string op_name = filter_name(test.op_name);
                Result<std::optional<size_t>> err = op_map.evaluate(op_name, stack);
                REQUIRE(err.operator bool());

                CowVec<StackItem> _items = stack.pop_items(1);
                const StackItem& res = _items[0];
                Value value = res.result;

                CAPTURE(value.to_string(DisplayableTag::ONE_LINE));

                REQUIRE(compare(value.to_string(DisplayableTag::ONE_LINE), test_case.result));
            }
        }
    }

    SECTION("Negative tests") {
        for (const OpErrTest& test : op_err_tests) {
            CAPTURE(test.op_name);

            for (const OpErrTestCase& test_case : test.err_test_cases) {
                stack.clear();

                CAPTURE(test_case.params);

                for (const char* param : test_case.params) {
                    Value value = Value::parse(param).value();
                    stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });
                }

                std::string op_name = filter_name(test.op_name);
                Result<std::optional<size_t>> err = op_map.evaluate(op_name, stack);
                REQUIRE_FALSE(err.operator bool());
            }
        }
    }

    Value::set_precision(old_precision);
}

TEST_CASE("Addition/Multiplication expressions", "[core][allocates]") {
    RPNStack stack;
    OperatorMap& op_map = OperatorMap::get_operator_map();
    CowVec<OpTest> op_tests = get_op_tests();

    Value value = Value::parse("1").value();
    stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });
    value = Value::parse("2").value();
    stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

    SECTION("Two adds") {
        {
            Result<std::optional<size_t>> err = op_map.evaluate("add", stack);
            REQUIRE(err.operator bool());

            const CowVec<StackItem>& _items = stack.get_items();
            REQUIRE(_items.size() == 1z);
            const StackItem& res = _items[0];
            REQUIRE(res.p_input->dbg_display() == "1 + 2");
        }

        value = Value::parse("3").value();
        stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

        Result<std::optional<size_t>> err = op_map.evaluate("add", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "1 + 2 + 3");
    }

    SECTION("Mul then add") {
        {
            Result<std::optional<size_t>> err = op_map.evaluate("mul", stack);
            REQUIRE(err.operator bool());

            const CowVec<StackItem>& _items = stack.get_items();
            REQUIRE(_items.size() == 1);
            const StackItem& res = _items[0];
            REQUIRE(res.p_input->dbg_display() == "1 × 2");
        }

        value = Value::parse("3").value();
        stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

        Result<std::optional<size_t>> err = op_map.evaluate("add", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "(1 × 2) + 3");
    }

    SECTION("Add then nul") {
        {
            Result<std::optional<size_t>> err = op_map.evaluate("add", stack);
            REQUIRE(err.operator bool());

            const CowVec<StackItem>& _items = stack.get_items();
            REQUIRE(_items.size() == 1);
            const StackItem& res = _items[0];
            REQUIRE(res.p_input->dbg_display() == "1 + 2");
        }

        value = Value::parse("3").value();
        stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

        Result<std::optional<size_t>> err = op_map.evaluate("mul", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "(1 + 2) × 3");
    }

    SECTION("Add then add reversed") {
        value = Value::parse("3").value();
        stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

        {
            Result<std::optional<size_t>> err = op_map.evaluate("add", stack);
            REQUIRE(err.operator bool());

            const CowVec<StackItem>& _items = stack.get_items();
            REQUIRE(_items.size() == 2);
            const StackItem& res = _items[1];
            REQUIRE(res.p_input->dbg_display() == "2 + 3");
        }

        Result<std::optional<size_t>> err = op_map.evaluate("add", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "1 + 2 + 3");
    }

    SECTION("Mul then add reversed") {
        value = Value::parse("3").value();
        stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

        {
            Result<std::optional<size_t>> err = op_map.evaluate("mul", stack);
            REQUIRE(err.operator bool());

            const CowVec<StackItem>& _items = stack.get_items();
            REQUIRE(_items.size() == 2);
            const StackItem& res = _items[1];
            REQUIRE(res.p_input->dbg_display() == "2 × 3");
        }

        Result<std::optional<size_t>> err = op_map.evaluate("add", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "1 + (2 × 3)");
    }

    SECTION("Add then mul reversed") {
        value = Value::parse("3").value();
        stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

        {
            Result<std::optional<size_t>> err = op_map.evaluate("add", stack);
            REQUIRE(err.operator bool());

            const CowVec<StackItem>& _items = stack.get_items();
            REQUIRE(_items.size() == 2);
            const StackItem& res = _items[1];
            REQUIRE(res.p_input->dbg_display() == "2 + 3");
        }

        Result<std::optional<size_t>> err = op_map.evaluate("mul", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "1 × (2 + 3)");
    }

    SECTION("Mul then mul reversed") {
        value = Value::parse("3").value();
        stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

        {
            Result<std::optional<size_t>> err = op_map.evaluate("mul", stack);
            REQUIRE(err.operator bool());

            const CowVec<StackItem>& _items = stack.get_items();
            REQUIRE(_items.size() == 2);
            const StackItem& res = _items[1];
            REQUIRE(res.p_input->dbg_display() == "2 × 3");
        }

        Result<std::optional<size_t>> err = op_map.evaluate("mul", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "1 × 2 × 3");
    }

    SECTION("Two muls") {
        {
            Result<std::optional<size_t>> err = op_map.evaluate("mul", stack);
            REQUIRE(err.operator bool());

            const CowVec<StackItem>& _items = stack.get_items();
            REQUIRE(_items.size() == 1);
            const StackItem& res = _items[0];
            REQUIRE(res.p_input->dbg_display() == "1 × 2");
        }

        value = Value::parse("3").value();
        stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

        Result<std::optional<size_t>> err = op_map.evaluate("mul", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "1 × 2 × 3");
    }
}

TEST_CASE("Column vectors", "[core][allocates]") {
    RPNStack stack;
    OperatorMap& op_map = OperatorMap::get_operator_map();
    CowVec<OpTest> op_tests = get_op_tests();

    Value value = Value::parse("{[1, 2], [3, 4]}").value();
    stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });
    value = Value::parse("[5, 6]").value();
    stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

    SECTION("Mul") {
        Result<std::optional<size_t>> err = op_map.evaluate("mul", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "| 1, 2 |\n| 3, 4 | × | 5 |\n| 6 |");
    }

    SECTION("Div") {
        Result<std::optional<size_t>> err = op_map.evaluate("div", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "| 1, 2 |\n| 3, 4 | / | 5 |\n| 6 |");
    }
}

TEST_CASE("Negation stack", "[core][allocates]") {
    RPNStack stack;
    OperatorMap& op_map = OperatorMap::get_operator_map();
    CowVec<OpTest> op_tests = get_op_tests();

    Value value = Value::parse("123").value();
    stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

    {
        Result<std::optional<size_t>> err = op_map.evaluate("neg", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "-123");
    }
    {
        Result<std::optional<size_t>> err = op_map.evaluate("neg", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 1);
        const StackItem& res = _items[0];
        REQUIRE(res.p_input->dbg_display() == "123");
    }
}

TEST_CASE("Range", "[core][allocates]") {
    RPNStack stack;
    OperatorMap& op_map = OperatorMap::get_operator_map();
    CowVec<OpTest> op_tests = get_op_tests();

    SECTION("With zero") {
        Value value = Value::parse("123").value();
        stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

        Result<std::optional<size_t>> err = op_map.evaluate("range", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 124);

        for (Int index = 0; index < 124; ++index) {
            REQUIRE(_items[index].result == Value(index));
        }
    }

    SECTION("Without zero") {
        Value value = Value::parse("n123").value();
        stack.try_push_item(StackItem { create_displayables_from(value), std::move(value), false });

        Result<std::optional<size_t>> err = op_map.evaluate("range", stack);
        REQUIRE(err.operator bool());

        const CowVec<StackItem>& _items = stack.get_items();
        REQUIRE(_items.size() == 123);

        for (Int index = 0; index < 123; ++index) {
            REQUIRE(_items[index].result == Value(index + 1));
        }
    }
}