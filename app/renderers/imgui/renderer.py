import sys

def get_priority():
    return 10


def is_available():
    return True


def meta():
    return {
        'header': 'app/renderers/imgui/imgui_renderer.h',
        'class': 'ImGuiRenderer'
    }


def configure(env: "Environment"):
    env["enabled_command_scopes"].append(meta()['class'])

    if env["platform"] == "win" and env["windows_subsystem"] == "console":
        print("The ImGui renderer does NOT work on windows with the console subsystem!")
        print("Either...")
        print("\tDisable the ImGui renderer (enable_imgui_renderer=no)")
        print("\tOR Switch to the GUI renderer (windows_subsystem=gui)")
        sys.exit(255)
    
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