#pragma once

#include <vector>
#include <span>
#include <string_view>
#include "types.h"

#include "core/memory/cowvec.h"

namespace std {

// CowVec iterator comparison is shallow (ptr based), so std::equal is out.

template<typename T>
bool operator==(const std::span<T>& lhs, const RCalc::CowVec<T>& rhs) {
    if (lhs.size() != rhs.size()) { return false; }

    for (size_t index = 0; index < lhs.size(); ++index) {
        if (lhs[index] != rhs[index]) { return false; }
    }

    return true;
}

template<typename T>
bool operator==(const std::span<const T>& lhs, const RCalc::CowVec<T>& rhs) {
    if (lhs.size() != rhs.size()) { return false; }

    for (size_t index = 0; index < lhs.size(); ++index) {
        if (lhs[index] != rhs[index]) { return false; }
    }

    return true;
}

}

namespace RCalc::TypeComparison {

bool compare(std::string_view a, std::string_view b);
bool compare(RCalc::Real a, RCalc::Real b);
bool compare(RCalc::Vec2 a, RCalc::Vec2 b);
bool compare(RCalc::Vec3 a, RCalc::Vec3 b);
bool compare(RCalc::Vec4 a, RCalc::Vec4 b);
bool compare(RCalc::Mat2 a, RCalc::Mat2 b);
bool compare(RCalc::Mat3 a, RCalc::Mat3 b);
bool compare(RCalc::Mat4 a, RCalc::Mat4 b);

}