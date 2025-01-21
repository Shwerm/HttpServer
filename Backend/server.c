/* 
HTTP Server with User Authentication - Sam Camilleri
Server implementation
Dependencies: server.h, handlers.h, utils.h
*/

#include "server.h"
#include "handlers.h"
#include "utils.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define DEFAULT_PORT 8080

static SOCKET server_fd;

void startServer() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        logError("WSAStartup failed.");
        return;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        logError("Socket creation failed.");
        WSACleanup();
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(DEFAULT_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        logError("Binding socket failed.");
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    listen(server_fd, 3);
    printf("Server running on port %d. Press Ctrl+C to stop.\n", DEFAULT_PORT);

    while (keepRunning) {
        clientSocket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (clientSocket == INVALID_SOCKET) continue;
        clientHandler(clientSocket);
    }

    closesocket(server_fd);
    WSACleanup();
}
