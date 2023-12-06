#include "help_cache.h"

#include "app/displayable/displayable.h"
#include "app/operators/operators.h"
#include "core/filter.h"
#include "core/format.h"

namespace RCalc::ImGuiHelpCache {


CachedOperator::CachedOperator(const Operator& op) : op(op) {
    id = fmt("#op_examples_%s", op.name);
}

void CachedOperator::build() {
    RPNStack stack;

    OperatorMap& op_map = OperatorMap::get_operator_map();

    for (const std::span<const char * const>& example_params : op.examples) {
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

        CowVec<StackItem> _items = stack.pop_items(1);
        const StackItem& res = _items[0];

        ImGuiDisplayEntry entry { res };
        entry.output.str = ss.str();

        examples.push_back(entry);
    }
}


CachedOperatorCategory::CachedOperatorCategory(const OperatorCategory& category)
    : category_name(category.category_name)
{
    for (const Operator* op : category.category_ops) {
        category_ops.emplace_back(*op);
    }
}

}
