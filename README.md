# LuminaEdit

LuminaEdit is a lightweight terminal-based text editor built from scratch in C, inspired by Kilo. The project focuses on understanding low-level editor internals by implementing terminal control, rendering, text manipulation, and file operations without external libraries.

It directly interacts with Unix terminals using POSIX APIs, featuring raw-mode input handling, ANSI escape sequence rendering, dynamic text buffers, manual memory management, and filesystem integration.

## Features

- Raw terminal mode using `termios` for byte-level input processing
- Custom keyboard event handling with ANSI escape sequence parsing
- Flicker-free rendering through buffered screen updates
- Dynamic row-based text storage using manual memory management
- Cursor navigation, scrolling, and editing operations
- File loading and saving through Unix system calls
- Incremental search and syntax highlighting support

## Tech Stack

- C
- POSIX APIs
- GCC/Clang
- Make
- Git
