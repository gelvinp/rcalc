def get_priority():
    return 5


def is_available(env: "Environment"):
    return env["tests_enabled"]


def meta():
    return {
        'header': 'app/renderers/dummy/dummy_renderer.h',
        'class': 'DummyRenderer'
    }


def configure(env: "Environment"):
    env["enabled_command_scopes"].append(meta()['class'])