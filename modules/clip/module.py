import os
import sys

def get_opts(env: "Environment"):
    return []

def configure(env: "Environment"):
    if env["platform"] == "linux":
        if env["windowing"] == "x11":
            deps = ["xcb"]

            if os.system(f"pkg-config --exists {' '.join(deps)}"):
                print(f"Error: Required libraries '{', '.join(deps)}' not found. Aborting.")
                sys.exit(255)
            
            env.ParseConfig(f"pkg-config {' '.join(deps)} --cflags --libs")
    elif env["platform"] == "win":
        LIBS=["ole32", "shlwapi", "User32", "windowscodecs"]
        if env.msvc:
            env.Append(LINKFLAGS=[p + env["LIBSUFFIX"] for p in LIBS])
        else:
            env.Append(LIBS=LIBS)