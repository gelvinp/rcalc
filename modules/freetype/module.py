import os
import sys

def get_opts(env: "Environment"):
    from SCons.Variables import BoolVariable, EnumVariable

    opts = [
        BoolVariable("builtin_freetype", "Statically link the vendored copy of freetype", False)
    ]

    return opts


def configure(env: "Environment"):
    if env["platform"] == "linux":
        if env["builtin_freetype"]:
            env.Append(CPPPATH=["#/modules/freetype/upstream/include"])
            env.ParseFlags("-isystem modules/freetype/upstream/include")
        else:
            deps = [
                "freetype2"
            ]
        
            if os.system(f"pkg-config --exists {' '.join(deps)}"):
                print("Error: Required libraries not found. Aborting.")
                sys.exit(255)
        
            env.ParseConfig(f"pkg-config {' '.join(deps)} --cflags --libs")