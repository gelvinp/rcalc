# RCalc - A Lightweight RPN Calculator

RCalc is an RPN ([Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation)) calculator written in C++.
It aims to be quick to open and use, and easy to extend with new types, operators, and commands.

**RCalc is currently unfinished and in active development. Please see the [feature table](#feature-table) below for more information.**


## Feature Table

| Feature                                                       | Status            |
| -------                                                       | ------            |
| Standard datatypes (ints and doubles)                         | Complete          |
| BigInt datatype                                               | Complete          |
| Vec2 / 3 / 4 datatypes                                        | Complete          |
| Mat2 / 3 / 4 datatypes                                        | Might not happen  |
| Basic operators (add, sub, mul, div, neg, abs)                | Complete          |
| Extended core operators (pow, log, ceil, sign, fact)          | Complete          |
| Trig operators ({,a}{sin,cos,tan}{,h})                        | Complete          |
| Floating point division (quot, rem, mod)                      | Complete          |
| Range operators (min, max, avg)                               | Complete          |
| Vector operators                                              | Complete          |
| Matrix operators                                              | Might not happen  |
| Conversion operators (deg<>rad, f<>c, ft<>m, mi<>km)          | Complete          |
| Core app commands (clear, quit, pop, swap, copy, dup, count)  | Complete          |
| Renderer commands (help, options)                             | In Progress       |
| Renderer scratchpad history                                   | Complete          |
| Renderer scratchpad autocomplete                              | Not started       |
| Alternate renderer (TUI)                                      | Not started       |


## Technical debt
 - [ ] Use perfect hash generator for operator and command maps
 - [ ] Operator map generator is getting pretty complex, could maybe use a rewrite?
 - [ ] Look into statically linking things because wow mac os is a headache when you don't use xcode
 - [ ] Rewrite help page (Collapsable param types, examples, how to enter values, how does rpn work, group ops by category)


## Platform support

| Platform  | Status                            |
| --------  | ------                            |
| Linux     | [Supported](#building-for-linux)  |
| Mac OS    | [Supported](#building-for-macos)  |
| Windows   | Not started                       |


### Building for Linux

The following dependencies are required to build RCalc:

 - Python
 - SCons
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


### Building for MacOS

The following dependencies are required to build RCalc:

 - Python
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