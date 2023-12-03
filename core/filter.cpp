#include "filter.h"

#include <algorithm>

namespace RCalc {


std::string filter_name(const char* p_str) {
	std::string str(p_str);
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return (char)std::tolower(c); });

	std::string allowed = "abcdefghijklmnopqrstuvwxyz0123456789_";
	std::erase_if(str, [&allowed](char ch) {
		return std::find(allowed.begin(), allowed.end(), ch) == allowed.end();
	});

	return str;
}

}