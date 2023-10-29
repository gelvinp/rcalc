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
    env["enabled_modules"].append("ftxui")
    env["enabled_command_scopes"].append("TerminalRenderer")