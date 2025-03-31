#include "variable_map.h"


namespace RCalc {

void VariableMap::store(std::string name, Value value) {
    auto pair = std::make_pair<std::string, Value>(std::string { name }, Value { value });
    values.push_back(pair);
}

std::optional<Value> VariableMap::load(const std::string& name) const {
    for (size_t index = 0; index < values.size(); ++index) {
        const auto& [e_name, e_value] = values[index];
        if (e_name == name) { return e_value; }
    }
    return std::nullopt;
}

void VariableMap::remove(const std::string& name) {
    bool found = false;
    for (size_t index = 0; index < values.size(); ++index) {
        // Found cannot be true at index 0 so this will not blow up
        if (found) { values.mut_at(index - 1) = values[index]; }

        const auto& [e_name, e_value] = values[index];
        if (e_name == name) { found = true; }
    }

    values.resize(values.size() - 1);
}

void VariableMap::clear() {
    values.clear();
}

}