#include "operators.h"

namespace RCalc {

OperatorMap OperatorMap::singleton;

OperatorMap& OperatorMap::get_operator_map() {
	if (!singleton.built) { singleton.build(); }
	return singleton;
}

}