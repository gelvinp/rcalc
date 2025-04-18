def get_priority():
    return 5


def is_available(env: "Environment"):
    return True


def meta():
    return {
        'header': 'app/renderers/template/renderer_template.h',
        'class': 'RendererTemplate'
    }


def configure(env: "Environment"):
    env["enabled_command_scopes"].append(meta()['class'])