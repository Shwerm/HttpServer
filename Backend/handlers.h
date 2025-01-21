/* 
HTTP Server with User Authentication - Sam Camilleri
Handler Header
*/
#ifndef HANDLERS_H
#define HANDLERS_H

#include <winsock2.h>

// Maximum buffer size for request handling
#define MAX_BUFFER_SIZE 4096

// Handles incoming client connections
// Parameters:
// - clientSocket: Socket representing the client connection
void clientHandler(SOCKET clientSocket);

#endif // HANDLERS_H
