/* Http Server - Sam Camilleri */

// Include statements
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// Pragma statements
#pragma comment(lib, "ws2_32.lib") // Linking the ws2_32.lib library

// Constants
#define DEFAULT_PORT 8080
#define MAX_BUFFER_SIZE 4096

// Global Variable for Graceful Shutdown
volatile int keepRunning = 1;

// Signal Handler for Graceful Shutdown
void handleSignal(int signal) {
    printf("\nReceived termination signal. Shutting down server...\n");
    keepRunning = 0;
}

// Client Handler Function
void clientHandler(SOCKET clientSocket)
{
    char buffer[MAX_BUFFER_SIZE];
    int bytesReceived;

    // Receive HTTP request
    bytesReceived = recv(clientSocket, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (bytesReceived == SOCKET_ERROR) {
        printf("Failed to receive data. Error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        return;
    }

    // Null-terminate the buffer
    buffer[bytesReceived] = '\0';

    // Log the raw received data (in hex) for debugging
    printf("Raw received data (Hex): ");
    for (int i = 0; i < bytesReceived; i++) {
        printf("%02X ", (unsigned char)buffer[i]);
    }
    printf("\n");

    // Validate if the request starts with an HTTP method
    if (strncmp(buffer, "GET", 3) != 0 && strncmp(buffer, "POST", 4) != 0 &&
        strncmp(buffer, "HEAD", 4) != 0 && strncmp(buffer, "PUT", 3) != 0 &&
        strncmp(buffer, "DELETE", 6) != 0 && strncmp(buffer, "OPTIONS", 7) != 0) {
        printf("Ignored non-HTTP request.\n");
        closesocket(clientSocket);
        return;
    }

    // Print the HTTP request for valid HTTP traffic
    printf("Received HTTP request:\n%s\n", buffer);

    // Filter out favicon.ico requests
    if (strstr(buffer, "GET /favicon.ico") != NULL) {
        printf("Ignored request for favicon.ico\n");
        closesocket(clientSocket);
        return;
    }

    // Log client connection details
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    getpeername(clientSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    printf("Connection from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // Prepare an HTTP response
    const char *responseTemplate =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s";

    const char *body = "<html><body><h1>Hello, World!</h1></body></html>";
    char response[MAX_BUFFER_SIZE];
    snprintf(response, sizeof(response), responseTemplate, strlen(body), body);

    // Send the response
    int bytesSent = send(clientSocket, response, strlen(response), 0);
    if (bytesSent == SOCKET_ERROR) {
        printf("Failed to send data. Error: %d\n", WSAGetLastError());
    } else {
        printf("Sent HTTP response:\n%s\n", response);
    }

    // Close the client socket
    if (closesocket(clientSocket) == SOCKET_ERROR) {
        printf("Failed to close socket. Error: %d\n", WSAGetLastError());
    }
}

// Main Function
int main()
{
    WSADATA wsaData;
    SOCKET server_fd, clientSocket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Handle signals for graceful shutdown
    signal(SIGINT, handleSignal);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(DEFAULT_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed. Error: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Listen failed. Error: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server is running on port %d. Press Ctrl+C to stop.\n", DEFAULT_PORT);

    // Accept and handle incoming connections
    while (keepRunning) {
        clientSocket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (clientSocket == INVALID_SOCKET) {
            printf("Accept failed. Error: %d\n", WSAGetLastError());
            continue;
        }
        clientHandler(clientSocket);
    }

    // Cleanup on shutdown
    closesocket(server_fd);
    WSACleanup();
    printf("Server shut down gracefully.\n");
    return 0;
}
