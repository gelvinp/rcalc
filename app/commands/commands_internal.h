#pragma once


#define RCALC_CMD(scopename, name, argname) void scopename::_CMDIMPL_##name(scopename& argname)
