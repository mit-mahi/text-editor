# Lumina — Terminal Text Editor in C

Lumina is a zero-dependency terminal text editor built from scratch in C using low-level POSIX APIs.  
It implements its own terminal control, rendering engine, text buffer, editing system, filesystem layer, search, and syntax highlighting.

## Features

### Terminal Engine
- Raw terminal mode using termios
- Byte-by-byte keyboard input handling
- ANSI VT100 escape sequence rendering
- Dynamic terminal size detection
- Cursor positioning and viewport scrolling

### Text Editing Engine
- Dynamic in-memory text buffer using custom row structures
- Manual memory management using malloc, realloc, and free
- Character insertion and deletion
- Multi-line editing with row splitting
- Efficient buffer manipulation using memcpy and memmove

### File System Support
- File loading from disk
- Save support using Ctrl-S
- POSIX based persistence using:
  - open()
  - write()
  - ftruncate()
  - close()

### Editor State Management
- Filename tracking
- Dirty-state detection
- Modified status indicator
- Unsaved-change protection

### Search
- Interactive Ctrl-F search prompt
- Dynamic query buffer
- Cursor navigation to matches

### Syntax Highlighting
- Custom token classification engine
- C keyword highlighting
- Number highlighting
- ANSI color rendering

## Build

```bash
make
