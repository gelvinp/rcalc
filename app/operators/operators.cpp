#include "operators.h"

namespace RCalc {

OperatorMap OperatorMap::singleton;

OperatorMap& OperatorMap::get_operator_map() {
	return singleton;
}


}
