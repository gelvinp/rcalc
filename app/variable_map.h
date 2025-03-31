#pragma once

#include "core/memory/cowvec.h"
#include "core/value.h"

#include <functional>
#include <optional>
#include <string>
#include <utility>


namespace RCalc {

class VariableMap {
public:
    void store(std::string name, Value value);
    std::optional<Value> load(const std::string& name) const;
    void remove(const std::string& name);
    void clear();

    const CowVec<std::pair<std::string, Value>>& get_values() const { return values; }

private:
    CowVec<std::pair<std::string, Value>> values;
};

typedef std::optional<std::reference_wrapper<const VariableMap>> OptionalVariables;

}