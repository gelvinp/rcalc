import sys

def get_priority():
    return 1


def is_available(env: "Environment"):
    return True


def meta():
    return {
        'header': 'app/renderers/terminal/terminal_renderer.h',
        'class': 'TerminalRenderer'
    }


def get_opts(_env):
    from SCons.Variables import BoolVariable

    return [
        BoolVariable("enable_terminal_clipboard", "Enable clipboard support for the terminal renderer, requiring a desktop manager on linux.", True),
    ]


def configure(env: "Environment"):
    env["enabled_command_scopes"].append(meta()['class'])

    if env["platform"] == "win" and env["windows_subsystem"] == "gui":
        print("The terminal renderer does NOT work on windows with the GUI subsystem!")
        print("Either...")
        print("\tDisable the terminal renderer (enable_terminal_renderer=no)")
        print("\tOR Switch to the console renderer (windows_subsystem=console)")
        sys.exit(255)
    
    env["enabled_modules"].append("ftxui")

    if env["enable_terminal_clipboard"]:
        env["enabled_modules"].append("clip")