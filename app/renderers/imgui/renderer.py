import sys
import os

def get_priority():
    return 10


def is_available():
    return True


def meta():
    return {
        'header': 'app/renderers/imgui/imgui_renderer.h',
        'class': 'ImGuiRenderer'
    }


def get_opts():
    from SCons.Variables import BoolVariable

    return [
        BoolVariable("enable_dbus", "Enable D-Bus support for auto-detecting light/dark mode", True),
    ]


def configure(env: "Environment"):
    env["enabled_command_scopes"].append(meta()['class'])

    if env["enable_dbus"]:
        env.Append(CPPDEFINES=["DBUS_ENABLED"])
        if os.system("pkg-config dbug-1 --exists") != 0:
            print("Error: D-Bus dev libraries not found. Please either install them or set `enable_dbus=no`.")
        env.ParseConfig("pkg-config dbus-1 --cflags --libs")

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