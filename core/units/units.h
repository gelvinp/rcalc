#pragma once

#include "core/types.h"
#include "core/error.h"

#include <functional>
#include <optional>
#include <span>

namespace RCalc {

struct Unit;
class Value;

template<typename T>
struct UnitImpl;


struct UnitFamily {
    const char* p_name;
    Type base_type;
    const std::span<Unit*> units;

    void setup();
};


struct Unit {
    const char* p_name;
    const char* p_usage;
    const UnitFamily* p_family;
    const Unit* p_impl;

    template<typename T>
    const UnitImpl<T>& get_impl() const {
        return *reinterpret_cast<const UnitImpl<T>*>(p_impl);
    }
};


template<typename T>
struct UnitImpl : public Unit {
    Result<T>(*from_base)(T);
    Result<T>(*to_base)(T);
};


class UnitsMap {
public:
    static UnitsMap& get_units_map();
    std::optional<const Unit*> find_unit(const std::string& str);
    const std::span<UnitFamily const *> get_alphabetical() const;

private:
    bool built = false;
    void build();

    static UnitsMap singleton;
};

}
