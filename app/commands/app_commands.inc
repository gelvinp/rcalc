// @RCalcCommand
// Description: Clears the stack.
// Requires: "app/application.h"
RCALC_CMD(Application, Clear) {
    stack.clear();
}

// @RCalcCommand
// Description: Removes the top element from the stack.
// Requires: "app/application.h"
RCALC_CMD(Application, Pop) {
    stack.pop_items(1);
}

// @RCalcCommand
// Description: Swaps the top two elements from the stack.
// Requires: "app/application.h"
// Requires: <algorithm>
RCALC_CMD(Application, Swap) {
    if (stack.size() < 2) { return; }
    CowVec<StackItem> items = stack.pop_items(2);
    std::swap(items.mut_at(0), items.mut_at(1));
    stack.try_push_items(std::move(items));
}

// @RCalcCommand
// Description: Duplicates the top element from the stack.
// Requires: "app/application.h"
RCALC_CMD(Application, Dup) {
    if (stack.size() == 0) { return; }
    if (!stack.try_push_item(stack.get_items().back())) {
        p_renderer->display_error("The stack is full.");
    }
}

// @RCalcCommand
// Description: Pushes the current stack size onto the stack.
// Requires: "app/application.h"
// Requires: <limits>
RCALC_CMD(Application, Count) {
    size_t count = stack.size();

    // No floating point, check for int64_t
    if (count > (size_t)std::numeric_limits<int64_t>::max()) {
        Value value { BigInt((long long int)count) };
        if (!stack.try_push_item(StackItem {
            create_displayables_from("count"),
            std::move(value),
            false
        })) {
            p_renderer->display_error("The stack is full.");
        }
        return;
    }

    Value value { (int64_t)count };
    if (!stack.try_push_item(StackItem {
        create_displayables_from("count"),
        std::move(value),
        false
    })) {
        p_renderer->display_error("The stack is full.");
    }
}

// @RCalcCommand
// Description: Return the stack to its state before the previous action.
// Requires: "app/application.h"
RCALC_CMD(Application, Undo) {
    stack = *p_stack_backup;
}

// @RCalcCommand
// Description: Interrogate the type of the top-most value on the stack.
// Requires: "app/application.h"
// Requires: "core/format.h"
RCALC_CMD(Application, Type) {
    if (stack.size() == 0) { p_renderer->display_error("The stack is empty."); return; }

    const StackItem& back = stack.get_items().back();
    std::string type_info = fmt("Type: %s", Value::get_type_name(back.result.get_type()));

    p_renderer->display_info(type_info);
}

// @RCalcCommand
// Description: Store the value arg0 in the identifier arg1.
// Requires: "app/application.h"
RCALC_CMD(Application, Store) {
    if (stack.size() < 2) { p_renderer->display_error("Not enough arguments."); return; }

    const StackItem& back = stack.get_items().back();
    if (back.result.get_type() != TYPE_IDENTIFIER) { p_renderer->display_error("No identifier given."); return; }

    Identifier var_name = back.result;

    stack.pop_items(1);
    const StackItem& value = stack.get_items().back();

    variable_map.store(var_name, value.result);

    std::string value_str = value.result.to_string();

    const char* fmt_str = "Value '%s' stored in variable \"%s\".";

    // Check for matrix formatting
    if (value_str.find('\n') != value_str.npos) {
      fmt_str = "Value\n%s\nstored in variable \"%s\".";
    }

    std::string type_info = fmt(fmt_str, value_str.c_str(), var_name.c_str());
    p_renderer->display_info(type_info);
}

// @RCalcCommand
// Description: Loads the value stored in the identifier arg0.
// Requires: "app/application.h"
RCALC_CMD(Application, Load) {
    if (stack.size() == 0) { p_renderer->display_error("No identifier given."); return; }

    const StackItem& back = stack.get_items().back();
    if (back.result.get_type() != TYPE_IDENTIFIER) { p_renderer->display_error("No identifier given."); return; }

    Identifier var_name = back.result;
    std::optional<Value> value = variable_map.load(var_name);
    
    if (!value) {
        std::string error = fmt("No value stored in variable \"%s\".", var_name.c_str());
        p_renderer->display_error(error);
        return;
    }

    stack.pop_items(1);

    StackItem item {};
    item.p_input = create_displayables_from("load(\"", var_name, "\")");
    item.result = value.value();
    stack.try_push_item(item);
}

// @RCalcCommand
// Description: Clears all stored variables.
// Requires: "app/application.h"
RCALC_CMD(Application, ClearVars) {
    variable_map.clear();
}
