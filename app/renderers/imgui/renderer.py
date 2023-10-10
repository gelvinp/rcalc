def get_priority():
    return 10


def is_available():
    return True


def configure(env: "Environment"):
    env["enabled_modules"].extend([
        "imgui_core",
        "glfw",
        "freetype"
    ])