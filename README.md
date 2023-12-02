# RCalc

<p align="center">
    <img src="docs/images/header.jpg" alt="RCalc Logo">
</p>

RCalc is an RPN ([Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation)) calculator written in C++.
It aims to be quick to open and use, and easy to extend with new types, operators, and commands.

| [Features](#features) | [Install](#install) | [Build](#build) | [Future Plans](#future-plans) | [Contribute](#contribute) |
| :-: | :-: | :-: | :-: | :-: |
| | | | | |

## Features

### RPN Stack

### Extended Types

### XYZ Operators

### Unit Conversions

### Value Representation

### Multi-Platform, Multi-Modal

## Install

### Arch Linux

### Debian

### MacOS

### Windows

## Build

Please see the relevant wiki page for instructions:

* [Building for Linux](#building-for-linux)
* [Building for MacOS](#building-for-macos)
* [Building for Windows](#building-for-windows)

## Future Plans

In no particular order

- Publish iOS version
- Publish iPadOS version
- Add configurable precision framework, implement in iOS port
- Add settings page to ImGui renderer including render scaling, default theme (light/dark/system?), precision
- Add ability to "dump the api" in debug builds, i.e. save the help text, command/operator/unit list, license info, etc to JSON or equivalent. Have some sort of python script after that can help keep any wiki documentation in the repo up to date.

## Contribute

Please see the relevant wiki page for instructions:

- Adding new operators
- Adding new commands
- Adding new units / unit families
- Adding new renderers
- Adding new types


## Install RCalc Now

If you use Arch Linux, RCalc is in the AUR. Simply `paru -S rcalc` (or equivalent AUR helper).

If you use Debian, MacOS, or Windows, you can download the appropriate file from the [latest release](https://github.com/gelvinp/rcalc/releases/latest).


## Feature Table

| Feature                                                       | Status            |
| -------                                                       | ------            |
| Standard datatypes (ints and doubles)                         | Complete          |
| BigInt datatype                                               | Complete          |
| Vec2 / 3 / 4 datatypes                                        | Complete          |
| Mat2 / 3 / 4 datatypes                                        | Complete          |
| Basic operators (add, sub, mul, div, neg, abs)                | Complete          |
| Extended core operators (pow, log, ceil, sign, fact)          | Complete          |
| Trig operators ({,a}{sin,cos,tan}{,h})                        | Complete          |
| Floating point division (quot, rem, mod)                      | Complete          |
| Range operators (min, max, avg)                               | Complete          |
| Vector operators                                              | Complete          |
| Matrix operators                                              | Complete          |
| Conversion operators (deg<>rad, f<>c, ft<>m, mi<>km)          | Complete          |
| Core app commands (clear, quit, pop, swap, copy, dup, count)  | Complete          |
| Renderer commands (help)                                      | Complete          |
| Renderer scratchpad history                                   | Complete          |
| Renderer scratchpad autocomplete                              | Complete          |
| Extended Conversion Operators                                 | Complete          |
| Binary/Bitwise operators + Alternate display modes (hex, etc) | Complete          |
| Alternate renderer (TUI)                                      | Complete          |
| Lib target (for SwiftUI port)                                 | Complete          |


## Technical debt
 - [ ] Implement arena allocator (At least for the displayables? Just generally audit for unneeded allocations)
 - [ ] Go poke at wayland? Find a machine that can *actually* run it?
 - [ ] Rewrite readme / create wiki with documentation on usage and development


## Platform support

| Platform  | Status                            |
| --------  | ------                            |
| Linux     | [Supported](#building-for-linux)  |
| Mac OS    | [Supported](#building-for-macos)  |
| Windows   | [Supported](#building-for-windows)|


### Building for Linux

The following dependencies are required to build RCalc:

 - Python 3.10 or later
 - SCons 4.4 or later
 - pkg-config
 - glfw3 + development headers*
 - OpenGL + development headers
 - freetype2 + development headers*
 - A version of GCC or clang that supports c++20

\* GLFW and FreeType can be statically linked using the vendored modules.
To do this, first make sure to initialize the git submodules, and then
run `scons builtin_glfw=yes builtin_freetype=yes`.
(You can also choose to only statically link one or the other.)

For a debug build, run `scons` from the project root.

For a release build, run `scons target=release` from the project root.

To build with clang instead of GCC, use `scons use_llvm=yes`.

To build with asan, use `scons use_asan=yes`.

To use gperf to generate the operator, command, and unit maps, instead of std::map, use `scons gperf_path=/path/to/gperf`.


### Building for MacOS

The following dependencies are required to build RCalc:

 - Python 3.10 or later
 - SCons
 - pkg-config
 - glfw3 + development headers*
 - freetype2 + development headers*
 - A version of apple-clang, clang or gcc that supports c++20

\* GLFW and FreeType can be statically linked using the vendored modules.
To do this, first make sure to initialize the git submodules, and then
run `scons builtin_glfw=yes builtin_freetype=yes`.
(You can also choose to only statically link one or the other.)

For a debug build, run `scons` from the project root.

For a release build, run `scons target=release` from the project root.

Building on MacOS will always produce a `.app` bundle.

To build with GCC instead of clang, use `scons use_gcc=yes`, and make sure your `CC`, `CXX`, `RANLIB` and `AR` are set appropriately.
You must set these yourself as MacOS aliases `gcc` and `g++` to use Apple Clang instead.

To build with asan, use `scons use_asan=yes`.

To use gperf to generate the operator, command, and unit maps, instead of std::map, use `scons gperf_path=/path/to/gperf`.


### Building for Windows

~~CL.exe / MSVC is *not* a supported build method at this time. Instead, please install a MinGW distribution.~~

You may use MSVC to compile RCalc. SCons should auto-detect your installation from any command line, but if it is unable to,
please run SCons from a "Native Tools Command Prompt for VS". More specific instructions are coming soon.

The following package lists assume you are using MSYS2:

#### Building with MSYS2 using GCC

The following dependencies, or suitable alternatives, are required to build RCalc:

- python 3.10 or later
- mingw-w64-ucrt-x86_64-scons 4.4 or later
- mingw-w64-ucrt-x86_64-pkg-config
- mingw-w64-ucrt-x86_64-glfw*
- mingw-w64-ucrt-x86_64-freetype*
- mingw-w64-ucrt-x86_64-gcc (A version that supports c++20)

#### Building with MSYS2 using clang

The following dependencies, or suitable alternatives, are required to build RCalc:

- python 3.10 or later
- mingw-w64-clang-x86_64-scons 4.4 or later
- mingw-w64-clang-x86_64-pkg-config
- mingw-w64-clang-x86_64-glfw*
- mingw-w64-clang-x86_64-freetype*
- mingw-w64-clang-x86_64-clang (A version that supports c++20)


\* GLFW and FreeType can be linked using the vendored modules.
To do this, first make sure to initialize the git submodules, and then
run `scons builtin_glfw=yes builtin_freetype=yes`.
(You can also choose to only use one builtin or the other.)

When compiling with MinGW, both GLFW and Freetype will be statically linked, regardless
if using the builtin modules or not. Additionally, libgcc and libstdc++ will also be
statically linked.

For a debug build, run `scons` from the project root.

For a release build, run `scons target=release` from the project root.

To build with clang instead of GCC, use `scons use_llvm=yes`.

To use gperf to generate the operator, command, and unit maps, instead of std::map, use `scons gperf_path=/path/to/gperf`.
