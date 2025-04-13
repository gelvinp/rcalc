# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

### Fixed

- Fix building terminal renderer in release mode with clip module disabled (unused argument error).
- The statusbar message on both terminal and imgui renderers when saving a variable now puts matrices on newlines.
- The variables screen in the terminal renderer now renders matrices correctly (with newlines).

## [2.1.0] - 2025-04-05

### Major Feature: Variables

- Added a new type, Identifier, which gives variables a name. Identifiers are any value that begins and ends with double quotes (e.g. "var"). Identifiers are _NOT_ case sensitive, and they _CANNOT_ be empty.
- Save and load variables with the new '\store' and '\load' commands. Store expects the stack to contain the value to be stored followed by the identifier, and it consumes the identifier. Load expects an identifier, which is replaced on the stack by the loaded value.
- Clear all stored variables with the new '\clearvars' command.
- Use variables directly in expressions. When invoking an operator against a variable, it will be automatically loaded as its value.
- Graphically create, view, and manage your variables when using the ImGui renderer, as an alternative to the commands.
- Graphically view and manage your variables when using the Terminal renderer, as an alternative to the commands. (The Terminal renderer does not implement right-clicking to save a value as a variable.)
- Both renderers can use the new '\variables' command or the F2 key to view and manage variables.

### Changed

- The Terminal renderer now supports the F1 shortcut to open the help menu, and the F12 shortcut to open the settings menu.
- The ImGui renderer now supports the F12 shortcut to open the settings menu.

## [2.0.1] - 2024-07-26

### Changed

- Allow pasting minus signs into the scratchpad without immediately calling subtract.

### Fixed

- Added an include that I never needed before

## [2.0.0] - 2024-01-18

### Added

- Implemented `Sqr` and `Cube` operators for BigInts.
- Implemented `Neg` operator for Vec3s and Vec4s.
- Added a `Frac` operator for Reals, which returns the fractional component.
- Added a `Median` operator to calculate the median of a group of values.
- Added `SampStdDev` and `SampNormDist` operators to use the *sample* standard deviation.
- Added a new `--logfile` command line argument to send application logs to a file.

### Changed

- Improved the way that matrices are copied to the clipboard (Copied as one line that can be pasted back into RCalc as a valid matrix instead of formatted on multiple lines).
- Improved the way that swizzles are formatted (they now respect whether their input is an expression).
- Improved the way that integer addition, subtraction, and multiplication are checked for BigInt promotion.
- Negative values are now treated as expressions.
- BigInt values are parsed more reliably, especially when entered as binary, octal, or hexidecimal numbers. As a side effect, you can no longer enter negative binary, octal, or hexidecimal numbers directly, however negating a prefixed value probably never resulted in what you actually wanted anyways. Further work may be required in this area.
- Changed the way that the ImGui renderer wraps long lines to put the result on a new line for increased legibility (behaves the same as on iOS).
- Passing a negative value to `range` will skip `0`, going from 1...arg0 instead. The old behavior was to crash.
- Floating point rendering logic has been slightly adjusted again.
- Clarified that `StdDev` and `NormDist` use the *population* standard deviation.
- The `Sum` operator works to preserve precision when operating on Ints, BigInts, and no Reals.
- Fixed subtraction between Vec3s and subtraction between Vec4s
- The TerminalRenderer will now never log to stdout, and will only log when given the `--logfile` argument.
- The stack now has a maximum size of 100,000 items.

### Fixed

- Fixed a crash when swizzling an empty stack.
- Improved validation around inputs to the gamma operator. Additionally, the gamma operator now correctly returns Real numbers.
- Fix the bitwise `Not` operator returning too many digits.

### Removed

- Removed the Factorial operator for BigInts, due to performance reasons. Factorials can still *return* BigInt's.
- The rounding operators (ceil, floor, trunc, round) will no longer operator on Ints or BigInts.

## [1.4.0] - 2024-01-09

### Added

- Added a settings window to the ImGui renderer
  - Choose your color scheme, light mode or dark mode, or automatically select based on your system.
  - Change the UI scale to make RCalc more readable, or give yourself more space.
  - Change the floating point display precision to suit your needs.
- Added a settings window to the Terminal renderer
  - Change the floating point display precision to suit your needs.
- Settings will be shared between the ImGui and Terminal renderers.

### Changed

- Floating point rendering logic has been overhauled to be more consistent with values less than zero.
- Examples will always be rendered with precision set to `4` to avoid wrapping.
- The various Statistics operators render their inputs in individual chunks for more natural line wrapping.
- Some unit conversions have additional checks (i.e. negative radius is automatically corrected).

### Fixed

- Fixed a crash in the `\swap` command.
- Color conversions no longer check for being within a set range when convering *from* CIEXYZ D65.
- Spherical coordinate conversions use parameters in the correct order.

## [1.3.1] - 2023-12-27

### Fixed

- Fix a crash when operators return an error.

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
