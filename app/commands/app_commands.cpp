#include "app/application.h"

namespace RCalc {

// @RCalcCommand
// Scope: Application
// Usage: \clear
// Description: Clears the stack.
void Application::appcmd_clear() {
    stack.clear();
}

// @RCalcCommand
// Scope: Application
// Usage: \quit
// Usage: \q
// Description: Quits RCalc.
void Application::appcmd_quit() {
    Platform::get_singleton().close_requested = true;
}

}