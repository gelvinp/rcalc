// @RCalcCommand
// Name: Clear
// Scope: Application
// Description: Clears the stack.
// Usage: clear
// Requires: app/application.h
RCALC_CMD(Application, Clear, app) {
    app.stack.clear();
}

// @RCalcCommand
// Name: Quit
// Scope: Application
// Description: Quits RCalc.
// Usage: quit
// Usage: q
RCALC_CMD(Application, Quit, _app) {
    UNUSED(_app);
    Platform::get_singleton().close_requested = true;
}
