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
    if (stack.size() == 0) { return; }

    const StackItem& back = stack.get_items().back();
    std::string type_info = fmt("Type: %s", Value::get_type_name(back.result.get_type()));

    p_renderer->display_info(type_info);
}