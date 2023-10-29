"""Functions used to generate source files during build time

All such functions are invoked in a subprocess on Windows to prevent build flakiness.

"""
from platform_methods import subprocess_main
import os


def gen_renderers(target, source, env):
    dst = target[0]

    lines = [
        '/* THIS FILE IS GENERATED DO NOT EDIT */',
        '',
        '#include "renderer.h"',
        '#include "core/logger.h"',
        '#include "core/format.h"',
        '',
        '#include <stdexcept>',
        ''
    ]

    for _name, meta in env["renderer_meta"].items():
        lines.append(f'#include "{meta["header"]}"')

    lines.extend([
        '',
        'namespace RCalc {',
        '',
        'Result<Renderer*> Renderer::create(const std::string_view& name, RendererCreateInfo info) {',
        '\tRenderer* p_renderer = nullptr;',
        ''
    ])

    for name, meta in env["renderer_meta"].items():
        lines.extend([
            f'\tif (name == "{name}") {{',
            f'\t\tp_renderer = reinterpret_cast<Renderer*>(new {meta["class"]}(info));',
            '\t}',
            ''
        ])
    
    lines.extend([
        '\tif (p_renderer == nullptr) {',
        '\t\treturn Err(ERR_INIT_FAILURE, fmt("The requested renderer \'%s\' is invalid!", name.data()));',
        '\t}',
        '',
        '\tResult<> res = p_renderer->init(info.p_application);',
        '\tif (!res) { return res.unwrap_err(); }',
        '',
        '\treturn Ok(p_renderer);',
        '}',
        '',
        f'const char* Renderer::default_renderer = "{env["default_renderer"]}";',
        'std::vector<const char*> enabled_renderers {'
    ])

    lines.append(',\n'.join([f'\t"{renderer}"' for renderer in env["enabled_renderers"]]))

    lines.extend([
        '};',
        '',
        'const std::vector<const char*>& Renderer::get_enabled_renderers() {',
        '\treturn enabled_renderers;',
        '}',
        '',
        '}'
    ])

    with open(dst, "w") as g:
        g.write('\n'.join(lines))


if __name__ == "__main__":
    subprocess_main(globals())
