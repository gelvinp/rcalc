#include "help_cache.h"

#include "app/operators/operators.h"
#include "core/filter.h"
#include "core/format.h"

namespace RCalc {

CachedOperator::CachedOperator(const Operator& op, RPNStack& stack)
    : op(op)
{
    OperatorMap& op_map = OperatorMap::get_operator_map();

    for (const std::vector<const char*>& example_params : op.examples) {
        stack.clear();

        for (const char* param : example_params) {
            Value value = Value::parse(param).value();
            stack.push_item(StackItem { value.to_string(), value.to_string(), std::move(value), false });
        }

        std::string op_name = filter_name(op.name);
        Result<> err = op_map.evaluate(op_name, stack);

        if (!err) {
            Logger::log_err("Cannot display example: %s", err.unwrap_err().get_message().c_str());
            continue;
        }

        bool first = true;
        std::stringstream ss;

        for (const StackItem& item : stack.get_items()) {
            if (first) {
                first = false;
            }
            else {
                ss << ", ";
            }

            ss << item.output;
        }

        StackItem res = std::move(stack.pop_items(1).at(0));

        examples.push_back(fmt("%s -> %s", res.input.c_str(), ss.str().c_str()));
    }
}


CachedOperatorCategory::CachedOperatorCategory(const OperatorCategory& category, RPNStack& stack)
    : category_name(category.category_name)
{
    for (const Operator* op : category.category_ops) {
        category_ops.emplace_back(*op, stack);
    }
}

}