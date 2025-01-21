/* 
HTTP Server with User Authentication - Sam Camilleri
Utilities Header
*/
#ifndef UTILS_H
#define UTILS_H

#include <signal.h>

// Global flag for server shutdown
extern volatile int keepRunning;

// Sets up signal handlers for graceful shutdown
void setupSignalHandlers();

// Signal handler for graceful shutdown
void handleSignal(int signal);

// Logs an error message
// Parameters:
// - message: The error message to log
void logError(const char *message);

#endif // UTILS_H
