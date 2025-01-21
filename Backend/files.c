/* 
HTTP Server with User Authentication - Sam Camilleri
Entry point
Dependencies: files.h
*/
#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Utility function to read content from a file
// Parameters:
// - filename: Name of the file to read
// Returns: Pointer to the dynamically allocated content, or NULL on failure
char *readFile(const char *filename) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "Frontend/HTML/%s", filename);
    FILE *file = fopen(filepath, "r");
    if (!file) return NULL;
    
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
// Parameters:
// - filename: Name of the file to write
// - content: Content to write to the file
void writeFile(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file) {
        fprintf(file, "%s", content);
        fclose(file);
    }
}
