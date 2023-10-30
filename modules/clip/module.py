import os
import sys

def get_opts(env: "Environment"):
    return []

def configure(env: "Environment"):
    if env["platform"] == "linux":
        if env["windowing"] == "x11":
            deps = ["xcb"]

            if os.system(f"pkg-config --exists {' '.join(deps)}"):
                print("Error: Required libraries not found. Aborting.")
                sys.exit(255)
            
            env.ParseConfig(f"pkg-config {' '.join(deps)} --cflags --libs")