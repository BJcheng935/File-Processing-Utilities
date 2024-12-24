//Bungein Cheng 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define DEFAULT_NUM_LINES 10
#define BUFFER_SIZE 1024

// Function to print the first `n` lines from the given file descriptor
void print_first_n_lines(int fd, int num_lines) {
    char buffer[BUFFER_SIZE];
    int lines_printed = 0, bytes_read;
    
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0 && lines_printed < num_lines) {
        for (int i = 0; i < bytes_read; i++) {
            if (write(STDOUT_FILENO, &buffer[i], 1) == -1) {
                perror("Error writing to stdout");
                exit(1);
            }
            if (buffer[i] == '\n') {
                lines_printed++;
                if (lines_printed == num_lines) break;
            }
        }
    }

    if (bytes_read == -1) {
        perror("Error reading file");
        exit(1);
    }
}

// Function to convert string to integer
int string_to_int(const char *str) {
    int result = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            fprintf(stderr, "Invalid number format\n");
            exit(1);
        }
        result = result * 10 + (str[i] - '0');
    }
    return result;
}

int main(int argc, char *argv[]) {
    int num_lines = DEFAULT_NUM_LINES;
    int fd = STDIN_FILENO; // Default to reading from stdin
    int file_arg = 0;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {
                num_lines = string_to_int(argv[i + 1]);
                i++;
            } else {
                fprintf(stderr, "Missing argument for -n\n");
                return 1;
            }
        } else {
            file_arg = i;
        }
    }

    // Open file if filename is provided
    if (file_arg) {
        fd = open(argv[file_arg], O_RDONLY);
        if (fd == -1) {
            perror("Error opening file");
            return 1;
        }
    }

    print_first_n_lines(fd, num_lines);

    if (fd != STDIN_FILENO) close(fd);
    return 0;
}
