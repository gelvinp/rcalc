from glob import glob
from enum import Enum
from string import ascii_lowercase, digits


class CommandMapBuilder:
    State = Enum('State', ['WAITING', 'WAITING_NAME', 'WAITING_SCOPE', 'CAPTURING', 'ERROR'])
    included_requires = []
    included_filenames = []
    seen_usages = []

    class Scope:

        class Command:
            def __init__(self):
                self.description = ""
                self.usages = []
            
            def render(self, name, scope):
                lines = [
                    f'Command<{scope}> CMD_{scope}_{name} {{',
                    f'\t"{name}",',
                    f'\t"{self.description}",',
                    f'\t{scope}::_CMDIMPL_{name},',
                    '\t{'
                ]

                lines.extend([f'\t\t"\\\\{usage}",' for usage in self.usages])

                lines.extend([
                    '\t}',
                    '};', ''
                ])

                return lines

        def __init__(self, filename):
            self.commands = {}
            self.filenames = [filename]
            
        def render(self, name):
            lines = [
                f'/* Scope {name} */', ''
            ]
            
            for [cmd_name, command] in self.commands.items():
                lines.extend(command.render(cmd_name, name))

            return lines

        def render_map(self, name):
            lines = [
                'template<>',
                f'RCalc::CommandMap<RCalc::{name}> RCalc::get_command_map() {{',
                f'\tCommandMap<{name}> map{{}};', '',
            ]

            for [cmd_name, command] in self.commands.items():
                for usage in command.usages:
                    lines.append(f'\tmap.emplace("\\\\{self._filter_name(usage)}", &Commands::CMD_{name}_{cmd_name});')
            
            lines.extend(['', '\treturn map;', '}', ''])

            return lines
    
        def _filter_name(self, name):
            lower = name.lower()
            allowed = list(ascii_lowercase + digits)
            return ''.join([c for c in lower if c in allowed])

    def __init__(self):
        self.state = self.State.WAITING
        self.error = ""
        self.current_scope = ""
        self.current_name = ""
        self.current_command = None
        self.scopes = {}
        self.line_no = 1
        self.filename = ""
        self.requires = []
    

    def process_file(self, path):
        self.filename = str(path)
        with open(path, 'r') as file:
            for line in file:
                self._process_line(line.rstrip())
    
    def get_error(self):
        if self.state == self.State.ERROR:
            return self.error

        return None

    def build(self):
        lines = [
            "/* THIS FILE IS GENERATED DO NOT EDIT */", "",
            "#include \"commands.h\"",
            "#include \"commands_internal.h\"", "",
        ]

        included = False
        for requires in self.requires:
            if not requires in self.__class__.included_requires:
                lines.append(f'#include {requires}')
                self.__class__.included_requires.append(requires)
                included = True
        
        if included:
            lines.append('')
        
        lines.extend(["namespace RCalc {", ""])

        included = False
        for [_, scope] in self.scopes.items():
            for filename in scope.filenames:
                if not filename in self.__class__.included_filenames:
                    lines.append(f'#include "{filename}"')
                    self.__class__.included_filenames.append(filename)
                    included = True
        
        if included:
            lines.append('')
        
        lines.extend([
            '}', '',
            'namespace RCalc::Commands {', ''
        ])

        for [name, scope] in self.scopes.items():
            lines.extend(scope.render(name))
        
        lines.extend(['}', ''])

        for [name, scope] in self.scopes.items():
            lines.extend(scope.render_map(name))

        return lines
    
    def _filter_name(self, name):
        lower = name.lower()
        allowed = list(ascii_lowercase + digits)
        return ''.join([c for c in lower if c in allowed])
    
    def _set_error(self, error):
        self.state = self.State.ERROR
        self.error = f"Error at {self.filename}:{self.line_no}\n\t{error}"
    
    def _process_line(self, line):
        match self.state:
            case self.State.WAITING:
                self._process_line_waiting(line)
            case self.State.WAITING_NAME:
                self._process_line_waiting_name(line)
            case self.State.WAITING_SCOPE:
                self._process_line_waiting_scope(line)
            case self.State.CAPTURING:
                self._process_line_capturing(line)
            case self.State.ERROR:
                pass
        
        self.line_no += 1
    
    def _process_line_waiting(self, line):
        if line.startswith("// @RCalcCommand"):
            self.state = self.State.WAITING_NAME
    
    def _process_line_waiting_name(self, line):
        if line.startswith("// Name: "):
            name = line[9:]
            self.current_name = name
            self.state = self.State.WAITING_SCOPE
        else:
            self._set_error("Name must be the first statement in an operator!")
    
    def _process_line_waiting_scope(self, line):
        if line.startswith("// Scope: "):
            scope = line[10:]
            self.current_scope = scope
            self.state = self.State.CAPTURING
            self.current_command = self.Scope.Command()

            if scope in self.scopes:
                if self.current_name in self.scopes[scope].commands:
                    self._set_error(f'Cannot redeclare command {self.current_scope}::{self.current_name}!')
                    return

                self.scopes[scope].filenames.append(self.filename)
            else:
                self.scopes[scope] = self.Scope(self.filename)
        else:
            self._set_error("Name must be the first statement in an operator!")
            
    
    def _process_line_capturing(self, line):
        if not line.startswith("// "):
            self.state = self.State.WAITING
            self.scopes[self.current_scope].commands[self.current_name] = self.current_command
            return
        
        self._process_statement(line[3:])
    
    def _process_statement(self, statement):
        split = statement.split(":")
        if len(split) != 2:
            self._set_error(f"Statement '{statement} is invalid!")
            return

        statement_type = split[0]
        statement_arg = split[1].strip()

        match statement_type:
            case "Name":
                self._set_error("Name statement cannot appear multiple times in one operator!")
            case "Scope":
                self._set_error("Scope statement cannot appear multiple times in one operator!")
            case "Description":
                self._process_statement_description(statement_arg)
            case "Usage":
                self._process_statement_usage(statement_arg)
            case "Requires":
                self._process_statement_requires(statement_arg)
            case _:
                self._set_error(f"Statement type '{statement_type}' is unknown!")
    
    def _process_statement_description(self, description):
        self.current_command.description = description
    
    def _process_statement_usage(self, usage):
        if usage in self.seen_usages:
            self._set_error(f'Usage "{usage}" cannot be redefined!')
        else:
            self.seen_usages.append(usage)
            self.current_command.usages.append(usage)
    
    def _process_statement_requires(self, requires):
        self.requires.append(requires)


def make_command_maps(target, source, env):
    dst = target[0]
    builder = CommandMapBuilder()

    for file in source:
        builder.process_file(file)
    
    if builder.get_error() is not None:
        print(builder.get_error())
        return 255

    built = builder.build()
    
    with open(dst, "w") as file:
        file.write("\n".join(built))
    
    return 0
