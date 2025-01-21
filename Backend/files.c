#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Utility function to read content from a file
char *readFile(const char *filename) {
    char filepath[512];

    // Ensure the path matches your structure
    snprintf(filepath, sizeof(filepath), "./Frontend/HTML/%s", filename); // Ensure "./" is added to denote current directory

    FILE *file = fopen(filepath, "r");
    if (!file) {
        // Log the missing file for debugging
        fprintf(stderr, "File not found: %s\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);
    char *content = malloc(length + 1);
    if (content) {
        fread(content, 1, length, file);
        content[length] = '\0';
    }
    fclose(file);
    return content;
}

// Utility function to write content to a file
void writeFile(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file) {
        fprintf(file, "%s", content);
        fclose(file);
    }
}
