# RCalc - A Lightweight RPN Calculator

RCalc is an RPN ([Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation)) calculator written in C++.
It aims to be quick to open and use, and easy to extend with new types, operators, and commands.

**RCalc is currently in active development. Please see the [feature table](#feature-table) below for more information.**


## Install RCalc Now

If you use Arch Linux, RCalc is in the AUR. Simply `paru -S rcalc` (or equivalent AUR helper).


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
| Renderer commands (help, options)                             | In Progress       |
| Renderer scratchpad history                                   | Complete          |
| Renderer scratchpad autocomplete                              | Not started       |
| Alternate renderer (TUI)                                      | Not started       |
| Extended Conversion Operators                                 | Complete          |
| Binary/Bitwise operators + Alternate display modes (hex, etc) | Complete          |


## Technical debt
 - [ ] Implement arena allocator


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

CL.exe / MSVC is *not* a supported build method at this time. Instead, please install a MinGW distribution.
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
