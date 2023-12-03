import os
import sys


def get_opts(env: "Environment"):
    return []


def configure(env: "Environment"):
    if env["platform"] == "win":
        LIBS = ["opengl32"]
        if env.msvc:
            env.Append(LINKFLAGS=[p + env["LIBSUFFIX"] for p in LIBS])
        else:
            env.Append(LIBS=LIBS)
    else:
        deps = ['gl']

        if os.system(f"pkg-config --exists {' '.join(deps)}"):
            print(f"Error: Required libraries '{', '.join(deps)}' not found. Aborting.")
            sys.exit(255)
        
        env.ParseConfig(f"pkg-config {' '.join(deps)} --cflags --libs")