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

- Windows or macOS
- Qt 6.x
- CMake 3.16 or higher
- C++17 compatible compiler
  - Windows: MSVC 2019/2022 or MinGW
  - macOS: Clang

## Installation

### Windows

1. Install Qt6 from [Qt's official website](https://www.qt.io/download)

2. Clone and build:
```bash
git clone https://github.com/tabledrop/focused-editor
cd focused-editor
mkdir build
cd build

# If using MSVC:
cmake -B . -S .. -DQt6_DIR="PathTo:/Qt/6.x.x/msvc2022_64/lib/cmake/Qt6"
cmake --build .

# If using MinGW:
cmake -B . -S .. -DQt6_DIR="PathTo:/Qt/6.x.x/mingw_64/lib/cmake/Qt6" -G "MinGW Makefiles"
cmake --build .
```

3. Deploy the application:
```bash
# Copy required Qt DLLs (replace path with your Qt installation)
PathTo:\Qt\6.x.x\msvc2022_64\bin\windeployqt.exe .\Debug\focused_editor.exe
```

4. Run the editor:
```bash
.\Debug\focused_editor.exe
```

### macOS
1. Install Qt6 using Homebrew:
```bash
brew install qt@6
```

2. Clone the repository and build:
```bash
git clone https://github.com/tabledrop/focused-editor
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

The editor supports both Windows and macOS keyboard shortcuts:

| Action | Windows | macOS |
|--------|---------|-------|
| Save | Ctrl + S | ⌘ + S |
| Save As | Ctrl + Shift + S | ⌘ + ⇧ + S |
| Open file | Ctrl + O | ⌘ + O |
| Full-screen | Ctrl + Shift + F | ⌃ + ⇧ + F |
| Quit | Alt + F4 | ⌘ + Q |
| Zoom in | Ctrl + = | ⌘ + = |
| Zoom in (alt) | Ctrl + Shift + = | ⌘ + ⇧ + = |
| Zoom out | Ctrl + - | ⌘ + - |
| Reset zoom | Ctrl + 0 | ⌘ + 0 |
| Preferences | Ctrl + , | ⌘ + , |

## Preferences

Access the preferences menu using Ctrl + , (Windows) or ⌘ + , (macOS) to customize:
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

The codebase is organized into multiple files:
- `main.cpp`: Application entry point
- `editor_window.h`: Editor window class declaration
- `editor_window.cpp`: Editor window implementation
- `preferences_dialog.h`: Preferences dialog declaration
- `preferences_dialog.cpp`: Preferences dialog implementation

## Contributing

Feel free to submit issues and enhancement requests!
