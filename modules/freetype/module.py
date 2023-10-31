import os
import sys

def get_opts(env: "Environment"):
    from SCons.Variables import BoolVariable, EnumVariable

    opts = [
        BoolVariable("builtin_freetype", "Statically link the vendored copy of freetype", False)
    ]

    return opts


def configure(env: "Environment"):
    if env["builtin_freetype"]:
        env.Append(CPPPATH=["#/modules/freetype/upstream/include"])
    
    if env["platform"] == "linux":
        if env["builtin_freetype"]:
            env.ParseFlags("-isystem modules/freetype/upstream/include")
        else:
            deps = [
                "freetype2"
            ]
        
            if os.system(f"pkg-config --exists {' '.join(deps)}"):
                print(f"Error: Required libraries '{', '.join(deps)}' not found. Aborting.")
                sys.exit(255)
        
            env.ParseConfig(f"pkg-config {' '.join(deps)} --cflags --libs")
    elif env["platform"] == "macos":
        if env["builtin_freetype"]:
            pass
        else:
            if os.system("pkg-config --exists freetype2"):
                print(f"Error: Required library 'freetype2' not found. Aborting.")
                sys.exit(255)
            
            env.ParseConfig("pkg-config --cflags --libs freetype2")
    elif env["platform"] == "win":
        if env["builtin_freetype"]:
            pass
        else:
            if os.system("pkg-config --exists freetype2"):
                print(f"Error: Required library 'freetype2' not found. Aborting.")
                sys.exit(255)
            
            env.ParseConfig("pkg-config --static --cflags --libs freetype2")