#pragma once

#include "core/value.h"
#include "core/memory/cowvec.h"

#ifdef TESTS_ENABLED

#include "snitch/snitch.hpp"

namespace snitch {

bool append(small_string_span ss, const RCalc::Vec2& vec);
bool append(small_string_span ss, const RCalc::Vec3& vec);
bool append(small_string_span ss, const RCalc::Vec4& vec);
bool append(small_string_span ss, const RCalc::CowVec<const char *>& vec);

}

#endif