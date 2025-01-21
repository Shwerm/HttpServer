/* HTTP Server with User Authentication */

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library for networking

#define DEFAULT_PORT 8080 // Default port for the HTTP server
#define MAX_BUFFER_SIZE 4096 // Maximum buffer size for request handling

volatile int keepRunning = 1; // Global flag for server shutdown

// Signal handler for graceful shutdown
void handleSignal(int signal) {
    printf("\nReceived termination signal. Shutting down server...\n");
    keepRunning = 0;
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

// Utility function to read content from a file
// Parameters:
// - filename: Name of the file to read
// Returns: Pointer to the dynamically allocated content, or NULL on failure
char *readFile(const char *filename) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "html/%s", filename);
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

// Handles incoming client connections
// Parameters:
// - clientSocket: Socket representing the client connection
void clientHandler(SOCKET clientSocket) {
    char buffer[MAX_BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (bytesReceived == SOCKET_ERROR) {
        closesocket(clientSocket);
        return;
    }
    buffer[bytesReceived] = '\0'; // Null-terminate the received data

    const char *responseTemplate =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s";

    const char *redirectTemplate =
        "HTTP/1.1 302 Found\r\n"
        "Location: /signin\r\n"
        "\r\n";

    char *body;
    // Serve the sign-in page
    if (strstr(buffer, "GET / ") || strstr(buffer, "GET /signin")) {
        body = readFile("signin.html");
        if (!body) body = "<html><body><h1>Sign-In Page Not Found</h1></body></html>";
    } 
    // Serve the account creation page
    else if (strstr(buffer, "GET /create-account")) {
        body = readFile("create_account.html");
        if (!body) body = "<html><body><h1>Account Creation Page Not Found</h1></body></html>";
    } 
    // Handle account creation via POST
    else if (strstr(buffer, "POST /create-account")) {
        char username[256], password[256];
        sscanf(buffer, "username=%255[^&]&password=%255s", username, password);
        FILE *users = fopen("users.txt", "a");
        if (users) {
            fprintf(users, "%s:%s\n", username, password);
            fclose(users);
        }
        // Redirect to the sign-in page after account creation
        send(clientSocket, redirectTemplate, strlen(redirectTemplate), 0);
        closesocket(clientSocket);
        return;
    } 
    // Handle sign-in via POST
    else if (strstr(buffer, "POST /signin")) {
        char username[256], password[256], line[512];
        sscanf(buffer, "username=%255[^&]&password=%255s", username, password);
        FILE *users = fopen("users.txt", "r");
        int authenticated = 0;
        if (users) {
            while (fgets(line, sizeof(line), users)) {
                char fileUser[256], filePass[256];
                sscanf(line, "%255[^:]:%255s", fileUser, filePass);
                if (strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0) {
                    authenticated = 1;
                    break;
                }
            }
            fclose(users);
        }
        if (authenticated) {
            body = "<html><body><h1>Authentication Successful</h1></body></html>";
        } else {
            body = "<html><body><h1>Authentication Failed</h1></body></html>";
        }
    } 
    // Handle unknown requests
    else {
        body = "<html><body><h1>404 Not Found</h1></body></html>";
    }

    char response[MAX_BUFFER_SIZE];
    snprintf(response, sizeof(response), responseTemplate, strlen(body), body);

    send(clientSocket, response, strlen(response), 0);

    if (body != NULL) free(body);
    closesocket(clientSocket);
}

// Entry point for the HTTP server
int main() {
    WSADATA wsaData;
    SOCKET server_fd, clientSocket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Set up signal handling for graceful shutdown
    signal(SIGINT, handleSignal);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return 1;
    }

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    // Bind the socket to the specified port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(DEFAULT_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    listen(server_fd, 3);

    printf("Server is running on port %d. Press Ctrl+C to stop.\n", DEFAULT_PORT);

    // Accept and handle client connections
    while (keepRunning) {
        clientSocket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (clientSocket == INVALID_SOCKET) continue;
        clientHandler(clientSocket);
    }

    // Cleanup resources on shutdown
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
