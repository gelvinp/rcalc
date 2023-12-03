import os
import sys

def get_opts(env: "Environment"):
    from SCons.Variables import BoolVariable, EnumVariable

    if env["platform"] == "win":
        env["builtin_glfw"] = True
        return []

    opts = [
        BoolVariable("builtin_glfw", "Statically link the vendored copy of GLFW", False)
    ]

    return opts

def configure(env: "Environment"):
    if env["builtin_glfw"]:
        env.Append(CPPPATH=["#/modules/glfw/upstream/include"])
    
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
            print(f"Error: Required libraries '{', '.join(deps)}' not found. Aborting.")
            sys.exit(255)
        
        env.ParseConfig(f"pkg-config {' '.join(deps)} --cflags --libs")
    elif env["platform"] == "macos":
        env.Append(LINKFLAGS=[
            "-framework", "Foundation",
            "-framework", "Metal",
            "-framework", "QuartzCore",
            "-framework", "AppKit",
            "-framework", "Cocoa",
            "-framework", "IOKit",
            "-framework", "CoreFoundation"
        ])
        
        if env["builtin_glfw"]:
            pass
        else:
            if os.system("pkg-config --exists glfw3"):
                print(f"Error: Required library 'glfw3' not found. Aborting.")
                sys.exit(255)
            
            env.ParseConfig("pkg-config --cflags --libs glfw3")
    elif env["platform"] == "win":
        LIBS=["gdi32"]
        if env.msvc:
            env.Append(LINKFLAGS=[p + env["LIBSUFFIX"] for p in LIBS])
        else:
            env.Append(LIBS=LIBS)
        
        if env["builtin_glfw"]:
            pass
        else:
            if os.system(f"{env['mingw_prefix']}/bin/pkg-config --exists glfw3"):
                print(f"Error: Required library 'glfw3' not found. Aborting.")
                sys.exit(255)
            
            env.Append(LINKFLAGS=["-static"])
            env.ParseConfig(f"{env['mingw_prefix']}/bin/pkg-config --static --cflags --libs glfw3")