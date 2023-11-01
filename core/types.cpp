#include "types.h"


namespace RCalc {

bool is_type_castable(Type from, Type to) {
    if (from == to) {
        return true;
    }
    else if (
        from == TYPE_INT &&
        (
            to == TYPE_BIGINT ||
            to == TYPE_REAL
        )
    ) {
        return true;
    }
    else if (
        from == TYPE_BIGINT &&
        to == TYPE_REAL
    ) {
        return true;
    }

    return false;
}

}