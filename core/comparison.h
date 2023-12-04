#include <vector>
#include <span>
#include "types.h"

#include "logger.h"

namespace std {

// CowVec iterator comparison is shallow (ptr based), so std::equal is out.

template<typename T>
bool operator==(const std::span<T>& lhs, const CowVec<T>& rhs) {
    if (lhs.size() != rhs.size()) { return false; }

    for (size_t index = 0; index < lhs.size(); ++index) {
        if (lhs[index] != rhs[index]) { return false; }
    }

    return true;
}

template<typename T>
bool operator==(const std::span<const T>& lhs, const CowVec<T>& rhs) {
    if (lhs.size() != rhs.size()) { return false; }

    for (size_t index = 0; index < lhs.size(); ++index) {
        if (lhs[index] != rhs[index]) { return false; }
    }

    return true;
}

}