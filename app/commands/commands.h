#pragma once

#include <functional>
#include <string>
#include <vector>

#define REGISTER_COMMAND(scope, name) static void _CMDIMPL_##name(scope&)
    

namespace RCalc {

struct CommandMeta {
    const char* name;
    const char* description;
    std::vector<const char*> aliases;
};

template<typename Scope>
using Command = void (*)(Scope&);

struct ScopeMeta {
    const char* scope_name;
    const std::vector<CommandMeta const *>& scope_cmds;
};

class _GlobalCommandMap {
public:
    static const std::vector<ScopeMeta const *>& get_alphabetical();
};

template<typename Scope>
class CommandMap : public _GlobalCommandMap {
public:
    static CommandMap<Scope>& get_command_map();
    bool has_command(const std::string& str);
    void execute(const std::string& str, Scope& scope);

private:
    bool built = false;
    void build();
};

}
