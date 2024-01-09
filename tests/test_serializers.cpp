#include "test_serializers.h"

#ifdef TESTS_ENABLED

namespace snitch {

bool append(small_string_span ss, const RCalc::Vec2& vec) {
    return append(ss, "{", vec.x, ",", vec.y, "}");
}

bool append(small_string_span ss, const RCalc::Vec3& vec) {
    return append(ss, "{", vec.x, ",", vec.y, ",", vec.z, "}");
}

bool append(small_string_span ss, const RCalc::Vec4& vec) {
    return append(ss, "{", vec.x, ",", vec.y, ",", vec.z, ",", vec.w, "}");
}

}

#endif