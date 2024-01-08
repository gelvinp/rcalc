#pragma once

#include <functional>
#include <string>
#include <vector>
#include <span>
#include <string_view>
#include <utility>

#define REGISTER_COMMAND(scope, name) void _CMDIMPL_##name()
    

namespace RCalc {

struct CommandMeta {
    const char* name;
    const char* description;
    const std::span<const char * const> aliases;

    constexpr CommandMeta(const char* name, const char* description, const std::span<const char * const> aliases)
	: name(name), description(description), aliases(aliases) {}
};

struct ScopeMeta {
    const char* scope_name;
    const std::span<CommandMeta const * const> scope_cmds;
};

class _CommandMap {
public:
    const std::vector<CommandMeta const *>& get_alphabetical() const { return commands; }
    static const std::span<ScopeMeta const * const> get_compiled_alphabetical();

    void register_scope(const ScopeMeta& scope);

private:
    std::vector<CommandMeta const *> commands = {};
    std::vector<const char*> active_scopes = {};
};

template<typename Scope>
class CommandMap : public _CommandMap {
public:
    static CommandMap<Scope>& get_command_map();

    void activate(_CommandMap& cmd_map);
    bool has_command(std::string_view str);
    void execute(std::string_view str, Scope& scope);

private:
    bool built = false;
    void build();
};

}
