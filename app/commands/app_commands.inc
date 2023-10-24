// @RCalcCommand
// Description: Clears the stack.
// Requires: "app/application.h"
RCALC_CMD(Application, Clear, app) {
    app.stack.clear();
}

// @RCalcCommand
// Description: Quits RCalc.
// Alias: q
// Requires: "app/application.h"
RCALC_CMD(Application, Quit, app) {
    app.p_renderer->p_backend->close_requested = true;
}

// @RCalcCommand
// Description: Removes the top element from the stack.
// Requires: "app/application.h"
RCALC_CMD(Application, Pop, app) {
    app.stack.pop_items(1);
}

// @RCalcCommand
// Description: Swaps the top two elements from the stack.
// Requires: "app/application.h"
// Requires: <algorithm>
RCALC_CMD(Application, Swap, app) {
    std::vector<StackItem> items = app.stack.pop_items(2);
    std::swap(items[0], items[1]);
    app.stack.push_items(std::move(items));
}

// @RCalcCommand
// Description: Copies the top element from the stack to the clipboard.
// Requires: "app/application.h"
RCALC_CMD(Application, Copy, app) {
    if (app.stack.size() == 0) { return; }
    app.p_renderer->p_backend->copy_to_clipboard(app.stack.get_items().back().result.to_string());
}

// @RCalcCommand
// Description: Duplicates the top element from the stack.
// Requires: "app/application.h"
RCALC_CMD(Application, Dup, app) {
    if (app.stack.size() == 0) { return; }
    
    const StackItem& back = app.stack.get_items().back();
    app.stack.push_item(StackItem(back));
}

// @RCalcCommand
// Description: Pushes the current stack size onto the stack.
// Requires: "app/application.h"
// Requires: <limits>
RCALC_CMD(Application, Count, app) {
    size_t count = app.stack.size();

    // No floating point, check for int64_t
    if (count > std::numeric_limits<int64_t>::max()) {
        Value value { BigInt((long long int)count) };
        app.stack.push_item(StackItem {
            "count",
            std::move(value),
            false
        });
        return;
    }

    Value value { (int64_t)count };
    app.stack.push_item(StackItem {
        "count",
        std::move(value),
        false
    });
}

