def get_priority():
    return 1


def is_available():
    return True


def configure(env: "Environment"):
    env["enabled_command_scopes"].append("RendererTemplate")