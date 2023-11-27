def get_priority():
    return 1


def is_available():
    return True


def meta():
    return {
        'header': 'app/renderers/template/renderer_template.h',
        'class': 'RendererTemplate'
    }


def configure(env: "Environment"):
    env["enabled_command_scopes"].append("RendererTemplate")