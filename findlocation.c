// Bungein Cheng 
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h> // for errno

#define BUFFER_SIZE 4096
#define LINE_SIZE 32

// Function prototypes
void display_error_message(const char *message);
size_t my_strlen(const char *s);
int my_strncmp(const char *s1, const char *s2, size_t n);
int is_valid_number(const char *number);
void lookup_location(int fd, const char *number);

void display_error_message(const char *message) // Function to display error messages
{
    write(STDERR_FILENO, message, my_strlen(message));
}

void my_memcpy(char *dest, const char *src, size_t n) // Function to copy memory
{
    for (size_t i = 0; i < n; i++)
    {
        dest[i] = src[i];
    }
}

size_t my_strlen(const char *s) // Function to get length of string
{
    size_t len = 0;
    while (s[len]) // Check for end of string
        len++;     // Increment length
    return len;
}

int my_strncmp(const char *s1, const char *s2, size_t n) // Function to compare strings
{
    for (size_t i = 0; i < n; i++) // Compare up to `n` characters
    {
        if (s1[i] != s2[i])                                     // Compare characters
            return (unsigned char)s1[i] - (unsigned char)s2[i]; // Return difference
        if (s1[i] == '\0')                                      // Check for end of string
            return 0;
    }
    return 0;
}

int is_valid_number(const char *number) // Function to check if the number is valid
{
    for (int i = 0; i < 10; i++)
    {
        if (number[i] < '0' || number[i] > '9') // Check if the number contains non-digit characters
        {
            return 0;
        }
    }
    if (number[10] != '\0') // Check if the number has exactly 10 digits
    {
        return 0;
    }
    return 1;
}

void lookup_location(int fd, const char *number) // Function to lookup the location for the given number
{
    struct stat st;
    if (fstat(fd, &st) == -1) // Get file size
    {
        display_error_message("An error occurred while getting the file size.\n");
        return;
    }

    if (lseek(fd, 0, SEEK_CUR) == -1 && errno == ESPIPE) // Check if the file descriptor is seekable
    {
        size_t file_size = 0; // File descriptor is not seekable so read the file manually into memory
        char *buffer = NULL;
        ssize_t bytes_read;
        char temp_buf[BUFFER_SIZE];

        while ((bytes_read = read(fd, temp_buf, sizeof(temp_buf))) > 0) // Read the input into buffer dynamically
        {
            char *new_buffer = realloc(buffer, file_size + bytes_read); // Reallocate memory
            if (new_buffer == NULL)                                     // Check if memory allocation was successful
            {
                free(buffer);
                display_error_message("Memory allocation failed.\n"); // Display error message
                return;
            }
            buffer = new_buffer;                              // Update buffer
            memcpy(buffer + file_size, temp_buf, bytes_read); // Copy data into buffer
            file_size += bytes_read;                          // Update file size
        }
        if (bytes_read == -1)
        {
            free(buffer); // Free memory
            display_error_message("An error occurred while reading the file.\n");
            return;
        }

        // Perform binary search on the buffer
        size_t left = 0, right = file_size / LINE_SIZE; // Binary search
        while (left <= right)
        {
            size_t mid = (left + right) / 2;         // Calculate the middle index
            char *line = buffer + (mid * LINE_SIZE); // Get the line at the middle index
            int cmp = my_strncmp(line, number, 6);   // Make sure to compare only the first 6 characters
            if (cmp == 0)
            {
                write(STDOUT_FILENO, line + 6, 25); // Print location after the prefix
                write(STDOUT_FILENO, "\n", 1);      // Newline after location
                free(buffer);
                return;
            }
            else if (cmp < 0) // If the prefix is less than the current line
            {
                left = mid + 1;
            }
            else
            {
                if (mid == 0) // If the prefix is greater than the current line
                    break;
                right = mid - 1;
            }
        }
        display_error_message("Prefix not found.\n"); // Prefix not found
        free(buffer);
    }
    else
    {
        char *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0); // The file is seekable, use mmap
        if (map == MAP_FAILED)
        {
            display_error_message("An error occurred while mapping the file to memory.\n");
            return;
        }

        size_t left = 0, right = st.st_size / LINE_SIZE; // Perform binary search on the memory-mapped file
        while (left <= right)                            // Binary search
        {
            size_t mid = (left + right) / 2; // Calculate the middle index
            char *line = map + (mid * LINE_SIZE);
            int cmp = my_strncmp(line, number, 6); // Compare only the first 6 characters
            if (cmp == 0)
            {
                write(STDOUT_FILENO, line + 6, 25); // Print location
                write(STDOUT_FILENO, "\n", 1);      // Print newline
                munmap(map, st.st_size);
                return;
            }
            else if (cmp < 0) // If the prefix is less than the current line
            {
                left = mid + 1;
            }
            else
            {
                if (mid == 0) // If the prefix is greater than the current line
                    break;
                right = mid - 1;
            }
        }

        display_error_message("Prefix not found.\n"); // Prefix not found
        munmap(map, st.st_size);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3) // Check if the number of arguments is valid
    {
        display_error_message("Usage: findlocation <number> [file]\n");
        return 1;
    }

    if (!is_valid_number(argv[1])) // Check if the number format is valid
    {
        display_error_message("Invalid number format. Must be 10 digits.\n");
        return 1;
    }
    int fd = STDIN_FILENO; // Use stdin or open the file if provided
    if (argc == 3)
    {
        fd = open(argv[2], O_RDONLY);
        if (fd == -1)
        {
            display_error_message("An error occurred while opening the file.\n");
            return 1;
        }
    }
    lookup_location(fd, argv[1]); // Lookup the location for the given number
    if (fd != STDIN_FILENO)       // Close the file descriptor if it's not stdin
    {
        close(fd);
    }

    return 0;
}
