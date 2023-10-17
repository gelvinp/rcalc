#pragma once

#include "core/types.h"


#include <functional>
#include <optional>
#include <vector>

namespace RCalc {

struct Unit;
class Value;


struct UnitFamily {
    const char* p_name;
    Type base_type;
    const std::vector<Unit*>& units;

    void setup();
};


struct Unit {
    const char* p_name;
    const char* p_usage;
    const UnitFamily* p_family;

    Value(*from_base)(Value);
    Value(*to_base)(Value);
};


class UnitsMap {
public:
    static UnitsMap& get_units_map();
    std::optional<const Unit*> find_unit(const std::string& str);
    const std::vector<UnitFamily const *>& get_alphabetical() const;

private:
    bool built = false;
    void build();

    static UnitsMap singleton;
};

}