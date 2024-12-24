# File Processing Utilities

This project implements three command-line utilities for processing text files:
- `findlocation`: Looks up location information based on a phone number prefix
- `head`: Displays the first N lines of a file
- `tail`: Displays the last N lines of a file

## Author
- Bungein Cheng

## Components

### findlocation
A utility that performs binary search on a file containing phone number prefixes and their corresponding locations.

#### Usage
```
findlocation <number> [file]
```
- `number`: A 10-digit phone number
- `file`: Optional input file (defaults to stdin if not provided)

#### Features
- Binary search implementation for efficient lookup
- Supports both seekable and non-seekable file descriptors
- Memory-mapped file handling for improved performance
- Input validation for phone numbers
- Error handling for file operations

### head
A utility that displays the first N lines of a file.

#### Usage
```
head [-n count] [file]
```
- `-n count`: Number of lines to display (defaults to 10)
- `file`: Optional input file (defaults to stdin if not provided)

#### Features
- Configurable number of lines to display
- Buffer-based file reading for efficiency
- Proper error handling
- Support for reading from stdin or files

### tail
A utility that displays the last N lines of a file.

#### Usage
```
tail [-n count] [file]
```
- `-n count`: Number of lines to display (defaults to 10)
- `file`: Optional input file (defaults to stdin if not provided)

#### Features
- Configurable number of lines to display
- Rolling buffer implementation for memory efficiency
- Support for reading from stdin or files
- Proper memory management and cleanup

## Building
Compile each utility using a C compiler:
```bash
gcc -o findlocation findlocation.c
gcc -o head head.c
gcc -o tail tail.c
```

## Error Handling
All utilities include comprehensive error handling for:
- Invalid command-line arguments
- File operations
- Memory allocation
- Input/output operations

## Implementation Notes
- All utilities use low-level system calls (open, read, write) for file operations
- Custom string handling functions are implemented in findlocation
- Memory-mapped files are used when possible for improved performance
- Buffer sizes are optimized for efficiency