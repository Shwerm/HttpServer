/* 
HTTP Server with User Authentication - Sam Camilleri
File reader Header
*/
#ifndef FILES_H
#define FILES_H

// Reads content from a file
// Parameters:
// - filename: Name of the file to read
// Returns: Pointer to the dynamically allocated content, or NULL on failure
char *readFile(const char *filename);

// Writes content to a file
// Parameters:
// - filename: Name of the file to write
// - content: Content to write to the file
void writeFile(const char *filename, const char *content);

#endif // FILES_H

