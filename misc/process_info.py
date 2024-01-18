#!python3

import json
import sys

class Processor:
    def __init__(self, filename):
        with open(filename, 'r') as file:
            self.data = json.loads(file.read())
    

    def dump_operators(self, filename):
        lines = [
            '# Operators',
            '',
            f"as of RCalc version {self.data['general']['version']} ({self.data['general']['git_hash'][:6]})",
            '',
        ]

        for category in self.data['operator_categories']:
            if not category['name'] is None:
                lines.extend([
                    f"## {category['name']} Operators",
                    ''
                ])
            
            for operator in category['operators']:

                lines.extend([
                    f"#### {operator['name']}",
                    '',
                    f"- {operator['description']}"
                ])

                if len(operator['allowed_types']) > 0:
                    argument_str = 'argument' if operator['param_count'] == 1 else 'arguments'
                    lines.append(f"- [Accepts {operator['param_count']} {argument_str}](#{operator['name'].lower()}-1)")

                if len(operator['examples']) > 0:
                    lines.append(f"- [Examples](#{operator['name'].lower()}-2)")
                
                lines.append('')
        
        lines.extend([
            '## Allowed Types',
            ''
        ])

        for category in self.data['operator_categories']:
            for operator in category['operators']:
                allowed_types = operator['allowed_types']
                if len(allowed_types) == 0:
                    continue
                
                lines.extend([
                    f"#### {operator['name']}",
                    ''
                ])

                first = True
                for call in allowed_types:
                    call_types = ', '.join(call)

                    if first:
                        first = False
                    else:
                        call_types = 'or ' + call_types
                    
                    lines.append(f"  - {call_types}")

                lines.extend([
                    f"  - [Go Back](#{operator['name'].lower()})",
                    ''
                ])
        
        lines.extend([
            '## Examples',
            ''
        ])

        for category in self.data['operator_categories']:
            for operator in category['operators']:
                examples = operator['examples']
                if len(examples) == 0:
                    continue
                
                lines.extend([
                    f"#### {operator['name']}",
                    ''
                ])

                for index in range(len(examples)):
                    example = examples[index]

                    parameter_str = 'Parameter' if len(example['params']) == 1 else 'Parameters'

                    lines.extend([
                        f"- Example {index + 1}",
                        f"  - {parameter_str}"
                    ])

                    lines.extend([f"    - {param}" for param in example['params']])

                    if len(example['outputs']) == 1:
                        lines.extend([
                            '  - Output',
                            f"    - {example['outputs'][0]}"
                        ])
                    else:
                        lines.append('  - Outputs')
                        lines.extend([f"    - {output}" for output in example['outputs']])

                lines.extend([
                    '',
                    f"[Go Back](#{operator['name'].lower()})",
                    ''
                ])
        
        with open(filename, 'w') as file:
            file.write('\n'.join(lines))
            
    

    def dump_commands(self, filename):
        lines = [
            '# Commands',
            '',
            f"as of RCalc version {self.data['general']['version']} ({self.data['general']['git_hash'][:6]})",
            '',
        ]

        for scope in self.data['command_scopes']:
            lines.extend([
                f"## {scope['name']} Commands",
                ''
            ])

            for command in scope['commands']:
                lines.extend([
                    f"- {command['name']}",
                    f"  - {command['description']}",
                ])

                if len(command['aliases']) == 0:
                    continue
                elif len(command['aliases']) == 1:
                    lines.append(f"  - Alias: {command['aliases'][0]}")
                else:
                    aliases = ', '.join(command['aliases'])
                    lines.append(f"  - Aliases: [{aliases}]")

                lines.append('')
        
        with open(filename, 'w') as file:
            file.write('\n'.join(lines))
    

    def dump_units(self, filename):
        lines = [
            '# Unit Families',
            '',
            f"as of RCalc version {self.data['general']['version']} ({self.data['general']['git_hash'][:6]})",
            '',
        ]

        for family in self.data['unit_families']:
            lines.extend([
                f"## {family['name']} Units",
                '',
                f"Base type: {family['base_type']}",
                ''
            ])

            lines.extend([f"-  {unit['name']} (Usage: {unit['usage']})" for unit in family['units']])
            lines.append('')
        
        with open(filename, 'w') as file:
            file.write('\n'.join(lines))

    

    def get_stats(self):
        lines = [
            f"Statistics for RCalc version {self.data['general']['version']} ({self.data['general']['git_hash'][:6]})",
            f"operator_category_count: {self.data['statistics']['operator_category_count']}",
            f"operator_count: {self.data['statistics']['operator_count']}",
            f"operator_manual_impl_count: {self.data['statistics']['operator_manual_impl_count']}",
            f"operator_total_impl_count: {self.data['statistics']['operator_total_impl_count']}",
            f"command_scope_count: {self.data['statistics']['command_scope_count']}",
            f"command_count: {self.data['statistics']['command_count']}",
            f"unit_family_count: {self.data['statistics']['unit_family_count']}",
            f"unit_count: {self.data['statistics']['unit_count']}"
        ]

        print('\n'.join(lines))


def run():    
    if sys.argv[1] == "--help" or sys.argv[1] == "-h":
        print_help()
        return

    if len(sys.argv) < 3:
        print_help()
        sys.exit(255)
    
    command = sys.argv[1]

    if command == "dump_operators":
        if len(sys.argv) < 4:
            print("Command 'dump_operators' requires an output parameter!")
            sys.exit(255)
        
        processor = Processor(sys.argv[2])
        processor.dump_operators(sys.argv[3])
    elif command == "dump_commands":
        if len(sys.argv) < 4:
            print("Command 'dump_commands' requires an output parameter!")
            sys.exit(255)
        
        processor = Processor(sys.argv[2])
        processor.dump_commands(sys.argv[3])
    elif command == "dump_units":
        if len(sys.argv) < 4:
            print("Command 'dump_units' requires an output parameter!")
            sys.exit(255)
        
        processor = Processor(sys.argv[2])
        processor.dump_units(sys.argv[3])
    elif command == "get_stats":
        processor = Processor(sys.argv[2])
        processor.get_stats()
    elif command == "dump_all":
        processor = Processor(sys.argv[2])
        processor.dump_commands("Commands.md")
        processor.dump_operators("Operators.md")
        processor.dump_units("Units.md")
    else:
        print(f"Invalid command: '{command}'")
        sys.exit(255)


def print_help():
    print("# process_info.py")
    print("Converts RCalc info dumps from JSON into Markdown")
    print("")
    print("Usage: python3 process_info.py <command> <input> [output]")
    print("")
    print("Example: python3 process_info.py dump_operators rcalc_dump.json operators.md")
    print("")
    print("Commands:")
    print("  dump_commands          Generate a listing of compiled commands.")
    print("  dump_operators         Generate a listing of compiled operators.")
    print("  dump_units             Generate a listing of compiled units.")
    print("  dump_all               Dump commands, operators, and units.")
    print("  get_stats              Print the dumped statistics to stdout.")
    print("")


if __name__ == "__main__":
    run()