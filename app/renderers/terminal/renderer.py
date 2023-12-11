import sys

def get_priority():
    return 1


def is_available():
    return True


def meta():
    return {
        'header': 'app/renderers/terminal/terminal_renderer.h',
        'class': 'TerminalRenderer'
    }


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