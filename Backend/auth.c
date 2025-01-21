/* 
HTTP Server with User Authentication - Sam Camilleri
Entry point
Dependencies: auth.h
*/

#include "auth.h"
#include <stdio.h>
#include <string.h>

// Checks if a username already exists in the database
// Parameters:
// - username: The username to check
// Returns: 1 if the username exists, 0 otherwise
int usernameExists(const char *username) {
    FILE *users = fopen("users.txt", "r");
    if (!users) return 0;

    char line[512], fileUser[256];
    while (fgets(line, sizeof(line), users)) {
        sscanf(line, "%255[^:]:", fileUser);
        if (strcmp(username, fileUser) == 0) {
            fclose(users);
            return 1;
        }
    }

    fclose(users);
    return 0;
}

// Generates a unique identifier for a new user
// Returns: The next available unique identifier
int generateUniqueID() {
    FILE *users = fopen("users.txt", "r");
    if (!users) return 1; // Start IDs from 1 if the file doesn't exist

    char line[512];
    int maxID = 0, currentID;
    while (fgets(line, sizeof(line), users)) {
        sscanf(line, "%*[^:]:%*[^:]:%d", &currentID);
        if (currentID > maxID) {
            maxID = currentID;
        }
    }

    fclose(users);
    return maxID + 1;
}
