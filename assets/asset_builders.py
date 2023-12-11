"""Functions used to generate source files during build time

All such functions are invoked in a subprocess on Windows to prevent build flakiness.

"""
from platform_methods import subprocess_main
from string import ascii_lowercase, digits
import os


def embed_asset(target, source, env):
    src = source[0]
    dst = target[0]

    asset_name = src.split(os.sep)[-1]
    asset_name = asset_name.lower()
    allowed = list(ascii_lowercase + digits + '_')
    asset_name = ''.join([c if c in allowed else '_' for c in asset_name])

    with open(src, "rb") as f:
        buf = f.read()

    with open(dst, "w") as g:
        g.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
        g.write("#pragma once\n\n")
        g.write("#include <array>\n\n")
        g.write("namespace RCalc::Assets {\n\n")
        g.write(f"constexpr const std::array<unsigned char, {len(buf)}> {asset_name} = {{\n")
        for i in range(len(buf)):
            g.write(str(buf[i]) + ",\n")
        g.write("};\n")
        g.write("\n}\n")


def embed_text(target, source, env):
    src = source[0]
    dst = target[0]

    asset_name = src.split(os.sep)[-1]
    asset_name = asset_name.lower()
    allowed = list(ascii_lowercase + digits + '_')
    asset_name = ''.join([c if c in allowed else '_' for c in asset_name])

    with open(dst, "w") as g:
        g.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
        g.write("#pragma once\n")
        g.write("namespace RCalc::Assets {\n")
        g.write(f'static const char* {asset_name} = R"EMBEDTEXTDELIMIT(')
        
        with open(src, 'r') as src_text:
            g.write(src_text.read().replace('\n\n\n', '\n\n\n)EMBEDTEXTDELIMIT" R"EMBEDTEXTDELIMIT('))
        
        g.write(')EMBEDTEXTDELIMIT";\n')
        g.write("\n}\n")


if __name__ == "__main__":
    subprocess_main(globals())