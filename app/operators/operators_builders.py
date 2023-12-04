from enum import Enum
from string import ascii_lowercase, digits
import itertools
import re
from copy import deepcopy
import subprocess
import tempfile
import os
from platform_methods import subprocess_main
import atexit


Tags = ['reversable', 'bigint_cast', 'real_cast', 'no_expr'] # Need a string in set check, not ordered
Types = { 'Int': 0, 'BigInt': 1, 'Real': 2, 'Vec2': 3, 'Vec3': 4, 'Vec4': 5, 'Mat2': 6, 'Mat3': 7, 'Mat4': 8, 'Unit': 9 } # Need a string in set check, ordered


class Capture:
    def __init__(self, filename: str):
        self.op_name = None
        self.op_description = None
        self.op_category = None

        self.call_types = []
        self.call_tags = []
        self.call_examples = []
        self.call_stack_ref = False

        self.filename = filename


class PermutationType:
    ComplexConvert = {
        'Int>BigInt': lambda stack_number: f'BigInt((long long int)(values[{stack_number}].result.operator Int()))',
        'BigInt>Real': lambda stack_number: f'(values[{stack_number}].result.operator BigInt()).get_real<Real>()'
    }


    def __init__(self, arg_type: str, arg_number: int, stack_type: str, stack_number: int):
        self.arg_type = arg_type
        self.arg_number = arg_number
        self.stack_type = stack_type
        self.stack_number = stack_number
    

    def build(self):
        if self.arg_type == self.stack_type:
            return f'\t\t\t{self.arg_type} arg{self.arg_number} = values[{self.stack_number}].result;'
        else:
            complex_convert = f'{self.stack_type}>{self.arg_type}'
            if complex_convert in self.ComplexConvert:
                return f'\t\t\t{self.arg_type} arg{self.arg_number} = {self.ComplexConvert[complex_convert](self.stack_number)};'
            else:
                return f'\t\t\t{self.arg_type} arg{self.arg_number} = {self.arg_type}(values[{self.stack_number}].result.operator {self.stack_type}());'


class Permutation:
    def __init__(self, types: list[PermutationType], function_types: list[str], call_args: list[int], stack_ref: bool):
        self.types = types
        self.function_types = function_types
        self.call_args = call_args
        self.stack_ref = stack_ref
        self.source = []
    

    def reverse(self):
        rev_types = deepcopy(self.types)
        rev_types[0].stack_number = 1
        rev_types[1].stack_number = 0

        rev = Permutation(rev_types, self.function_types, [1, 0], self.stack_ref)
        rev.source = self.source.copy()
        rev.source.append("reversed")
        return rev


    def get_type_names(self):
        return [self.types[idx].stack_type for idx in self.call_args]
    

    def build(self, name: str, tags: list[str]):
        lines = [type.build() for type in self.types]

        arg_names = [f'arg{type.arg_number}' for type in self.types]
        if self.stack_ref:
            arg_names.insert(0, "stack")

        if 'no_expr' in tags:
            lines.append('\t\t\texpression = false;')
        
        lines.extend([
            f"\t\t\tres = OP{len(self.types)}{'S' if self.stack_ref else ''}_{'_'.join(self.function_types)}_{name}({', '.join(arg_names)});",
            '\t\t\tbreak;'
        ])

        return lines


class Call:
    def __init__(self, capture: Capture):
        self.types = capture.call_types
        self.tags = capture.call_tags
        self.stack_ref = capture.call_stack_ref
    

    def get_permutations(self, manually_defined: list[str]):
        defined = manually_defined.copy()
        permutations = {}

        # Add our original permutation
        perm_types = []
        perm_call_args = []
        for type_idx in range(len(self.types)):
            perm_types.append(
                PermutationType(
                    self.types[type_idx],
                    type_idx,
                    self.types[type_idx],
                    type_idx
                )
            )
            perm_call_args.append(type_idx)
        
        perm = Permutation(perm_types, self.types, perm_call_args, self.stack_ref)
        permutations[",".join(self.types)] = perm

        # Check for reverse
        if 'reversable' in self.tags:
            reversed_types = [self.types[1], self.types[0]]
            if not reversed_types in defined:
                defined.append(reversed_types)
                permutations[",".join(reversed_types)] = perm.reverse()
        
        # Check for BigInt cast
        if 'bigint_cast' in self.tags:
            bigint_cast_types = ['BigInt', 'Int']
            typesets = []
            
            for type in self.types:
                if type == 'BigInt':
                    typesets.append(bigint_cast_types)
                else:
                    typesets.append([type])
            
            bigint_perm_types = [list(x) for x in list(itertools.product(*typesets))]
            bigint_perm_types.sort(key=lambda types: [Types[t] for t in types])
            for bigint_perm_type in bigint_perm_types:
                if bigint_perm_type in defined:
                    continue
                
                defined.append(bigint_perm_types)
                perm_types = []
                perm_call_args = []
                for type_idx in range(len(bigint_perm_type)):
                    value_type = 'BigInt' if bigint_perm_type[type_idx] in bigint_cast_types else bigint_perm_type[type_idx]
                    perm_types.append(
                        PermutationType(
                            value_type,
                            type_idx,
                            bigint_perm_type[type_idx],
                            type_idx
                        )
                    )
                    perm_call_args.append(type_idx)
        
                perm = Permutation(perm_types, self.types, perm_call_args, self.stack_ref)
                perm.source.append("bigint cast")
                permutations[",".join(bigint_perm_type)] = perm

                # Check for reverse
                if 'reversable' in self.tags:
                    reversed_types = [bigint_perm_type[1], bigint_perm_type[0]]
                    if not reversed_types in defined:
                        defined.append(reversed_types)
                        permutations[",".join(reversed_types)] = perm.reverse()
        
        # Check for Real cast
        if 'real_cast' in self.tags:
            real_cast_types = ['Real', 'BigInt', 'Int']
            typesets = []
            
            for type in self.types:
                if type == 'Real':
                    typesets.append(real_cast_types)
                else:
                    typesets.append([type])
            
            real_perm_types = [list(x) for x in list(itertools.product(*typesets))]
            real_perm_types.sort(key=lambda types: [Types[t] for t in types])
            for real_perm_type in real_perm_types:
                if real_perm_type in defined:
                    continue

                defined.append(real_perm_type)
                perm_types = []
                perm_call_args = []
                for type_idx in range(len(real_perm_type)):
                    value_type = 'Real' if real_perm_type[type_idx] in real_cast_types else real_perm_type[type_idx]
                    perm_types.append(
                        PermutationType(
                            value_type,
                            type_idx,
                            real_perm_type[type_idx],
                            type_idx
                        )
                    )
                    perm_call_args.append(type_idx)
        
                perm = Permutation(perm_types, self.types, perm_call_args, self.stack_ref)
                perm.source.append("real cast")
                permutations[",".join(real_perm_type)] = perm

                # Check for reverse
                if 'reversable' in self.tags:
                    reversed_types = [real_perm_type[1], real_perm_type[0]]
                    if not reversed_types in defined:
                        defined.append(reversed_types)
                        permutations[",".join(reversed_types)] = perm.reverse()

        return permutations



class Operator:
    included_filenames = []


    def __init__(self, capture: Capture):
        self.name = capture.op_name
        self.description = capture.op_description
        self.category = capture.op_category
        self.param_count = len(capture.call_types)

        self.calls = { ",".join(capture.call_types): Call(capture) }
        self.types = [capture.call_types]
        self.examples = capture.call_examples

        self.format_found = False
        self.format_stack_ref = False

        self.permutations = {}

        self.filenames = [capture.filename]
    

    def try_append(self, capture: Capture):
        if capture.call_types in self.types:
            return f"Cannot redefine {self.name} call with types '{', '.join(capture.call_types)}'"

        if self.param_count != len(capture.call_types):
            return f"Cannot define {self.name} with {len(capture.call_types)} args, previously defined with {self.param_count}"
        
        self.calls[",".join(capture.call_types)] = Call(capture)
        self.types.append(capture.call_types)
        self.examples.extend(capture.call_examples)
        self.filenames.append(capture.filename)

        if self.description is None:
            self.description = capture.op_description

        if self.category is None:
            self.category = capture.op_category
        
        return None


    def generate_permutations(self):
        self.types.sort(key=lambda types: [Types[t] for t in types])
        self.permutations = {}
        defined_types = self.types.copy()

        for typeset in self.types:
            typeset_perm = self.calls[",".join(typeset)].get_permutations(defined_types)
            for types in typeset_perm:
                type_names = typeset_perm[types].get_type_names()
                if type_names not in defined_types:
                    # Our original permutation will already be in the defined types list
                    defined_types.append(type_names)
            self.permutations.update(typeset_perm)
    

    def build(self):
        lines = [f'/* Operator {self.name} */', '']

        included = False
        self.filenames.sort()
        for filename in self.filenames:
            if not filename in self.__class__.included_filenames:
                lines.append(f"#include \"{filename}\"")
                self.__class__.included_filenames.append(filename)
                included = True
        
        if included:
            lines.append('')
            
        lines.append(f"Result<> OP_Eval_{self.name}(RPNStack& stack, const Operator& op) {{")

        perm_types = [perm.get_type_names() for _name, perm in self.permutations.items()]
        perm_types.sort(key=lambda types: [Types[t] for t in types])
            
        if self.param_count == 0:
            call = self.calls['']
            lines.append("UNUSED(op);")
            if call.stack_ref:
                lines.append(f"\tResult<Value> res = OP0S_{self.name}(stack);")
            else:
                lines.append(f"\tResult<Value> res = OP0_{self.name}();")
            
            if 'no_expr' in call.tags:
                lines.append("\tbool expression = false;")
            else:
                lines.append("\tbool expression = true;")
            
            lines.append("")
        else:
            param_string = "paramters" if self.param_count > 1 else "parameter"
            lines.extend([
                f"\tstd::vector<Type> types = stack.peek_types_vec({self.param_count});",
                '',
                '\tif (types.empty()) {',
                f'\t\treturn Err(ERR_INVALID_PARAM, "{self.name} op requires {self.param_count} {param_string}");',
                '\t}',
                '',
                "\tbool expression = true;",
                "\tResult<Value> res = Ok(Value());",
                "",
                "\tauto it = std::find(op.allowed_types.begin(), op.allowed_types.end(), types);",
                "\tif (it == op.allowed_types.end()) {",
                f'\t\treturn Err(ERR_INVALID_PARAM, "{self.name} op does not recognize " + stack.display_types({self.param_count}));',
                "\t}",
                "\tsize_t index = std::distance(op.allowed_types.begin(), it);",
                f"\tstd::vector<StackItem> values = stack.pop_items({self.param_count});",
                "",
                "\tswitch (index) {"
            ])

            for index in range(len(perm_types)):
                perm_type_string = "_".join(perm_types[index])
                perm = self.permutations[",".join(perm_types[index])]
                source_str = ', '.join(perm.source)
                lines.extend([
                    f'\t\t// {perm_type_string} {f"[{source_str}]" if len(perm.source) > 0 else ""}',
                    f'\t\tcase {index}: {{'
                ])
                lines.extend(perm.build(self.name, self.calls[",".join(perm.function_types)].tags))
                lines.append('\t\t}')

            lines.extend([
                '\t\tdefault:',
                '\t\t\tUNREACHABLE();',
                '\t}',
                '',
            ])

        lines.append('\tif (!res) {')

        if self.param_count > 0:
            lines.append('\t\tstack.push_items(std::move(values));')
        
        lines.extend([
            '\t\treturn res.unwrap_err();',
            '\t}',
            '',
            '\tValue value = res.unwrap_move(std::move(res));',
            '',
            '\tstack.push_item(StackItem {'
        ])
    
        if self.format_stack_ref:
            lines.append(f"\t\tOPS_FormatInput_{self.name}(stack, {', '.join([f'values[{idx}]' for idx in range(self.param_count)])}),")
        else:
            lines.append(f"\t\tOP_FormatInput_{self.name}({', '.join([f'values[{idx}]' for idx in range(self.param_count)])}),")
        
        lines.extend([
            "\t\tstd::move(value),",
            "\t\texpression",
            "\t});",
            '',
            "\treturn Ok();",
            "}",
            '',
        ])

        allowed_types_name = "{}"
        examples_name = "{}"

        if self.param_count > 0 or len(self.examples) > 0:
            lines.extend([
                f'namespace OP_{self.name}_Data {{',
                '',
            ])

            if self.param_count > 0:
                lines.extend([
                    'namespace AllowedTypes {',
                    ''
                ])

                for typeIndex in range(len(perm_types)):
                    typeSet = perm_types[typeIndex]
                    typeString = ',\n\t'.join([f'TYPE_{type.upper()}' for type in typeSet])

                    lines.extend([
                        f'constexpr Type _{typeIndex}[] {{',
                        f"\t{typeString}",
                        '};',
                        ''
                    ])
            
                typeString = ',\n\t'.join([f'AllowedTypes::_{index}' for index in range(len(perm_types))])

                lines.extend([
                    '}',
                    '',
                    f'constexpr const std::span<const Type> allowed_types[] {{',
                    f'\t{typeString}',
                    '};',
                    '', 
                ])

                allowed_types_name = f'OP_{self.name}_Data::allowed_types'
            

            if len(self.examples) > 0:
                lines.extend([
                    'namespace Examples {',
                    ''
                ])

                for exampleIndex in range(len(self.examples)):
                    example = self.examples[exampleIndex]
                    exampleString = ',\n\t'.join([f'"{e}"' for e in example])

                    lines.extend([
                        f'constexpr const char* _{exampleIndex}[] {{',
                        f'\t{exampleString}',
                        '};',
                        ''
                    ])
                
                exampleString = ',\n\t'.join([f'Examples::_{index}' for index in range(len(self.examples))])

                lines.extend([
                    '}',
                    '',
                    f'constexpr const std::span<const char* const> examples[] {{',
                    f'\t{exampleString}',
                    '};',
                    ''
                ])

                examples_name = f'OP_{self.name}_Data::examples'
            
            lines.extend([
                '}',
                '',
            ])

        lines.extend([
            f"constexpr Operator OP_{self.name} {{",
            f'\t"{self.name}",',
            f'\t"{self.description}",',
            f'\t{self.param_count or 0},',
            f'\t{allowed_types_name},',
            f'\t{examples_name},',
            f'\t&OP_Eval_{self.name},',
            '};',
            ''
        ])

        return lines



class OperatorMapBuilder:
    State = Enum('State', ['WAITING', 'CAPTURING', 'ERROR'])


    def __init__(self):
        self.state = self.State.WAITING

        self.error = ""
        self.line_no = 1
        self.filename = ""

        self.current_capture = None

        self.operators = {}
        self.operator_requires = ["<algorithm>", "<iterator>"]

        self.categories = {}

        self.manual_impl_count = 0
        self.total_impl_count = 0
    

    def process_file(self, path):
        self.filename = str(path)
        self.line_no = 1
        with open(path, 'r') as file:
            for line in file:
                self._process_line(line.rstrip())
    
    
    def get_error(self):
        if self.state == self.State.ERROR:
            return self.error

        return None


    def build(self, env):
        if self.state == self.State.ERROR:
            return

        # Make sure all operators have a description, a format function, etc...
        # Then, generate permutations for casts and reverses
        self._finalize_operators()

        if self.state == self.State.ERROR:
            return
        
        lines = [
            "/* THIS FILE IS GENERATED DO NOT EDIT */", "",
            "#include \"operators.h\"",
            "#include \"operators_internal.gen.h\"", ""
            '#include "core/comparison.h"', ''
        ]

        self.operator_requires.sort()
        lines.extend([f'#include {requires}' for requires in self.operator_requires])
        if len(self.operator_requires) > 0:
            lines.append('')
        
        lines.extend(["namespace RCalc::Operators {", ""])

        operators = list(self.operators.keys())
        operators.sort()

        for op_name in operators:
            lines.extend(self.operators[op_name].build())
            self.manual_impl_count += len(self.operators[op_name].calls)
            self.total_impl_count += len(self.operators[op_name].permutations)
        
        lines.extend([
            '}',
            '',
            'namespace RCalc {',
            '',
            'namespace OperatorCategories {',
            '',
            f'constexpr Operator const * NoCategoryOperators[] {{'
        ])

        lines.append(',\n'.join([f'\t&Operators::OP_{op_name}' for op_name in self.categories[None]]))

        lines.extend([
            '};',
            'constexpr OperatorCategory NoCategory {',
            '\tstd::nullopt,',
            '\tNoCategoryOperators',
            '};',
            ''
        ])

        category_names = list(self.categories.keys())
        category_names.remove(None)
        category_names.sort(key=lambda e: e.lower())
        for category_name in category_names:
            lines.append(f'constexpr Operator const * {category_name}CategoryOperators[] {{')
            lines.append(',\n'.join([f'\t&Operators::OP_{op_name}' for op_name in self.categories[category_name]]))
            lines.extend([
                '};',
                f'constexpr OperatorCategory {category_name}Category {{',
                f'\t"{category_name}",',
                f'\t{category_name}CategoryOperators',
                '};',
                ''
            ])

        category_count = len(category_names) + 1

        lines.append(f'constexpr OperatorCategory const * const alphabetical_categories[] {{')

        category_lines = [
            '\t&OperatorCategories::NoCategory'
        ]
        category_lines.extend([f'\t&OperatorCategories::{category_name}Category' for category_name in category_names])

        lines.append(',\n'.join(category_lines))

        lines.extend([
            '};',
            '',
            '}',
            '',
            'const std::span<OperatorCategory const * const> OperatorMap::get_alphabetical() const {',
            '\treturn OperatorCategories::alphabetical_categories;',
            '}',
            '',
            f'size_t OperatorMap::stat_manual_impl_count = {self.manual_impl_count};',
            f'size_t OperatorMap::stat_total_impl_count = {self.total_impl_count};',
            ''
            '}',
            ''
        ])

        if env["gperf_path"] == '':
            lines.extend(self._build_std_map(operators))
        else:
            lines.extend(self._build_gperf_map(operators, env["gperf_path"]))
        
        return lines

    
    def _filter_name(self, name):
        # Only allow lowercase letters, numbers, and underscores
        lower = name.lower()
        allowed = list(ascii_lowercase + digits + '_')
        return ''.join([c for c in lower if c in allowed])
    

    def _set_error(self, error):
        self.state = self.State.ERROR
        self.error = f"Error at {self.filename}:{self.line_no}\n\t{error}"
    

    def _process_line(self, line):
        match self.state:
            case self.State.WAITING:
                self._process_line_waiting(line)
            case self.State.CAPTURING:
                self._process_line_capturing(line)
            case self.State.ERROR:
                pass
        
        self.line_no += 1
    

    def _process_line_waiting(self, line):
        # Look for the start of an operator
        if line.startswith("// @RCalcOperator"):
            self.state = self.State.CAPTURING
            self.current_capture = Capture(self.filename)
        elif line.startswith("RCALC_OP_"):
            self.state = self.State.CAPTURING
            self.current_capture = Capture(self.filename)
            self._process_declaration(line)
        elif line.startswith("RCALC_FMT_"):
            self._process_format_declaration(line)
    

    def _process_line_capturing(self, line):
        # Capture information from the operator comments and definition

        if line.startswith("// "):
            self._process_statement(line[3:])
        elif line.startswith("RCALC_OP_"):
            self._process_declaration(line)
        else:
            self._finish_call()


    def _process_statement(self, statement):
        split = statement.split(":")
        if len(split) != 2:
            self._set_error(f"Statement '{statement} is invalid!")
            return

        statement_type = split[0]
        statement_arg = split[1].strip()
        
        if statement_type == "Description":
            self.current_capture.op_description = statement_arg
        elif statement_type == "Category":
            self.current_capture.op_category = statement_arg
        elif statement_type == "Tags":
            self._validate_tags(statement_arg)
        elif statement_type == "Requires":
            if not statement_arg in self.operator_requires:
                self.operator_requires.append(statement_arg)
        elif statement_type == "Example":
            self._validate_example(statement_arg)
        else:
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
    

    def _validate_example(self, example):
        if not example.startswith("[") or not example.endswith("]"):
            self._set_error(f"Example statement '{example}' is invalid!\n\tExpected form is [arg0, arg1, ...]")
            return
        
        brackets = False
        braces = False
        params = []
        param = []

        for ch in example[1:-1]:
            if ch == '[':
                if brackets:
                    self._set_error(f"Example statement '{example}' is invalid!\n\tMust close vector before starting new vector")
                    return
                brackets = True
            elif ch == ']':
                if not brackets:
                    self._set_error(f"Example statement '{example}' is invalid!\n\tClose bracket found with no matching open bracket")
                    return
                brackets = False
            elif ch == '{':
                if brackets:
                    self._set_error(f"Example statement '{example}' is invalid!\n\tCannot start matrix inside vector")
                    return
                if braces:
                    self._set_error(f"Example statement '{example}' is invalid!\n\tMust close matrix before starting new matrix")
                    return
                braces = True
            elif ch == '}':
                if brackets:
                    self._set_error(f"Example statement '{example}' is invalid!\n\tCannot close matrix inside vector")
                    return
                if not braces:
                    self._set_error(f"Example statement '{example}' is invalid!\n\tClose brace found with no matching open brace")
                    return
                braces = False
            elif ch == ',':
                if not (brackets or braces):
                    params.append(''.join(param).strip())
                    param = []
                    continue
            
            param.append(ch)
        
        params.append(''.join(param).strip())
        self.current_capture.call_examples.append(params)
    

    def _process_declaration(self, declaration):
        args_start = declaration.find("(")
        args_end = declaration.find(")")

        if args_start == -1 or args_end == -1:
            self._set_error(f'Operator declaration {declaration} is invalid!\n\tMissing parenthesis around function params')
            return
    
        # RCALC_OP_2_S(...)
        #          ^^^      // Extracting this part
        param_ref_stmt = declaration[9:args_start]

        if param_ref_stmt.endswith("_S"):
            self.current_capture.call_stack_ref = True
            param_ref_stmt = param_ref_stmt[:-2]
        
        if not param_ref_stmt.isdigit():
            self._set_error(f'Operator declaration {declaration} is invalid!\n\tOperator count {param_ref_stmt} is not a number')
            return
        
        param_count = int(param_ref_stmt)
        expected_arg_count = 2 * param_count + 1

        args = [arg.strip() for arg in declaration[args_start+1:args_end].split(",")]
        if len(args) != expected_arg_count:
            self._set_error(f'Operator declaration {declaration} is invalid!\n\tArgument count does not match declared parameters')
            return

        self.current_capture.op_name = args[0]

        for type in args[1:param_count+1]:
            if type in Types:
                self.current_capture.call_types.append(type)
            else:
                self._set_error(f'Operator declaration {declaration} is invalid!\n\tType {type} is invalid!')
                return
    
    def _finish_call(self):
        if self.current_capture.op_name in self.operators:
            append_err = self.operators[self.current_capture.op_name].try_append(self.current_capture)
            if not append_err is None:
                self._set_error(append_err)
                return
        else:
            self.operators[self.current_capture.op_name] = Operator(self.current_capture)
        
        self.state = self.State.WAITING
    

    def _process_format_declaration(self, declaration):
        args_start = declaration.find("(")
        args_end = declaration.find(")")

        if args_start == -1 or args_end == -1:
            self._set_error(f'Operator format declaration {declaration} is invalid!\n\tMissing parenthesis around function params')
            return
    
        # RCALC_FMT_2_S(...)
        #           ^^^      // Extracting this part
        param_ref_stmt = declaration[10:args_start]

        stack_ref = False

        if param_ref_stmt.endswith("_S"):
            stack_ref = True
            param_ref_stmt = param_ref_stmt[:-2]
        
        if not param_ref_stmt.isdigit():
            self._set_error(f'Operator format declaration {declaration} is invalid!\n\tOperator count {param_ref_stmt} is not a number')
            return
        
        param_count = int(param_ref_stmt)
        expected_arg_count = param_count + 1

        args = [arg.strip() for arg in declaration[args_start+1:args_end].split(",")]
        if len(args) != expected_arg_count:
            self._set_error(f'Operator format declaration {declaration} is invalid!\n\tArgument count does not match declared parameters')
            return

        op_name = args[0]

        if not op_name in self.operators:
            self._set_error(f'Operator format declaration {declaration} is invalid!\n\tOperator {op_name} is unknown')
            return

        if self.operators[op_name].param_count != param_count:
            self._set_error(f'Operator format declaration {declaration} is invalid!\n\tOperator {op_name} has {self.operators[op_name].param_count} params, not {param_count}')
            return
        
        if self.operators[op_name].format_found:
            self._set_error(f'Operator format declaration {declaration} is invalid!\n\tCannot redeclare format for operator {op_name}')
            return

        self.operators[op_name].format_found = True
        self.operators[op_name].format_stack_ref = stack_ref
    

    def _finalize_operators(self):
        # Make sure all operators have a description and a format function
        # Also make sure reversable calls have two parameters, and that all example counts match
        for op_name, operator in self.operators.items():
            if operator.description is None:
                self._set_error(f'Operator {op_name} is invalid!\n\tOperator is missing description')
                return
            elif not operator.format_found:
                self._set_error(f'Operator {op_name} is invalid!\n\tOperator is missing format function')
                return
            
            for _types, call in operator.calls.items():
                if 'reversable' in call.tags and len(call.types) != 2:
                    self._set_error(f'Operator {op_name} is invalid!\n\t\'reversable\' tag is only allowed for operators with two parameters')
                    return
            
            operator.generate_permutations()

            # Track operator categories
            if operator.category in self.categories:
                self.categories[operator.category].append(op_name)
            else:
                self.categories[operator.category] = [op_name]
            
            if len(operator.examples) == 0 and operator.param_count > 0:
                print(f"Notice: Operator '{operator.name}' does not have any examples.")
        
        for category_name in self.categories:
            self.categories[category_name].sort(key=lambda e: e.lower())
    

    def _build_std_map(self, operators: list[str]):
        lines = [
            '// Using std_map',
            '',
            '#include <map>',
            '',
            'namespace RCalc {',
            '',
            'std::map<std::string, Operator const * const> operator_map;',
            '',
            'void OperatorMap::build() {',
            '\tif (built) { return; }',
        ]

        operators.sort(key=lambda e: e.lower())
        lines.extend([f'\toperator_map.emplace("{self._filter_name(op_name)}", &Operators::OP_{op_name});' for op_name in operators])
        
        lines.extend([
            '\tbuilt = true;',
            '}',
            '',
            'bool OperatorMap::has_operator(const std::string& str) {',
            '\tif (!built) { build(); }',
            '\treturn operator_map.contains(str);',
            '}',
            '',
            'Result<> OperatorMap::evaluate(const std::string& str, RPNStack& stack) {',
            '\tif (!built) { build(); }',
            '\tconst Operator& op = *operator_map.at(str);',
            '\treturn op.evaluate(stack, op);',
            '}',
            '',
            '}'
        ])

        return lines
    

    def _build_gperf_map(self, operators: list[str], gperf_path: str):
        lines = [
            '// Using gperf',
            '',
            '#include <cstring>',
            '#include "core/filter.h"',
            '#ifndef NDEBUG',
            '#include <cassert>',
            '#endif',
            '',
            'namespace RCalc {',
            '',
            'namespace Operators::GPerf {',
            ''
        ]

        operators.sort(key=lambda e: e.lower())

        # Write operators list to temporary file and run gperf
        with tempfile.NamedTemporaryFile("w", newline='\n', delete=False) as op_file:
            atexit.register(lambda file: os.unlink(os.path.realpath(file.name)), op_file)
            for op_name in operators:
                op_file.write(f'{self._filter_name(op_name)}\n')
            op_file.flush()
            op_file.flush()
            proc = subprocess.run(
                [
                    gperf_path,
                    "--language=ANSI-C",
                    "--compare-lengths",
                    "--compare-strncmp",
                    "--readonly-tables",
                    "--switch=4",
                    "--multiple-iterations=10",
                    os.path.realpath(op_file.name)
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
            'Operator const* operator_map[MAX_HASH_VALUE-MIN_HASH_VALUE+1] = {};',
            '',
            'void OperatorMap::build() {',
            '\tif (built) { return; }',
            '#ifndef NDEBUG',
            '\tstd::vector<bool> dbg_vec;',
            '\tdbg_vec.resize(MAX_HASH_VALUE-MIN_HASH_VALUE+1, false);',
            '#endif',
            '\tfor (const OperatorCategory* category : get_alphabetical()) {',
            '\t\tfor (const Operator* op : category->category_ops) {',
            '\t\t\tstd::string op_name = filter_name(op->name);',
            '\t\t\toperator_map[Operators::GPerf::hash(op_name.c_str(), strlen(op->name)) - MIN_HASH_VALUE] = op;',
            '#ifndef NDEBUG',
            '\t\t\tdbg_vec[Operators::GPerf::hash(op_name.c_str(), strlen(op->name)) - MIN_HASH_VALUE] = true;',
            '#endif',
            '\t\t}',
            '\t}',
            '#ifndef NDEBUG',
            '\tif (std::count(dbg_vec.begin(), dbg_vec.end(), true) != TOTAL_KEYWORDS) {',
            '\t\tthrow std::logic_error("OperatorMap Assert Failed: GPerf hash did not reach every keyword.");',
            '\t}',
            '#endif',
            '\tbuilt = true;',
            '}',
            '',
            'bool OperatorMap::has_operator(const std::string& str) {',
            '\tif (!built) { build(); }',
            '\treturn Operators::GPerf::in_word_set(str.c_str(), str.size()) != nullptr;',
            '}',
            '',
            'Result<> OperatorMap::evaluate(const std::string& str, RPNStack& stack) {',
            '\tif (!built) { build(); }',
            '\tconst Operator& op = *operator_map[Operators::GPerf::hash(str.c_str(), str.size()) - MIN_HASH_VALUE];',
            '\treturn op.evaluate(stack, op);',
            '}',
            '',
            '}'
        ])

        return lines


def make_operators_map(target, source, env):
    dst = target[0]
    builder = OperatorMapBuilder()

    for file in source:
        builder.process_file(file)

    built = builder.build(env)
    
    if builder.get_error() is not None:
        print(builder.get_error())
        return 255
    
    with open(dst, "w") as file:
        file.write("\n".join(built))
    
    return 0


def make_internal_header(target, src, env):
    dst = target[0]

    MAX_PARAM_COUNT = 10

    lines = [
        '/* THIS FILE IS GENERATED DO NOT EDIT */',
        '',
        '#pragma once',
        '',
        '#include "core/error.h"',
        '#include "app/stack.h"',
        ''
    ]

    for count in range(MAX_PARAM_COUNT + 1):
        macro_def = f"{''.join([f', type_{idx}' for idx in range(count)])}{''.join([f', arg_{idx}' for idx in range(count)])}"
        func_name = ''.join([f'##type_{idx}##_' for idx in range(count)])
        func_args = ', '.join([f'type_{idx} arg_{idx}' for idx in range(count)])
        fmt_def = ''.join([f', arg_{idx}' for idx in range(count)])
        fmt_args = ', '.join([f'const StackItem& arg_{idx}' for idx in range(count)])

        lines.extend([
            f'/* {count} Parameter{"s" if count != 1 else ""} */',
            '',
            f'#define RCALC_OP_{count}(name{macro_def}) Result<Value> OP{count}_{func_name}##name({func_args})',
            f'#define RCALC_OP_{count}_S(name{macro_def}) Result<Value> OP{count}S_{func_name}##name(RPNStack& stack{", " if count > 0 else ""}{func_args})',
            '',
            f'#define RCALC_FMT_{count}(name{fmt_def}) std::shared_ptr<RCalc::Displayable> OP_FormatInput_##name({fmt_args})',
            f'#define RCALC_FMT_{count}_S(name{fmt_def}) std::shared_ptr<RCalc::Displayable> OPS_FormatInput_##name(RPNStack& stack{", " if count > 0 else ""}{fmt_args})',
            ''
        ])

    with open(dst, 'w') as file:
        file.write('\n'.join(lines))


if __name__ == "__main__":
    subprocess_main(globals())
