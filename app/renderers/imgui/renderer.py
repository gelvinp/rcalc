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
    
    if env["platform"] == "macos":
        env["enabled_modules"].append("imgui_metal")
    else:
        env["enabled_modules"].extend([
            "imgui_opengl3",
            "stb_image"
        ])
    
    env["enabled_command_scopes"].append("ImGuiRenderer")