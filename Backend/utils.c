/* 
HTTP Server with User Authentication - Sam Camilleri
Entry point
Dependencies: utils.h
*/
#include "utils.h"
#include <signal.h>
#include <stdio.h>

volatile int keepRunning = 1;

void setupSignalHandlers() {
    signal(SIGINT, handleSignal);
}

void handleSignal(int signal) {
    printf("\nReceived termination signal. Shutting down server...\n");
    keepRunning = 0;
}

void logError(const char *message) {
    fprintf(stderr, "%s\n", message);
}
