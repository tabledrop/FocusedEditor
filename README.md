# Focused Editor

A minimalist, distraction-free text editor built with Qt6 and C++. This editor is designed for programmers who want a clean, focused coding environment without the clutter of toolbars, menus, or other distracting UI elements.

## Features

- Clean, minimal interface without menus or ribbons
- Optimized for code editing with monospace fonts
- System-native monospace font support with fallbacks
- Full-screen mode for complete focus
- Subtle scrollbars that appear only when needed
- File change tracking with unsaved changes indicator
- Native macOS look and feel
- System theme support (light/dark mode)
- Text zoom functionality (default 13pt font size)
- Font customization through preferences
- Vim-style welcome screen

## Requirements

- macOS
- Qt 6.x
- CMake 3.16 or higher
- C++17 compatible compiler

## Installation

1. Install Qt6 using Homebrew:
```bash
brew install qt@6
```

2. Clone the repository and build:
```bash
git clone <repository-url>
cd focused-editor
mkdir build
cd build
cmake ..
cmake --build .
```

3. Run the editor:
```bash
./focused_editor
```

## Keyboard Shortcuts

The editor is designed to be keyboard-driven for minimal disruption to your writing flow:

| Shortcut | Action |
|----------|--------|
| ⌘ + S | Save current file |
| ⌘ + ⇧ + S | Save As |
| ⌘ + O | Open file |
| ⌃ + ⇧ + F | Toggle full-screen mode |
| ⌘ + Q | Quit |
| ⌘ + = | Zoom in |
| ⌘ + ⇧ + = | Alternative zoom in |
| ⌘ + - | Zoom out |
| ⌘ + 0 | Reset zoom to default size |
| ⌘ + , | Open preferences |

## Preferences

Access the preferences menu using ⌘ + , to customize:
- Font family (filtered to show only monospace fonts)
- Font size
- Live preview of font changes

## Design Philosophy

Focused Editor embodies three core principles:

1. **Minimalism**: No visual clutter or unnecessary features
2. **Focus**: Everything is designed to keep you in the writing flow
3. **Efficiency**: Fast startup, responsive editing, and intuitive shortcuts

## Development

This project is built with:
- C++17
- Qt6 for the GUI
- CMake for build management

The codebase is organized into three main files:
- `main.cpp`: Application entry point
- `editor_window.h`: Editor window class declaration
- `editor_window.cpp`: Editor window implementation
- `preferences_dialog.h`: Preferences dialog declaration
- `preferences_dialog.cpp`: Preferences dialog implementation

## Contributing

Feel free to submit issues and enhancement requests!
