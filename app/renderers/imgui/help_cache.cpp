#include "help_cache.h"

#include "app/displayable/displayable.h"
#include "app/operators/operators.h"
#include "core/filter.h"
#include "core/format.h"

namespace RCalc {

CachedOperator::CachedOperator(const Operator& op, RPNStack& stack)
    : op(op)
{
    id = fmt("#op_examples_%s", op.name);

    OperatorMap& op_map = OperatorMap::get_operator_map();

    for (const std::vector<const char*>& example_params : op.examples) {
        stack.clear();

        for (const char* param : example_params) {
            Value value = Value::parse(param).value();
            stack.push_item(StackItem { create_displayables_from(value), std::move(value), false });
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

            ss << item.result.to_string();
        }

        std::vector<StackItem> _items = stack.pop_items(1);
        StackItem& res = _items[0];

        examples.emplace_back(res);
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