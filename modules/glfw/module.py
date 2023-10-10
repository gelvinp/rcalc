import os
import sys

def get_opts(env: "Environment"):
    from SCons.Variables import BoolVariable, EnumVariable

    opts = [
        BoolVariable("builtin_glfw", "Statically link the vendored copy of GLFW", False)
    ]

    if env["platform"] == "linux":
        opts.extend([
            EnumVariable(
                "windowing",
                help="windowing system to use",
                default="x11",
                allowed_values=("x11", "wayland"),
                ignorecase=2
            )
        ])

    return opts

def configure(env: "Environment"):
    if env["platform"] == "linux":
        deps = []

        if env["builtin_glfw"]:
            deps.extend([
                "x11",
                "xrandr",
                "xinerama",
                "xi"
            ])
        else:
            deps.extend([
                "glfw3"
            ])
        
        if os.system(f"pkg-config --exists {' '.join(deps)}"):
            print("Error: Required libraries not found. Aborting.")
            sys.exit(255)
        
        env.ParseConfig(f"pkg-config {' '.join(deps)} --cflags --libs")