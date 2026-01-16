/*
 * File Copy Program Using C Library Functions
 * Uses: fopen(), fread(), fwrite(), fclose()
 *
 * This program copies files (both binary and text) using high-level
 * C library functions with buffering.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 4096  // 4KB buffer

int main(int argc, char *argv[]) {
    FILE *source_file, *dest_file;
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read, bytes_written;
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

    // Open source file for reading in binary mode
    source_file = fopen(argv[1], "rb");
    if (source_file == NULL) {
        perror("Error opening source file");
        return 1;
    }

    // Open destination file for writing in binary mode
    dest_file = fopen(argv[2], "wb");
    if (dest_file == NULL) {
        perror("Error opening destination file");
        fclose(source_file);
        return 1;
    }

    // Copy file contents
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, source_file)) > 0) {
        bytes_written = fwrite(buffer, 1, bytes_read, dest_file);
        if (bytes_written != bytes_read) {
            fprintf(stderr, "Error: Write operation failed\n");
            fclose(source_file);
            fclose(dest_file);
            return 1;
        }
        total_bytes += bytes_written;
    }

    // Check for read errors
    if (ferror(source_file)) {
        perror("Error reading source file");
        fclose(source_file);
        fclose(dest_file);
        return 1;
    }

    // Close both files
    fclose(source_file);
    fclose(dest_file);

    // End timing
    end = clock();

    // Calculate and display results
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\n========================================\n");
    printf("File Copy Complete (Library Functions)\n");
    printf("========================================\n");
    printf("Source:      %s\n", argv[1]);
    printf("Destination: %s\n", argv[2]);
    printf("Bytes copied: %ld bytes\n", total_bytes);
    printf("Time taken:   %.6f seconds\n", cpu_time_used);
    printf("========================================\n\n");

    return 0;
}
