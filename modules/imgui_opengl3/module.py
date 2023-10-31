import os
import sys


def get_opts(env: "Environment"):
    return []


def configure(env: "Environment"):
    if env["platform"] != "win":
        deps = ['gl']

        if os.system(f"pkg-config --exists {' '.join(deps)}"):
            print(f"Error: Required libraries '{', '.join(deps)}' not found. Aborting.")
            sys.exit(255)
        
        env.ParseConfig(f"pkg-config {' '.join(deps)} --cflags --libs")