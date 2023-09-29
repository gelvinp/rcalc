#pragma once

#include "core/error.h"
#include "app/stack.h"

#define RCALC_OP_0(name) Result<Value> OP0_##name()
#define RCALC_OP_0_S(name) Result<Value> OP0S_##name(RPNStack& stack)
#define RCALC_OP_1(name, type_0, arg_0) Result<Value> OP1_##type_0##_##name(type_0 arg_0)
#define RCALC_OP_1_S(name, type_0, arg_0) Result<Value> OP1S_##type_0##_##name(RPNStack& stack, type_0 arg_0)
#define RCALC_OP_2(name, type_0, type_1, arg_0, arg_1) Result<Value> OP2_##type_0##_##type_1##_##name(type_0 arg_0, type_1 arg_1)
#define RCALC_OP_2_S(name, type_0, type_1, arg_0, arg_1) Result<Value> OP2S_##type_0##_##type_1##_##name(RPNStack& stack, type_0 arg_0, type_1 arg_1)

#define RCALC_FMT_0(name) std::string OP_FormatInput_##name()
#define RCALC_FMT_0_S(name) std::string OPS_FormatInput_##name(RPNStack& stack)
#define RCALC_FMT_1(name, arg_0) std::string OP_FormatInput_##name(const StackItem& arg_0)
#define RCALC_FMT_1_S(name, arg_0) std::string OPS_FormatInput_##name(RPNStack& stack, const StackItem& arg_0)
#define RCALC_FMT_2(name, arg_0, arg_1) std::string OP_FormatInput_##name(const StackItem& arg_0, const StackItem& arg_1)
#define RCALC_FMT_2_S(name, arg_0, arg_1) std::string OPS_FormatInput_##name(RPNStack& stack, const StackItem& arg_0, const StackItem& arg_1)

#define UNUSED(arg) (void)(arg)