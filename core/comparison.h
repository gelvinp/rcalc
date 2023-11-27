#include <vector>
#include <span>
#include "types.h"

namespace std {

template<typename T>
bool operator==(const std::span<T>& lhs, const std::vector<T>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

}