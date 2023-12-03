# RCalc

<p align="center">
    <img src="docs/images/header.jpg" alt="RCalc Logo">
</p>

RCalc is an RPN ([Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation)) calculator written in C++.
It aims to be quick to open and use, and easy to extend with new types, operators, and commands.

| [Features](#features) | [Install](#install) | [Usage](#usage) | [Build](#build) | [Future Plans](#future-plans) | [Contribute](#contribute) |
| :-: | :-: | :-: | :-: | :-: | | :-: |
| | | | | | |

## Features

### RPN Stack

Work quickly and efficiently using the RPN Stack by pushing and popping values. The stack will always show what series
of inputs led to each output, so you won't lose context during complicated expressions.

### Extended Types

RCalc can understand scalar types like integers and real numbers, as well as vectors and square matrices of sizes 2, 3, and 4.
When operating purely in the scalar integer domain, RCalc can promote up to BigInts to avoid floating point imprecision.

### XYZ Operators

RCalc has XYZ built-in operators with included documentation and examples.
All the expected operators (basic arithmetic, exponents, logarithms) are present, as are:

- Rounding functions (floor/ceil/trunc/round)
- [Unit conversions](#unit-conversions)
- Bitwise operations (and/or/not/xor/shift left/shift right)
- Statistics (avg/min/max/sum/std dev/NPV)
- Trigonometry (sin/cos/tan + arc/hyperbolic variants)
- Vector operations (dot/cross/normalize/length/swizzle)
- Matrix operations (identity/inverse/transpose + translation/scale/rotation matrix generators)

For a complete listing of operators you can see the in-app help menu or the [Operators wiki page](https://github.com/gelvinp/rcalc/wiki/Operators)

### Unit Conversions

RCalc can convert between units within a wide variety of families, including:

- Angle
- Area
- Colors (3 and 4 components)
- Coordinate systems (2D and 3D)
- Length
- Mass
- Storage (Base 10 [SI, 1000 bytes in a kb] and Base 2 [1024 bytes in a kb])
- Temperature
- Time
- Volume

For a complete listing of units and families, you can see the in-app help menu or the [Units wiki page](https://github.com/gelvinp/rcalc/wiki/Units)

### Value Representation

RCalc recognizes values represented in binary, octal, decimal, and hexidecimal. You can:

- Enter values in one representation, and convert them to another
- See the result of bitwise operations in binary automatically
- View vec3 and vec4 components (truncated) in hexidecimal when working with colors.

### Multi-Platform, Multi-Modal

RCalc runs in a lot of places, in a lot of ways. You can run RCalc on:

- Linux, with an official package in the Arch AUR, and an official Debian package in development,
- MacOS, with integration into the native menu bar,
- and Windows, either portably or with an installer for a start menu entry.

Additionally, RCalc can run in different modes, either graphically with an ImGui frontend or text-based with a FTXUI frontend.
Use the same familiar calculator on all of your computers, over SSH, and on servers with no display server (when compiled for that purpose).

Finally, RCalc can be used as a component in other software, such as an (in-development) official iOS/iPadOS port called SwiftRPN.
More details about SwiftRPN will be coming soon.

## Install

### Arch Linux

RCalc has an official package in the AUR, simply install `rcalc` with your preferred AUR helper. The AUR package enables opening the graphical frontend from your launcher through the `.desktop` file, and opening the terminal version from a console by running `rcalc`.

### Debian

An official Debian package is in the works. Until then, you may find success using the `debian` branch to build a .deb package.

### MacOS

RCalc is available as an official signed and notarized package for MacOS. [Download it here](https://github.com/gelvinp/rcalc/releases/latest).
It probably supports OSX 10.13 or later for x64 based Macs, and MacOS 11 or later for arm64 (Apple Silicon) based Macs,
however I am unable to test this, so I just have to trust that apple-clang does what I want it to do.

It is possible to access the terminal frontend from a console by creating a shell script somewhere in your path with the following:

```
#!/bin/zsh
/path/to/RCalc.app/Contents/MacOS/rcalc --renderer terminal $@
```

### Windows

RCalc is available on Windows as a portable zip, and as a .msi installer. Both packages contain the same content,
however the installer will also add an entry to the start menu and a shortcut to the desktop.
[Download it here](https://github.com/gelvinp/rcalc/releases/latest).

Due to how Windows works, the package comes with two separate .exe files. `RCalc.exe` is the graphical version, and
`rcalc-console.exe` in the portable package or `console/rcalc.exe` in the installed packge is the text-based console version.

## Usage

RCalc is designed to be simple to use. You can always access help from within RCalc by entering `\help` into the scratchpad,
and if you are using the graphical frontend, you can press F1, navigate to File > Show Help on Linux and Windows, or Help > RCalc Help on Mac OS.

RCalc supports some command line options, which you can view by running RCalc from a terminal with the `--help` flag.
The most important flag is `--renderer`, which allows you to select which frontend to open.

`rcalc --renderer imgui` will open the graphical frontend ()

## Build

> [!IMPORTANT]  
> This repository makes use of git submodules, it is important to make sure all
> submodules have been initialized or you will not be able to build RCalc.

RCalc is built using SCons (4.4 or later) and Python (3.10 or later).
For specific instructions for each platform, please see the relevant wiki page:

* [Building for Linux](https://github.com/gelvinp/rcalc/wiki/Building-for-Linux)
* [Building for MacOS](https://github.com/gelvinp/rcalc/wiki/Building-for-MacOS)
* [Building for Windows](https://github.com/gelvinp/rcalc/wiki/Building-for-Windows)

## Future Plans

In no particular order:

- Publish iOS version
- Publish iPadOS version
- Add configurable precision framework, implement in iOS port
- Add settings page to ImGui renderer including render scaling, default theme (light/dark/system?), precision
- Add ability to "dump the api" in debug builds, i.e. save the help text, command/operator/unit list, license info, etc to JSON or equivalent. Have some sort of python script after that can help keep any wiki documentation in the repo up to date.
- Add representation-aware operator implementations (bitwise ops should preserve the representations of their arguments).
- Add Wayland support for the linux builds (waiting on getting a new computer that can hopefully run wayland).

## Contribute

Please see the relevant wiki page for instructions:

- [Adding new operators](https://github.com/gelvinp/rcalc/wiki/Adding-new-operators)
- [Adding new commands](https://github.com/gelvinp/rcalc/wiki/Adding-new-commands)
- [Adding new units / unit families](https://github.com/gelvinp/rcalc/wiki/Adding-new-units)
- [Adding new renderers](https://github.com/gelvinp/rcalc/wiki/Adding-new-renderers)
- [Adding new types](https://github.com/gelvinp/rcalc/wiki/Adding-new-types)
