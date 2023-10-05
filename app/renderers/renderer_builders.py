"""Functions used to generate source files during build time

All such functions are invoked in a subprocess on Windows to prevent build flakiness.

"""
from platform_methods import subprocess_main
import os


def set_default_renderer(target, source, env):
    dst = target[0]

    with open(dst, "w") as g:
        g.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
        g.write('#include "app/renderers/renderer.h"\n')
        g.write("namespace RCalc {\n\n")
        g.write(f'const char* Renderer::default_renderer = "{env["default_renderer"]}";\n\n')
        g.write("std::vector<const char*> Renderer::get_enabled_renderers() {\n")
        g.write("\treturn {\n")
        
        for renderer in env["enabled_renderers"]:
            g.write(f'\t\t"{renderer}",\n')
        
        g.write("\t};\n")
        g.write("}\n")
        g.write("\n}\n")


if __name__ == "__main__":
    subprocess_main(globals())