#include "units.h"

namespace RCalc {

UnitsMap UnitsMap::singleton;

UnitsMap& UnitsMap::get_units_map() {
	return singleton;
}


void UnitFamily::setup() {
	for (Unit* unit : units) {
		unit->p_family = this;
		unit->p_impl = unit;
	}
}

}