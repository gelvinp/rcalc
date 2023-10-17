from enum import Enum
from string import ascii_lowercase, digits
import subprocess
import tempfile
import os


Types = { 'Int': 0, 'BigInt': 1, 'Real': 2, 'Vec2': 3, 'Vec3': 4, 'Vec4': 5, 'Mat2': 6, 'Mat3': 7, 'Mat4': 8 }

def _filter_name(name):
    # Only allow lowercase letters, numbers, and underscores
    lower = name.lower()
    allowed = list(ascii_lowercase + digits + '_')
    return ''.join([c for c in lower if c in allowed])


class Capture:
    Type = Enum('Type', ['Family', 'From', 'To'])

    def __init__(self, filename: str):
        self.family_name = None
        self.family_unit_name = None
        self.family_unit_usage = None
        self.family_type = None

        self.unit_name = None
        self.unit_usage = None

        self.type = None

        self.filename = filename


class Unit:
    def __init__(self, capture: Capture):
        self.family = capture.family_name
        self.name = capture.unit_name
        self.usage = capture.unit_usage

        self.found_from = capture.type == Capture.Type.From
        self.found_to = capture.type == Capture.Type.To
    

    def update(self, capture: Capture):
        self.found_from |= capture.type == Capture.Type.From
        self.found_to |= capture.type == Capture.Type.To
    

    def build(self, family_name, family_type):
        lines = [
            f'Unit UNITDEF_{self.usage} {{',
            f'\t"{self.name}",',
            f'\t"_{self.usage}",',
            '\tnullptr,',
            f'\t[](Value value) {{ return Value(UNIT_BASE_{family_name}_FROM_{self.usage}(value.operator {family_type}())); }},',
            f'\t[](Value value) {{ return Value(UNIT_BASE_{family_name}_TO_{self.usage}(value.operator {family_type}())); }}',
            '};',
            ''
        ]

        return lines


class Family:
    included_filenames = []

    def __init__(self, capture: Capture):
        self.name = capture.family_name
        self.family_unit_name = capture.family_unit_name
        self.family_unit_usage = capture.family_unit_usage
        self.family_type = capture.family_type

        self.filenames = [capture.filename]

        if capture.unit_name is None:
            self.units = {}
        else:
            self.units = { capture.unit_name: Unit(capture) }
    

    def update(self, capture: Capture):
        if self.family_unit_name is None:
            self.family_unit_name = capture.family_unit_name
        if self.family_type is None:
            self.family_type = capture.family_type
        
        if capture.unit_name in self.units:
            self.units[capture.unit_name].update(capture)
        else:
            self.units[capture.unit_name] = Unit(capture)
        
        if not capture.filename in self.filenames:
            self.filenames.append(capture.filename)
    

    def build(self):
        lines = [f'/* Unit Family {self.name} */', '']

        included = False
        self.filenames.sort()
        for filename in self.filenames:
            if not filename in self.__class__.included_filenames:
                lines.append(f"#include \"{filename}\"")
                self.__class__.included_filenames.append(filename)
                included = True
        
        if included:
            lines.append('')

        units = list(self.units.keys())
        units.sort()

        for unit in units:
            lines.extend(self.units[unit].build(self.name, self.family_type))
        
        lines.extend([
            f'Unit UNITDEF_{self.family_unit_usage} {{',
            f'\t"{self.family_unit_name}",',
            f'\t"_{self.family_unit_usage}",',
            '\tnullptr,',
            '\t&UNIT_ECHO,',
            '\t&UNIT_ECHO',
            '};',
            '',
            f'std::vector<Unit*> UNITVEC_BASE_{self.name} {{'
        ])

        usages = [self.units[unit].usage for unit in units]
        usages.append(self.family_unit_usage)
        
        lines.append(',\n'.join([f'\t&UNITDEF_{usage}' for usage in usages]))

        lines.extend([
            '};',
            '',
            f'UnitFamily UNIT_FAMILY_{self.name} {{',
            f'\t"{self.name}",',
            f'\tTYPE_{self.family_type.upper()},',
            f'\tUNITVEC_BASE_{self.name}',
            '};',
            ''
        ])

        return lines


class UnitsMapBuilder:
    State = Enum('State', ['WAITING', 'CAPTURING', 'ERROR'])

    def __init__(self):
        self.state = self.State.WAITING

        self.error = ""
        self.line_no = 1
        self.filename = ""
        
        self.current_capture = None

        self.family_units = {}
        self.unit_requires = []
    

    def process_file(self, path):
        self.filename = str(path)
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

        # Make sure all commands have a description
        self._finalize_units()

        if self.state == self.State.ERROR:
            return

        lines = [
            "/* THIS FILE IS GENERATED DO NOT EDIT */", "",
            "#include \"units.h\"",
            "#include \"units_internal.h\"", "",
        ]

        self.unit_requires.sort()
        lines.extend([f'#include {requires}' for requires in self.unit_requires])
        if len(self.unit_requires) > 0:
            lines.append('')
        
        lines.extend(["namespace RCalc::Units {", ""])

        family_units = list(self.family_units.keys())
        family_units.sort()

        for family_unit in family_units:
            lines.extend(self.family_units[family_unit].build())
        
        lines.append(f'std::vector<UnitFamily const *> alphabetical {{')
        lines.append(',\n'.join([f'\t&UNIT_FAMILY_{family_unit}' for family_unit in family_units]))
        
        lines.extend([
            '};',
            '',
            '}',
            '',
            'namespace RCalc {',
            ''
            'const std::vector<UnitFamily const*>& UnitsMap::get_alphabetical() const {',
            '\treturn Units::alphabetical;',
            '}',
            '',
            '}',
            ''
        ])

        lines.extend(self._build_std_map())

        return lines
    
    
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
        # Look for the start of a command
        if line.startswith("// @Unit"):
            self.state = self.State.CAPTURING
            self.current_capture = Capture(self.filename)
        elif line.startswith("UNIT_"):
            self.current_capture = Capture(self.filename)
            self._process_declaration(line)
            
    
    def _process_line_capturing(self, line):
        # Capture information from the command comments and definition
        if line.startswith("// "):
            self._process_statement(line[3:])
        elif line.startswith("UNIT_"):
            self._process_declaration(line)
    
    
    def _process_statement(self, statement):
        split = statement.split(":")
        if len(split) != 2:
            self._set_error(f"Statement '{statement} is invalid!")
            return

        statement_type = split[0]
        statement_arg = split[1].strip()

        match statement_type:
            case "Requires":
                if not statement_arg in self.unit_requires:
                    self.unit_requires.append(statement_arg)
            case _:
                self._set_error(f"Statement type '{statement_type}' is unknown!")
    

    def _process_declaration(self, declaration):
        args_start = declaration.find("(")
        args_end = declaration.find(")")

        if args_start == -1 or args_end == -1:
            self._set_error(f'Unit declaration {declaration} is invalid!\n\tMissing parenthesis around function params')
            return

        args = [arg.strip() for arg in declaration[args_start+1:args_end].split(",")]
        if len(args) != 4:
            self._set_error(f'Unit declaration {declaration} is invalid!\n\tThree arguments are required (Unit family name, Unit name, Unit family type, Unit usage)')
            return
        
        if args[1][0] != '"' or args[1][-1] != '"':
            self._set_error(f'Unit declaration {declaration} is invalid!\n\tUnit name must be in double quotes!')
            return

        if not args[2] in Types:
            self._set_error(f'Unit declaration {declaration} is invalid!\n\tType {args[2]} is invalid!')
            return

        self.current_capture.family_name = args[0]
        self.current_capture.family_type = args[2]

        if declaration.startswith("UNIT_FAMILY"):
            self.current_capture.family_unit_name = args[1][1:-1]
            self.current_capture.family_unit_usage = args[3]
            self.current_capture.type = Capture.Type.Family
            self._finish_unit()
        elif declaration.startswith("UNIT_FROM_BASE"):
            self.current_capture.unit_name = args[1][1:-1]
            self.current_capture.unit_usage = args[3]
            self.current_capture.type = Capture.Type.From
            self._finish_unit()
        elif declaration.startswith("UNIT_TO_BASE"):
            self.current_capture.unit_name = args[1][1:-1]
            self.current_capture.unit_usage = args[3]
            self.current_capture.type = Capture.Type.To
            self._finish_unit()
    

    def _finish_unit(self):
        if self.current_capture.family_name in self.family_units:
            self.family_units[self.current_capture.family_name].update(self.current_capture)
        else:
            self.family_units[self.current_capture.family_name] = Family(self.current_capture)
    

    def _finalize_units(self):
        # Make sure every family and unit has a name and usage, and every unit has a from and to function
        for family_name, family in self.family_units.items():
            if family.family_unit_name is None:
                self._set_error(f'Unit family {family_name} is invalid!\n\tFamily is missing definition!')
                return
            if family.family_unit_usage is None:
                self._set_error(f'Unit family {family_name} is invalid!\n\tFamily is missing family unit usage!')
                return
            
            for unit_name, unit in family.units.items():
                if not unit.found_from:
                    self._set_error(f'Unit {unit_name} is invalid!\n\tFamily is missing from_family function!')
                    return
                if not unit.found_to:
                    self._set_error(f'Unit {unit_name} is invalid!\n\tFamily is missing to_family function!')
                    return
    

    def _build_std_map(self):
        lines = [
            '// Using std_map',
            '',
            '#include <map>',
            '',
            'namespace RCalc {',
            '',
            'std::map<std::string, Unit const * const> units_map;',
            '',
            'void UnitsMap::build() {',
            '\tif (built) { return; }',
        ]

        units = []
        for family_name in self.family_units:
            units.extend([self.family_units[family_name].units[unit_name].usage for unit_name in self.family_units[family_name].units])
            units.append(self.family_units[family_name].family_unit_usage)

        units.sort()
        lines.extend([f'\tunits_map.emplace("_{unit}", &Units::UNITDEF_{unit});' for unit in units])

        lines.append('')
        lines.extend([f'\tUnits::UNIT_FAMILY_{family}.setup();' for family in self.family_units])
        lines.append('')
        
        lines.extend([
            '\tbuilt = true;',
            '}',
            '',
            'std::optional<const Unit*> UnitsMap::find_unit(const std::string& str) {',
            '\tif (!built) { build(); }',
            '\tif (units_map.contains(str)) {',
            '\t\treturn units_map.at(str);',
            '\t}',
            '\telse { return std::nullopt; }',
            '}',
            '',
            '}',
            ''
        ])

        return lines
    

def make_units_maps(target, source, env):
    dst = target[0]
    builder = UnitsMapBuilder()

    for file in source:
        builder.process_file(file)

    built = builder.build(env)
    
    if builder.get_error() is not None:
        print(builder.get_error())
        return 255
    
    with open(dst, "w") as file:
        file.write("\n".join(built))
    
    return 0
