from enum import Enum
from string import ascii_lowercase, digits
import subprocess
import tempfile
import os
from platform_methods import subprocess_main


Tags = ['app_only']


def _filter_name(name):
    # Only allow lowercase letters, numbers, and underscores
    lower = name.lower()
    allowed = list(ascii_lowercase + digits + '_')
    return ''.join([c for c in lower if c in allowed])
    

class Capture:
    def __init__(self, filename: str):
        self.cmd_name = None
        self.cmd_description = None
        self.cmd_aliases = []

        self.call_tags = []
        self.scope_name = None

        self.filename = filename
        self.requires = []


class Command:
    def __init__(self, capture: Capture):
        self.name = capture.cmd_name
        self.description = capture.cmd_description
        self.aliases = capture.cmd_aliases
    

    def build(self, scope_name):
        lines = [
            f'CommandMeta CMDMETA_{scope_name}_{self.name} {{',
            f'\t"{self.name}",',
            f'\t"{self.description}",',
            '\t{',
        ]

        if len(self.aliases) > 0:
            lines.append(',\n'.join([f'\t\t"{alias}"' for alias in self.aliases]))

        lines.extend([
            '\t}',
            '};',
            '',
        ])

        return lines


    def get_usages(self, scope_name):
        usages = {}
        command = f'&{scope_name}::_CMDIMPL_{self.name}'
        
        usages[_filter_name(self.name)] = command
        for alias in self.aliases:
            usages[_filter_name(alias)] = command
        
        return usages


class Scope:
    def __init__(self, capture: Capture):
        self.name = capture.scope_name
        self.commands = { capture.cmd_name: Command(capture) }
        self.seen_usages = [_filter_name(capture.cmd_name)]
        self.seen_usages.extend([_filter_name(alias) for alias in capture.cmd_aliases])
    

    def try_append(self, capture: Capture):
        cmd_name = _filter_name(capture.cmd_name)
        if cmd_name in self.seen_usages:
            return f"Cannot reclare command '{capture.cmd_name}'"
        self.seen_usages.append(cmd_name)

        for cmd_alias in capture.cmd_aliases:
            alias = _filter_name(cmd_alias)
            if alias in self.seen_usages:
                return f"Cannot reclare command '{cmd_alias}'"
            self.seen_usages.append(alias)
        
        self.commands[capture.cmd_name] = Command(capture)
        
        return None
    

    def build(self):
        lines = [f'/* Scope {self.name} */', '']

        commands = list(self.commands.keys())
        commands.sort()

        for cmd_name in commands:
            lines.extend(self.commands[cmd_name].build(self.name))
        
        lines.append(f'std::vector<CommandMeta const *> SCOPECMDS_{self.name} {{')

        if len(commands) > 0:
            lines.append(',\n'.join([f'\t&CMDMETA_{self.name}_{cmd_name}' for cmd_name in commands]))
        
        lines.extend([
            '};',
            '',
            f'ScopeMeta SCOPEMETA_{self.name} {{',
            f'\t"{self.name}",',
            f'\tSCOPECMDS_{self.name}',
            '};',
            '',
        ])

        return lines


    def get_usages(self):
        usages = {}
        
        for _cmd_name, command in self.commands.items():
            usages.update(command.get_usages(self.name))
        
        return usages


class CommandMapBuilder:
    State = Enum('State', ['WAITING', 'CAPTURING', 'ERROR'])

    def __init__(self):
        self.state = self.State.WAITING

        self.error = ""
        self.line_no = 1
        self.filename = ""
        
        self.current_capture = None

        self.scopes = {}
        self.scope_requires = []
        self.scope_filenames = []
    

    def process_file(self, path, env):
        self.filename = str(path)
        self.line_no = 1
        with open(path, 'r') as file:
            for line in file:
                self._process_line(line.rstrip(), env)
    
    def get_error(self):
        if self.state == self.State.ERROR:
            return self.error

        return None

    def build(self, env):
        if self.state == self.State.ERROR:
            return

        # Make sure all commands have a description
        self._finalize_cmds()

        if self.state == self.State.ERROR:
            return

        lines = [
            "/* THIS FILE IS GENERATED DO NOT EDIT */", "",
            "#include \"commands.h\"",
            "#include \"commands_internal.h\"", "",
        ]

        self.scope_requires.sort()
        lines.extend([f'#include {requires}' for requires in self.scope_requires])
        if len(self.scope_requires) > 0:
            lines.append('')
        
        lines.extend([
            'namespace RCalc {',
            ''
        ])

        self.scope_filenames.sort()
        lines.extend([f'#include "{filename}"' for filename in self.scope_filenames])
        if len(self.scope_filenames) > 0:
            lines.append('')
        
        lines.extend([
            'namespace Commands {',
            ''
        ])

        scopes = [scope for scope in list(self.scopes.keys()) if scope in env["enabled_command_scopes"]]
        scopes.sort()
        for scope_name in scopes:
            lines.extend(self.scopes[scope_name].build())
        
        lines.extend([
            '}',
            ''
        ])

        for scope_name in scopes:
            lines.extend([
                'template<>',
                f'void CommandMap<{scope_name}>::activate() {{',
                f'\t_GlobalCommandMap::register_scope(Commands::SCOPEMETA_{scope_name});',
                '}',
                '',
            ])
        
        lines.extend(['}', ''])

        if env["gperf_path"] == '':
            lines.extend(self._build_std_map(scopes))
        else:
            lines.extend(self._build_gperf_map(scopes, env["gperf_path"]))
        
        return lines
    
    
    def _set_error(self, error):
        self.state = self.State.ERROR
        self.error = f"Error at {self.filename}:{self.line_no}\n\t{error}"
    

    def _process_line(self, line, env):
        match self.state:
            case self.State.WAITING:
                self._process_line_waiting(line)
            case self.State.CAPTURING:
                self._process_line_capturing(line, env)
            case self.State.ERROR:
                pass
        
        self.line_no += 1
    

    def _process_line_waiting(self, line):
        # Look for the start of a command
        if line.startswith("// @RCalcCommand"):
            self.state = self.State.CAPTURING
            self.current_capture = Capture(self.filename)
            
    
    def _process_line_capturing(self, line, env):
        # Capture information from the command comments and definition
        if line.startswith("// "):
            self._process_statement(line[3:])
        elif line.startswith("RCALC_CMD"):
            self._process_declaration(line)
        else:
            self._finish_cmd(env)
    
    
    def _process_statement(self, statement):
        split = statement.split(":")
        if len(split) != 2:
            self._set_error(f"Statement '{statement} is invalid!")
            return

        statement_type = split[0]
        statement_arg = split[1].strip()

        match statement_type:
            case "Description":
                self.current_capture.cmd_description = statement_arg
            case "Alias":
                if not statement_arg in self.current_capture.cmd_aliases:
                    self.current_capture.cmd_aliases.append(statement_arg)
            case "Requires":
                if not statement_arg in self.current_capture.requires:
                    self.current_capture.requires.append(statement_arg)
            case "Tags":
                self._validate_tags(statement_arg)
            case _:
                self._set_error(f"Statement type '{statement_type}' is unknown!")
    

    def _validate_tags(self, tags):
        if not tags.startswith("[") or not tags.endswith("]"):
            self._set_error(f"Tag statement '{tags}' is invalid!\n\tExpected form is [tag1, tag2, ...]")
            return

        tags = [tag.strip() for tag in tags[1:-1].split(",")]

        for tag in tags:
            # Ensure the given tag is recognized
            if tag in Tags:
                self.current_capture.call_tags.append(tag)
            else:
                self._set_error(f"Tag `{tag}` is unknown!")
    

    def _process_declaration(self, declaration):
        args_start = declaration.find("(")
        args_end = declaration.find(")")

        if args_start == -1 or args_end == -1:
            self._set_error(f'Operator declaration {declaration} is invalid!\n\tMissing parenthesis around function params')
            return

        args = [arg.strip() for arg in declaration[args_start+1:args_end].split(",")]
        if len(args) != 3:
            self._set_error(f'Operator declaration {declaration} is invalid!\n\tThree arguments are required (Scope name, command name, scope parameter name)')
            return

        self.current_capture.scope_name = args[0]
        self.current_capture.cmd_name = args[1]
    
    
    def _finish_cmd(self, env):
        if (not self.current_capture.scope_name in env["enabled_command_scopes"]) or ('app_only' in self.current_capture.call_tags and env["build_type"] != 'application'):
            self.current_capture = None
            self.state = self.State.WAITING
            return
        
        if self.current_capture.scope_name in self.scopes:
            append_err = self.scopes[self.current_capture.scope_name].try_append(self.current_capture)
            if not append_err is None:
                self._set_error(append_err)
                return
        else:
            scope = Scope(self.current_capture)
            if type(scope) is Scope:
                self.scopes[self.current_capture.scope_name] = scope
            else:
                self._set_error(scope)
                return
        
        if not self.filename in self.scope_filenames:
            self.scope_filenames.append(self.filename)
        
        for require in self.current_capture.requires:
            if not require in self.scope_requires:
                self.scope_requires.append(require)
        
        self.state = self.State.WAITING
    

    def _finalize_cmds(self):
        for scope_name, scope in self.scopes.items():
            for cmd_name, command in scope.commands.items():
                if command.description is None:
                    self._set_error(f"Command {scope_name}::{cmd_name} is missing a description!")
                    return
    

    def _build_std_map(self, scopes: list[str]):
        lines = [
            '// Using std_map',
            '',
            '#include <map>',
            '',
            'namespace RCalc {',
            '',
        ]

        scopes.sort(key=lambda e: e.lower())
        for scope_name in scopes:
            lines.extend([
                f'/* Scope {scope_name} */',
                '',
                f'std::map<std::string, Command<{scope_name}>> CMDMAP_{scope_name};',
                '',
                'template<>',
                f'void CommandMap<{scope_name}>::build() {{',
                '\tif (built) { return; }',
            ])

            usages = self.scopes[scope_name].get_usages()
            usage_names = list(usages.keys())
            usage_names.sort()

            lines.extend([f'\tCMDMAP_{scope_name}.emplace("\\\\{usage}", {usages[usage]});' for usage in usage_names])

            lines.extend([
                '\tbuilt = true;',
                '}',
                '',
                'template<>',
                f'bool CommandMap<{scope_name}>::has_command(const std::string& str) {{',
                '\tif (!built) { build(); }',
                f'\treturn CMDMAP_{scope_name}.contains(str);',
                '}',
                '',
                'template<>',
                f'void CommandMap<{scope_name}>::execute(const std::string& str, {scope_name}& scope) {{',
                '\tif (!built) { build(); }',
                f'\tCMDMAP_{scope_name}.at(str)(scope);',
                '}',
                '',
                'template<>',
                f'CommandMap<{scope_name}>& CommandMap<{scope_name}>::get_command_map() {{',
                f'\tstatic CommandMap<{scope_name}> singleton;',
                '\treturn singleton;',
                '}',
                ''
            ])
        
        lines.append('}')

        return lines
    

    def _build_gperf_map(self, scopes: list[str], gperf_path: str):
        lines = [
            '// Using gperf',
            '',
            '#include <cstring>',
            '#ifndef NDEBUG',
            '#include <cassert>',
            '#include <algorithm>',
            '#endif',
            '',
            'namespace RCalc {',
            '',
        ]

        scopes.sort(key=lambda e: e.lower())
        for scope_name in scopes:
            lines.extend([
                f'/* Scope {scope_name} */',
                '',
                'namespace Commands::GPerf {',
                ''
            ])

            usages = self.scopes[scope_name].get_usages()
            usage_names = list(usages.keys())
            usage_names.sort()

            # Write commands list to temporary file and run gperf
            with tempfile.NamedTemporaryFile("w", newline='\n') as cmd_file:
                for usage in usage_names:
                    cmd_file.write(f'\\{_filter_name(usage)}\n')
                cmd_file.flush()
                cmd_file.flush()
                proc = subprocess.run(
                    [
                        gperf_path,
                        "--language=ANSI-C",
                        "--compare-lengths",
                        "--compare-strncmp",
                        "--readonly-tables",
                        "--switch=4",
                        "--multiple-iterations=10",
                        f"--hash-function-name=SCOPEHASH_{scope_name}",
                        f"--lookup-function-name=SCOPELOOKUP_{scope_name}",
                        os.path.realpath(cmd_file.name)
                    ],
                    capture_output=True,
                    encoding="utf-8"
                )
                proc.check_returncode()
                gperf = str(proc.stdout).replace("register ", "")
                lines.append(gperf)

            lines.extend([
                '}',
                '',
                f'Command<{scope_name}> CMDMAP_{scope_name}[MAX_HASH_VALUE-MIN_HASH_VALUE+1] = {{}};',
                '',
                'template<>',
                f'void CommandMap<{scope_name}>::build() {{',
                '\tif (built) { return; }',
                '#ifndef NDEBUG',
                '\tstd::vector<bool> dbg_vec;',
                '\tdbg_vec.resize(MAX_HASH_VALUE-MIN_HASH_VALUE+1, false);',
                '#endif',
            ])

            for usage_name in usage_names:
                lines.extend([
                    f'\tCMDMAP_{scope_name}[Commands::GPerf::SCOPEHASH_{scope_name}("\\\\{usage_name}", {len(usage_name) + 1}) - MIN_HASH_VALUE] = {usages[usage_name]};'
                ])
            
            lines.append('#ifndef NDEBUG')

            for usage_name in usage_names:
                lines.extend([
                    f'\tdbg_vec[Commands::GPerf::SCOPEHASH_{scope_name}("\\\\{usage_name}", {len(usage_name) + 1}) - MIN_HASH_VALUE] = true;'
                ])

            lines.extend([
                '\tif (std::count(dbg_vec.begin(), dbg_vec.end(), true) != TOTAL_KEYWORDS) {',
                '\t\tthrow std::logic_error("OperatorMap Assert Failed: GPerf hash did not reach every keyword.");',
                '\t}',
                '#endif',
                '\tbuilt = true;',
                '}',
                '',
                'template<>',
                f'bool CommandMap<{scope_name}>::has_command(const std::string& str) {{',
                '\tif (!built) { build(); }',
                f'\treturn Commands::GPerf::SCOPELOOKUP_{scope_name}(str.c_str(), str.size()) != nullptr;',
                '}',
                '',
                'template<>',
                f'void CommandMap<{scope_name}>::execute(const std::string& str, {scope_name}& scope) {{',
                '\tif (!built) { build(); }',
                f'\tCMDMAP_{scope_name}[Commands::GPerf::SCOPEHASH_{scope_name}(str.c_str(), str.size()) - MIN_HASH_VALUE](scope);',
                '}',
                '',
                'template<>',
                f'CommandMap<{scope_name}>& CommandMap<{scope_name}>::get_command_map() {{',
                f'\tstatic CommandMap<{scope_name}> singleton;',
                '\treturn singleton;',
                '}',
                '',
                '#undef TOTAL_KEYWORDS',
                '#undef MIN_WORD_LENGTH',
                '#undef MAX_WORD_LENGTH',
                '#undef MIN_HASH_VALUE',
                '#undef MAX_HASH_VALUE',
                ''
            ])
        
        lines.append('}')

        return lines


def make_command_maps(target, source, env):
    dst = target[0]
    builder = CommandMapBuilder()
    for file in source:
        builder.process_file(file, env)

    built = builder.build(env)
    
    if builder.get_error() is not None:
        print(builder.get_error())
        return 255
    
    with open(dst, "w") as file:
        file.write("\n".join(built))
    
    return 0


if __name__ == "__main__":
    subprocess_main(globals())