#pragma once

#include <functional>
#include <string>
#include <map>
#include <vector>

#define REGISTER_COMMAND(scope, name) static void _CMDIMPL_##name(scope&)
    

namespace RCalc {

template<typename Scope>
struct Command {
    const char* name;
    const char* description;
    std::function<void(Scope&)> execute;
    std::vector<const char*> signatures;
};

template<typename Scope>
using CommandMap = std::map<std::string, Command<Scope> const * const>;

template<typename Scope>
CommandMap<Scope> get_command_map();

}