# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Terminal renderer, a completely text-based interface, allows access to all of RCalc with just a keyboard.
  Works in 2, 16, 256, and true color palettes, in terminal emulators, over ssh, on machines with no display server, and in the windows terminal(s).
  Optionally supports the mouse for scrolling and interacting with the help, and optionally supports copying to the clipboard.
- Add a command line flag to print the license info.

### Changed

- Allow smart autocomplete in the reverse direction (by pressing Shift + Tab instead of Tab).
- Buildsystem changes to allow builds capable of running with no display server by only linking against neccesary libraries.
- Add additional padding between entries in the ImGui renderer stack.

### Deprecated

-

### Removed

-

### Fixed

- Fixed a bug that caused results to be cut off when the scrollbar appeared in the ImGui renderer.
- Correctly display the swizzle operation
- Only pop entries from the ImGui renderer stack when pressing the delete key and **when the scratchpad is empty**.
- Avoid re-opening the help menu when pressing F1 when the help menu is already open.

## Security

-

## [1.1.0] - 2023-10-27

### Added

- Smart autocomplete to recommend commands, operators, and units based on the types of values in the stack.

### Changed

- Reworked renderer to properly display matrices and column vectors using newlines.
- Provide feedback when the version hash was copied to the clipboard in the ImGui renderer.
