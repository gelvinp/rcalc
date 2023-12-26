# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.3.0] - 2023-12-26

### Added

- Byte unit family (Power of 2 and power of 10).
- Add days, weeks, and Julian years (365.25 days) to the Time unit family.
- Undo command (reset stack to before previous action).

### Changed

- Allow the hex operator to operate on vectors (useful for colors).
- Clarify the error message when given too few parameters, fix pluralization as well.
- Unify the whitespace for help text sections.
- Reduce redundant negation (i.e. -(-(5)) becomes just 5).
- Change floating point precision to 15 places (Likely to become adjustable soon).
- Improve units autocomplete to only suggest the second unit from the same family as the first unit.
- Improve units autocomplete to not suggest a second unit equal to the first unit.
- Display all commands in one list, not separated by Application / Renderer scope.
- Clear scratchpad on backspace during autocomplete in the terminal renderer
- Lots of internal changes to support integration into a native iOS app.
- Changed sort order of units within families in the help page to be more natural.
- Optimized memory allocation.

### Fixed

- Correctly display non-decimal values in the terminal renderer.
- Fix background misalignment for some colored text in the ImGui renderer.
- Use the correct function for calculating the cube root.
- Fix autocomplete behavior for suggesting operators and units.

## [1.2.0] - 2023-10-31

### Added

- Terminal renderer, a completely text-based interface, allows access to all of RCalc with just a keyboard.
  Works in 2, 16, 256, and true color palettes, in terminal emulators, over ssh, on machines with no display server, and in the windows terminal(s).
  Optionally supports the mouse for scrolling and interacting with the help, and optionally supports copying to the clipboard.
- Add a command line flag to print the license info.

### Changed

- Allow smart autocomplete in the reverse direction (by pressing Shift + Tab instead of Tab).
- Buildsystem changes to allow builds capable of running with no display server by only linking against neccesary libraries.
- Add additional padding between entries in the ImGui renderer stack.

### Fixed

- Fixed a bug that caused results to be cut off when the scrollbar appeared in the ImGui renderer.
- Correctly display the swizzle operation
- Only pop entries from the ImGui renderer stack when pressing the delete key and **when the scratchpad is empty**.
- Avoid re-opening the help menu when pressing F1 when the help menu is already open.

## [1.1.0] - 2023-10-27

### Added

- Smart autocomplete to recommend commands, operators, and units based on the types of values in the stack.

### Changed

- Reworked renderer to properly display matrices and column vectors using newlines.
- Provide feedback when the version hash was copied to the clipboard in the ImGui renderer.
