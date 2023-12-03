#pragma once

#ifdef MSVC
#define UNREACHABLE() __assume(false);
#else
#define UNREACHABLE() __builtin_unreachable();
#endif

#define UNUSED(arg) (void)(arg);