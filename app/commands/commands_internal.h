#pragma once


#define RCALC_CMD(scopename, name) void scopename::_CMDIMPL_##name()

template<typename Scope>
using Command = void (Scope::*)();