#include <vector>
#include <span>
#include "types.h"

namespace std {

template<typename T>
bool operator==(const std::span<T>& lhs, const CowVec<T>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T>
bool operator==(const std::span<const T>& lhs, const CowVec<T>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

}