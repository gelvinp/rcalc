#pragma once

#include <functional>
#include <string>
#include <vector>
#include <span>

#define REGISTER_COMMAND(scope, name) static void _CMDIMPL_##name(scope&)
    

namespace RCalc {

struct CommandMeta {
    const char* name;
    const char* description;
    const std::span<const char * const> aliases;
};

template<typename Scope>
using Command = void (*)(Scope&);

struct ScopeMeta {
    const char* scope_name;
    const std::span<CommandMeta const * const> scope_cmds;
};

class _GlobalCommandMap {
public:
    static const std::vector<CommandMeta const *>& get_alphabetical() { return commands; }

protected:
    static void register_scope(const ScopeMeta& scope);

private:
    static std::vector<CommandMeta const *> commands;
};

template<typename Scope>
class CommandMap : public _GlobalCommandMap {
public:
    static CommandMap<Scope>& get_command_map();

    void activate();
    bool has_command(const std::string& str);
    void execute(const std::string& str, Scope& scope);

private:
    bool built = false;
    void build();
};

}
