// @RCalcCommand
// Name: Clear
// Scope: Application
// Description: Clears the stack.
// Usage: clear
// Requires: "app/application.h"
RCALC_CMD(Application, Clear, app) {
    app.stack.clear();
}

// @RCalcCommand
// Name: Quit
// Scope: Application
// Description: Quits RCalc.
// Usage: quit
// Usage: q
// Requires: "app/application.h"
// Requires: "platform/platform.h"
RCALC_CMD(Application, Quit, _app) {
    UNUSED(_app);
    Platform::get_singleton().close_requested = true;
}

// @RCalcCommand
// Name: Pop
// Scope: Application
// Description: Removes the top element from the stack.
// Usage: pop
// Requires: "app/application.h"
RCALC_CMD(Application, Pop, app) {
    app.stack.pop_items(1);
}

// @RCalcCommand
// Name: Swap
// Scope: Application
// Description: Swaps the top two elements from the stack.
// Usage: swap
// Requires: "app/application.h"
// Requires: <algorithm>
RCALC_CMD(Application, Swap, app) {
    std::vector<StackItem> items = app.stack.pop_items(2);
    app.stack.push_items(std::move(items));
}

// @RCalcCommand
// Name: Copy
// Scope: Application
// Description: Copies the top element from the stack to the clipboard.
// Usage: copy
// Requires: "app/application.h"
// Requires: "platform/platform.h"
RCALC_CMD(Application, Copy, app) {
    if (app.stack.size() == 0) { return; }
    Platform::get_singleton().copy_to_clipboard(app.stack.get_items().back().output);
}

// @RCalcCommand
// Name: Dup
// Scope: Application
// Description: Duplicates the top element from the stack.
// Usage: dup
// Requires: "app/application.h"
RCALC_CMD(Application, Dup, app) {
    if (app.stack.size() == 0) { return; }
    
    const StackItem& back = app.stack.get_items().back();
    app.stack.push_item(StackItem {
        back.input,
        back.output,
        back.result.make_copy(),
        back.input_is_expression
    });
}

// @RCalcCommand
// Name: Count
// Scope: Application
// Description: Pushes the current stack size onto the stack.
// Usage: count
// Requires: "app/application.h"
// Requires: <limits>
RCALC_CMD(Application, Count, app) {
    size_t count = app.stack.size();

    // No floating point, check for int64_t
    if (count > std::numeric_limits<int64_t>::max()) {
        Value value { BigInt((long long int)count) };
        app.stack.push_item(StackItem {
            "count",
            value.to_string(),
            std::move(value),
            false
        });
        return;
    }

    Value value { (int64_t)count };
    app.stack.push_item(StackItem {
        "count",
        value.to_string(),
        std::move(value),
        false
    });
}

