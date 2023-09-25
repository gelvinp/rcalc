from glob import glob
from enum import Enum
from string import ascii_lowercase, digits


class OperatorMapBuilder:
    State = Enum('State', ['WAITING', 'WAITING_NAME', 'CAPTURING', 'ERROR'])
    included_requires = []

    class Operator:
        included_filenames = []

        class Call:
            Types = ['Int', 'BigInt', 'Real', 'Vec2', 'Vec3', 'Vec4', 'Mat2', 'Mat3', 'Mat4']
            Tags = ['reversable', 'stack_ref']

            def __init__(self):
                self.types = []
                self.tags = []
            
            def render(self, name, first):
                lines = [
                    f"\t{'if' if first else 'else if'} (types == \"{self._render_types()}\") {{",
                    f"\t\tvalues = stack.pop_items({len(self.types)});"
                ]

                arg_names = []
                for type in self.types:
                    arg_name = f"arg{len(arg_names)}"
                    lines.append(f"\t\t{type} {arg_name} = values[{len(arg_names)}].result;")
                    arg_names.append(arg_name)
                
                if 'stack_ref' in self.tags:
                    arg_names.insert(0, "stack")
                
                lines.extend([
                    f"\t\tres = OP{len(self.types)}{'S' if 'stack_ref' in self.tags else ''}_{self._render_types()}_{name}({', '.join(arg_names)});",
                    "\t}",
                ])

                if 'reversable' in self.tags:
                    self.types.reverse()

                    lines.extend([
                        f"\telse if (types == \"{self._render_types()}\") {{",
                        f"\t\tvalues = stack.pop_items({len(self.types)});"
                    ])

                    self.types.reverse()

                    offset = len(self.types) - 1
                    arg_names = []
                    for type in self.types:
                        arg_name = f"arg{len(arg_names)}"
                        lines.append(f"\t\t{type} {arg_name} = values[{offset - len(arg_names)}].result;")
                        arg_names.append(arg_name)
                
                    if 'stack_ref' in self.tags:
                        arg_names.insert(0, "stack")

                    lines.extend([
                        f"\t\tres = OP{len(self.types)}{'S' if 'stack_ref' in self.tags else ''}_{self._render_types()}_{name}({', '.join(arg_names)});",
                        "\t}",
                    ])

                return lines
            
            def _render_types(self):
                return "_".join(self.types)

        def __init__(self, filename):
            self.parameter_count = None
            self.calls = []
            self.type_sets = []
            self.description = ""
            self.filenames = [filename]
            
        def render(self, name):
            lines = [f'/* Operator {name} */', '']
            
            included = False
            for filename in self.filenames:
                if not filename in self.__class__.included_filenames:
                    lines.append(f"#include \"{filename}\"")
                    self.__class__.included_filenames.append(filename)
                    included = True
            
            if included:
                lines.append('')
            
            lines.extend([
                f"Result<> OP_Eval_{name}(RPNStack& stack) {{",
                f"\tstd::string types = stack.peek_types({self.parameter_count});",
                "\tstd::vector<StackItem> values;",
                "\tResult<Value> res = Ok(Value());", ""
            ])

            first = True
            for call in self.calls:
                lines.extend(call.render(name, first))
                first = False
            
            call_types = []
            for call in self.calls:
                types = []
                for type in call.types:
                    types.append(f'Value::TYPE_{type.upper()}')
                call_types.append(f"{{{', '.join(types)}}}")
                
                if 'reversable' in call.tags:
                    types = []
                    rev = call.types.copy()
                    rev.reverse()
                    for type in rev:
                        types.append(f'Value::TYPE_{type.upper()}')
                    call_types.append(f"{{{', '.join(types)}}}")

            lines.extend([
                f"\telse {{ return Err(ERR_INVALID_PARAM, \"{name} operator does not recognize types \" + types); }}", '',
                "\tif (!res) { return res.unwrap_err(); }",
                "\tValue value = res.unwrap_move(std::move(res));", '',
                "\tstack.push_item(StackItem{",
                f"\t\tOP_FormatInput_{name}({', '.join([f'values[{idx}]' for idx in range(self.parameter_count)])}),",
                "\t\tvalue.to_string(),",
                "\t\tstd::move(value),",
                "\t\ttrue",
                "\t});", '',
                "\treturn Ok();",
                "}", '',
                f"Operator OP_{name} {{",
                f'\t"{name}",',
                f'\t"{self.description}",',
                f'\t{self.parameter_count},',
                f'\tOP_Eval_{name},',
                '\t{'
            ])

            for call_type in call_types:
                lines.append(f"\t\t{call_type},")
            
            lines.extend(['\t}', '};'])

            return lines

    def __init__(self):
        self.state = self.State.WAITING
        self.error = ""
        self.current_name = ""
        self.current_call = None
        self.operators = {}
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
            "#include \"operators.h\"",
            "#include \"operators_internal.h\"", ""
        ]

        included = False
        for requires in self.requires:
            if not requires in self.__class__.included_requires:
                lines.append(f'#include "{requires}"')
                self.__class__.included_requires.append(requires)
                included = True
        
        if included:
            lines.append('')
        
        lines.extend(["namespace RCalc::Operators {", ""])

        for [name, operator] in self.operators.items():
            lines.extend(operator.render(name))
        
        lines.extend([
            "}", '',
            "RCalc::OperatorMap RCalc::get_operator_map() {",
            "\tOperatorMap map{};", ''
        ])

        for [name, operator] in self.operators.items():
            lines.append(f'\tmap.emplace("{self._filter_name(name)}", &Operators::OP_{name});')
        
        lines.extend(['',
            "\treturn map;",
            "}"
        ])

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
            case self.State.CAPTURING:
                self._process_line_capturing(line)
            case self.State.ERROR:
                pass
        
        self.line_no += 1
    
    def _process_line_waiting(self, line):
        if line.startswith("// @RCalcOperator"):
            self.state = self.State.WAITING_NAME
            self.current_call = self.Operator.Call()
    
    def _process_line_waiting_name(self, line):
        if line.startswith("// Name: "):
            name = line[9:]
            self.current_name = name
            self.state = self.State.CAPTURING

            if name in self.operators:
                self.operators[name].filenames.append(self.filename)
            else:
                self.operators[name] = self.Operator(self.filename)
        else:
            self._set_error("Name must be the first statement in an operator!")
            
    
    def _process_line_capturing(self, line):
        if not line.startswith("// "):
            if self.current_call.types in self.operators[self.current_name].type_sets:
                self._set_error(f"Operator {self.current_name} cannot redeclare types [{', '.join(self.current_call.types)}]")
            else:
                self.state = self.State.WAITING
                self.operators[self.current_name].calls.append(self.current_call)
                self.operators[self.current_name].type_sets.append(self.current_call.types)

                if 'reversable' in self.current_call.tags:
                    rev = self.current_call.types.copy()
                    rev.reverse()
                    self.operators[self.current_name].type_sets.append(rev)
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
            case "Parameters":
                self._process_statement_params(statement_arg)
            case "Description":
                self._process_statement_description(statement_arg)
            case "Tags":
                self._process_statement_tags(statement_arg)
            case "Requires":
                self._process_statement_requires(statement_arg)
            case _:
                self._set_error(f"Statement type '{statement_type}' is unknown!")
    
    def _process_statement_params(self, params):
        if not params.startswith("[") or not params.endswith("]"):
            self._set_error(f"Parameters statement '{params}' is invalid!\n\tExpected form is [type1, type2, ...]")
            return

        params = params[1:-1].split(",")
        match self.operators[self.current_name].parameter_count:
            case None:
                self.operators[self.current_name].parameter_count = len(params)
            case count if count != len(params):
                self._set_error(f"Parameters statement '{params}' is invalid!\n\tOperator{self.current_name} is defined as taking {count} parameters, not {len(params)}")
                return
        
        for param in params:
            self._validate_param_type(param.strip())
    
    def _validate_param_type(self, param):
        if param in self.Operator.Call.Types:
            self.current_call.types.append(param)
        else:
            self._set_error(f"Parameter type `{param}` is unknown!")
    
    def _process_statement_description(self, description):
        self.operators[self.current_name].description = description
    
    def _process_statement_tags(self, tags):
        if not tags.startswith("[") or not tags.endswith("]"):
            self._set_error(f"Tags statement '{tags}' is invalid!\n\tExpected form is [tag1, tag2, ...]")
            return

        tags = tags[1:-1].split(",")
        for tag in tags:
            self._validate_tag(tag.strip())
    
    def _validate_tag(self, tag):
        if tag in self.Operator.Call.Tags:
            if tag == 'reversable' and self.operators[self.current_name].parameter_count != 2:
                self._set_error("'reversable' tag may only be used for operators with two parameters!")
                return

            self.current_call.tags.append(tag)
        else:
            self._set_error(f"Tag `{tag}` is unknown!")
    
    def _process_statement_requires(self, requires):
        self.requires.append(requires)


def make_operators_map(target, source, env):
    dst = target[0]
    builder = OperatorMapBuilder()

    for file in source:
        builder.process_file(file)
    
    if builder.get_error() is not None:
        print(builder.get_error())
        return 255
    
    with open(dst, "w") as file:
        file.write("\n".join(builder.build()))
    
    return 0