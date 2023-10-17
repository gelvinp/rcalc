#pragma once

#include "core/value.h"

#define UNIT_FAMILY(base_name, unit_name, base_type, unit_usage)
#define UNIT_FROM_BASE(base_name, unit_name, base_type, unit_usage) base_type UNIT_BASE_##base_name##_FROM_##unit_usage(base_type value)
#define UNIT_TO_BASE(base_name, unit_name, base_type, unit_usage) base_type UNIT_BASE_##base_name##_TO_##unit_usage(base_type value)
#define UNUSED(arg) (void)(arg)


namespace RCalc {

Value UNIT_ECHO(Value value) { return value; }

}