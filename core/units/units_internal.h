#pragma once

#include "core/value.h"
#include "core/error.h"

#define UNIT_FAMILY(base_name, base_type, unit_name, unit_usage)
#define UNIT_FROM_BASE(base_name, base_type, unit_name, unit_usage) Result<base_type> UNIT_BASE_##base_name##_FROM_##unit_usage(base_type value)
#define UNIT_TO_BASE(base_name, base_type, unit_name, unit_usage) Result<base_type> UNIT_BASE_##base_name##_TO_##unit_usage(base_type value)


namespace RCalc {

template<typename T>
Result<T> UNIT_ECHO(T value) { return Ok(value); }

template<>
Result<Value> UNIT_ECHO<Value>(Value value) { return Ok(std::move(value)); }

}