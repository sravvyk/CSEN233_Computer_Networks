/*
 * File Copy Program Using System Calls
 * Uses: open(), read(), write(), close()
 *
 * This program copies files (both binary and text) using low-level
 * system calls without internal buffering.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 4096  // 4KB buffer

int main(int argc, char *argv[]) {
    int source_fd, dest_fd;
    unsigned char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    clock_t start, end;
    double cpu_time_used;
    long total_bytes = 0;

    // Check command line arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    // Start timing
    start = clock();

    // Open source file for reading
    source_fd = open(argv[1], O_RDONLY);
    if (source_fd == -1) {
        perror("Error opening source file");
        return 1;
    }

    // Open destination file for writing (create if doesn't exist, truncate if exists)
    dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("Error opening destination file");
        close(source_fd);
        return 1;
    }

    // Copy file contents
    while ((bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            if (bytes_written == -1) {
                perror("Error writing to destination file");
            } else {
                fprintf(stderr, "Error: Incomplete write operation\n");
            }
            close(source_fd);
            close(dest_fd);
            return 1;
        }
        total_bytes += bytes_written;
    }

    // Check for read errors
    if (bytes_read == -1) {
        perror("Error reading source file");
        close(source_fd);
        close(dest_fd);
        return 1;
    }

    // Close both files
    close(source_fd);
    close(dest_fd);

    // End timing
    end = clock();

    // Calculate and display results
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\n========================================\n");
    printf("File Copy Complete (System Calls)\n");
    printf("========================================\n");
    printf("Source:      %s\n", argv[1]);
    printf("Destination: %s\n", argv[2]);
    printf("Bytes copied: %ld bytes\n", total_bytes);
    printf("Time taken:   %.6f seconds\n", cpu_time_used);
    printf("========================================\n\n");

    return 0;
}
