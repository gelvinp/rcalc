#include "operators.h"

namespace RCalc {

OperatorMap OperatorMap::singleton;

OperatorMap& OperatorMap::get_operator_map() {
	if (!singleton.built) { singleton.build(); }
	return singleton;
}


std::string OperatorMap::filter_name(const char* p_str) const {
	std::string str(p_str);
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });

	std::string allowed = "abcdefghijklmnopqrstuvwxyz0123456789_";
	std::erase_if(str, [&allowed](char ch) {
		return std::find(allowed.begin(), allowed.end(), ch) == allowed.end();
	});

	return str;
}


}