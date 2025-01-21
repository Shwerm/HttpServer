/* 
HTTP Server with User Authentication - Sam Camilleri
Entry point
Dependencies: server.h, utils.h
*/

#include "server.h"
#include "utils.h"

int main() 
{
    setupSignalHandlers(); // Set up signal handlers
    startServer();         // Start the server
    return 0;
}
