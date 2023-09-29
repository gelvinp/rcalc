# RCalc - A Lightweight RPN Calculator

RCalc is an RPN ([Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation)) calculator written in C++.
It aims to be quick to open and use, and easy to extend with new types, operators, and commands.

**RCalc is currently unfinished and in active development. Please see the [feature table](#feature-table) below for more information.**


## Feature Table

| Feature                                                       | Status            |
| -------                                                       | ------            |
| Standard datatypes (ints and doubles)                         | Complete          |
| BigInt datatype                                               | Not started       |
| Vec2 / 3 / 4 datatypes                                        | Not started       |
| Mat2 / 3 / 4 datatypes                                        | Might not happen  |
| Basic operators (add, sub, mul, div, neg, abs)                | Complete          |
| Extended core operators (pow, log, ceil, sign, fact)          | Complete          |
| Trig operators ({,a}{sin,cos,tan}{,h})                        | Complete          |
| Floating point division (quot, rem, mod)                      | Complete          |
| Range operators (min, max, avg)                               | Not started       |
| Vector operators                                              | Not started       |
| Matrix operators                                              | Might not happen  |
| Core app commands (clear, quit, pop, swap, copy, dup, count)  | Complete          |
| Renderer commands (help, options)                             | Not started       |
| Renderer scratchpad history                                   | Not started       |
| Renderer scratchpad autocomplete                              | Not started       |
| Alternate renderer (TUI)                                      | Not started       |


## Technical debt
 - [ ] Refactor renderer into buildtime-enabled, runtime-selected, inherited class
 - [ ] Use perfect hash generator for operator and command maps


## Platform support

| Platform  | Status                            |
| --------  | ------                            |
| Linux     | [Supported](#building-for-linux)  |
| Mac OS    | Not started                       |
| Windows   | Not started                       |


### Building for Linux

The following dependencies are required to build RCalc:

 - Python
 - SCons
 - pkg-config
 - glfw3 + development headers
 - OpenGL + development headers
 - freetype2 + development headers
 - A version of GCC or clang that supports c++20

For a debug build, run `scons` from the project root.

For a release build, run `scons target=release` from the project root.

To build with clang instead of GCC, use `scons use_llvm=yes`.

To build with asan, use `scons use_asan=yes`.