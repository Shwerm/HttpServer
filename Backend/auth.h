/* 
HTTP Server with User Authentication - Sam Camilleri
Authentication Header
*/
#ifndef AUTH_H
#define AUTH_H

// Checks if a username already exists in the database
// Parameters:
// - username: The username to check
// Returns: 1 if the username exists, 0 otherwise
int usernameExists(const char *username);

// Generates a unique identifier for a new user
// Returns: The next available unique identifier
int generateUniqueID();

#endif // AUTH_H
