from enum import Enum
from string import ascii_lowercase, digits
import subprocess
import tempfile
import os
import csv
from platform_methods import subprocess_main


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
    

    def try_update(self, capture: Capture):
        if self.family != capture.family_name:
            return f"Unit '{self.name}' belongs to family '{self.family}', not '{capture.family_name}'!"
        if self.usage != capture.unit_usage:
            return f"Unit '{self.name}' is defined with usage '{self.usage}', not '{capture.unit_usage}'!"
        
        self.found_from |= capture.type == Capture.Type.From
        self.found_to |= capture.type == Capture.Type.To

        return None
    

    def build(self, family_name, family_type):
        lines = [
            f'UnitImpl<{family_type}> UNITDEF_{self.usage} {{',
            '\t{',
            f'\t\t"{self.name}",',
            f'\t\t"_{self.usage}",',
            '\t\tnullptr,',
            '\t\tnullptr',
            '\t},',
            f'\t&UNIT_BASE_{family_name}_FROM_{self.usage},',
            f'\t&UNIT_BASE_{family_name}_TO_{self.usage}',
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
    

    def try_update(self, capture: Capture):
        if self.family_unit_name is None:
            self.family_unit_name = capture.family_unit_name
        elif (not capture.family_unit_name is None) and self.family_unit_name != capture.family_unit_name:
            return f"Unit family '{self.name}' is defined with base name '{self.family_unit_name}', not '{capture.family_unit_name}'!"
        
        if self.family_type is None:
            self.family_type = capture.family_type
        elif (not capture.family_type is None) and self.family_type != capture.family_type:
            return f"Unit family '{self.name}' is defined with base type '{self.family_type}', not '{capture.family_type}'!"
        
        if capture.unit_name in self.units:
            error = self.units[capture.unit_name].try_update(capture)
            if not error is None:
                return error
        else:
            self.units[capture.unit_name] = Unit(capture)
        
        if not capture.filename in self.filenames:
            self.filenames.append(capture.filename)
        
        return None
    

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
            f'UnitImpl<{self.family_type}> UNITDEF_{self.family_unit_usage} {{',
            '\t{',
            f'\t\t"{self.family_unit_name}",',
            f'\t\t"_{self.family_unit_usage}",',
            '\t\tnullptr,',
            '\t\tnullptr',
            '\t},',
            f'\t&UNIT_ECHO<{self.family_type}>,',
            f'\t&UNIT_ECHO<{self.family_type}>',
            '};',
            '',
            f'Unit* UNITVEC_BASE_{self.name}[] {{'
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

        # Make sure all commands have a description
        self._finalize_units()

        if self.state == self.State.ERROR:
            return

        lines = [
            "/* THIS FILE IS GENERATED DO NOT EDIT */", "",
            "#include \"units.h\"",
            "#include \"units_internal.h\"", "",
            '#include <span>', ''
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
        
        lines.append(f'constexpr UnitFamily const * alphabetical[] {{')
        lines.append(',\n'.join([f'\t&UNIT_FAMILY_{family_unit}' for family_unit in family_units]))
        
        lines.extend([
            '};',
            '',
            '}',
            '',
            'namespace RCalc {',
            ''
            'const std::span<UnitFamily const * const> UnitsMap::get_alphabetical() const {',
            '\treturn Units::alphabetical;',
            '}',
            '',
            '}',
            ''
        ])

        if env["gperf_path"] == '':
            lines.extend(self._build_std_map())
        else:
            lines.extend(self._build_gperf_map(env["gperf_path"]))

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
        if line.startswith("// @RCalcUnit"):
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
        
        # Unit names can have commas, so we have to use a complicated split
        splitter = csv.reader([declaration[args_start+1:args_end]], skipinitialspace=True)
        unsplit = [line for line in splitter][0]
        args = [arg.strip() for arg in unsplit]
        if len(args) != 4:
            self._set_error(f'Unit declaration {declaration} is invalid!\n\tThree arguments are required (Unit family name, Unit name, Unit family type, Unit usage)')
            return

        if not args[1] in Types:
            self._set_error(f'Unit declaration {declaration} is invalid!\n\tType {args[2]} is invalid!')
            return

        self.current_capture.family_name = args[0]
        self.current_capture.family_type = args[1]

        if declaration.startswith("UNIT_FAMILY"):
            self.current_capture.family_unit_name = args[2]
            self.current_capture.family_unit_usage = args[3]
            self.current_capture.type = Capture.Type.Family
            self._finish_unit()
        elif declaration.startswith("UNIT_FROM_BASE"):
            self.current_capture.unit_name = args[2]
            self.current_capture.unit_usage = args[3]
            self.current_capture.type = Capture.Type.From
            self._finish_unit()
        elif declaration.startswith("UNIT_TO_BASE"):
            self.current_capture.unit_name = args[2]
            self.current_capture.unit_usage = args[3]
            self.current_capture.type = Capture.Type.To
            self._finish_unit()
    

    def _finish_unit(self):
        if self.current_capture.family_name in self.family_units:
            if self.current_capture.type == Capture.Type.Family:
                self._set_error(f"Cannot redefine unit family {self.current_capture.family_name}!")
                return
            
            error = self.family_units[self.current_capture.family_name].try_update(self.current_capture)
            if not error is None:
                self._set_error(error)
                return
        else:
            self.family_units[self.current_capture.family_name] = Family(self.current_capture)
        
        self.state = self.State.WAITING
    

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
                    self._set_error(f'Unit {unit_name} is invalid!\n\tUnit is missing from_base function!')
                    return
                if not unit.found_to:
                    self._set_error(f'Unit {unit_name} is invalid!\n\tUnit is missing to_base function!')
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
    

    def _build_gperf_map(self, gperf_path: str):
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
            'namespace Units::GPerf {',
            ''
        ]

        units = []
        for family_name in self.family_units:
            units.extend([self.family_units[family_name].units[unit_name].usage for unit_name in self.family_units[family_name].units])
            units.append(self.family_units[family_name].family_unit_usage)

        units.sort()

        # Write units list to temporary file and run gperf
        with tempfile.NamedTemporaryFile("w", newline='\n') as unit_file:
            for unit in units:
                unit_file.write(f'_{unit}\n')
            unit_file.flush()
            unit_file.flush()
            proc = subprocess.run(
                [
                    gperf_path,
                    "--language=ANSI-C",
                    "--compare-lengths",
                    "--compare-strncmp",
                    "--readonly-tables",
                    "--switch=4",
                    "--multiple-iterations=10",
                    os.path.realpath(unit_file.name)
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
            'Unit const* units_map[MAX_HASH_VALUE-MIN_HASH_VALUE+1] = {};',
            '',
            'void UnitsMap::build() {',
            '\tif (built) { return; }',
            '#ifndef NDEBUG',
            '\tstd::vector<bool> dbg_vec;',
            '\tdbg_vec.resize(MAX_HASH_VALUE-MIN_HASH_VALUE+1, false);',
            '#endif',
            '\tfor (const UnitFamily* family : get_alphabetical()) {',
            '\t\tfor (const Unit* unit : family->units) {',
            '\t\t\tunits_map[Units::GPerf::hash(unit->p_usage, strlen(unit->p_usage)) - MIN_HASH_VALUE] = unit;',
            '#ifndef NDEBUG',
            '\t\t\tdbg_vec[Units::GPerf::hash(unit->p_usage, strlen(unit->p_usage)) - MIN_HASH_VALUE] = true;',
            '#endif',
            '\t\t}',
            '\t}',
            '#ifndef NDEBUG',
            '\tif (std::count(dbg_vec.begin(), dbg_vec.end(), true) != TOTAL_KEYWORDS) {',
            '\t\tthrow std::logic_error("UnitsMap Assert Failed: GPerf hash did not reach every keyword.");',
            '\t}',
            '#endif'
        ])

        lines.append('')
        lines.extend([f'\tUnits::UNIT_FAMILY_{family}.setup();' for family in self.family_units])
        lines.append('')

        lines.extend([
            '\tbuilt = true;',
            '}',
            '',
            'std::optional<const Unit*> UnitsMap::find_unit(const std::string& str) {',
            '\tif (!built) { build(); }',
            '\tif (Units::GPerf::in_word_set(str.c_str(), str.size()) != nullptr) {',
            '\t\treturn units_map[Units::GPerf::hash(str.c_str(), str.size()) - MIN_HASH_VALUE];',
            '\t}',
            '\telse { return std::nullopt; }',
            '}',
            '',
            '}'
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


if __name__ == "__main__":
    subprocess_main(globals())
