#include "commands.h"

namespace RCalc {

std::vector<CommandMeta const *> _GlobalCommandMap::commands = {};

void _GlobalCommandMap::register_scope(const ScopeMeta& scope) {
    commands.reserve(commands.size() + scope.scope_cmds.size());

    for (const CommandMeta* cmd : scope.scope_cmds) {
        commands.push_back(cmd);
    }
    
    std::sort(commands.begin(), commands.end(), [](CommandMeta const* a, CommandMeta const* b) {
        std::string_view sva { a->name };
        std::string_view svb { b->name };
        return std::lexicographical_compare(sva.begin(), sva.end(), svb.begin(), svb.end());
    });
}

}