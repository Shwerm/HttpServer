#include "handlers.h"
#include "files.h"
#include "auth.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Template for HTTP responses
static const char *responseTemplate =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: %d\r\n"
    "\r\n"
    "%s";

// Sends a 404 Not Found response
static void send404(SOCKET clientSocket) {
    const char *body = "<html><body><h1>404 Not Found</h1></body></html>";
    char response[MAX_BUFFER_SIZE];
    snprintf(response, sizeof(response), responseTemplate, (int)strlen(body), body);
    send(clientSocket, response, strlen(response), 0);
}

// Sends a generic error response
static void sendError(SOCKET clientSocket, const char *message) {
    char body[1024];
    snprintf(body, sizeof(body), "<html><body><h1>Error</h1><p>%s</p></body></html>", message);
    char response[MAX_BUFFER_SIZE];
    snprintf(response, sizeof(response), responseTemplate, (int)strlen(body), body);
    send(clientSocket, response, strlen(response), 0);
}

// Handles GET requests
static void handleGetRequest(SOCKET clientSocket, const char *path) {
    char *body = NULL;

    if (strcmp(path, "/") == 0 || strcmp(path, "/signin") == 0) {
        body = readFile("SignInPage/signin.html");
        if (!body) body = strdup("<html><body><h1>Sign-In Page Not Found</h1></body></html>");
    } else if (strcmp(path, "/create-account") == 0) {
        body = readFile("AccountCreationPage/create_account.html");
        if (!body) body = strdup("<html><body><h1>Account Creation Page Not Found</h1></body></html>");
    } else {
        send404(clientSocket);
        return;
    }

    char response[MAX_BUFFER_SIZE];
    snprintf(response, sizeof(response), responseTemplate, (int)strlen(body), body);
    send(clientSocket, response, strlen(response), 0);
    free(body);
}

// Handles POST requests
static void handlePostRequest(SOCKET clientSocket, const char *path, const char *body) {
    if (strcmp(path, "/create-account") == 0) {
        char username[256] = {0}, password[256] = {0};

        char *usernameStart = strstr(body, "username=");
        char *passwordStart = strstr(body, "password=");
        if (usernameStart && passwordStart) {
            sscanf(usernameStart, "username=%255[^&]", username);
            sscanf(passwordStart, "password=%255[^\r\n]", password);

            strtok(username, "\r\n");
            strtok(password, "\r\n");

            if (strlen(username) > 0 && strlen(password) > 0) {
                if (usernameExists(username)) {
                    const char *errorResponse =
                        "<html><body>\n"
                        "<h1>Create Account</h1>\n"
                        "<p style=\"color:red;\">Username already exists. Please choose another.</p>\n"
                        "</body></html>";
                    char response[MAX_BUFFER_SIZE];
                    snprintf(response, sizeof(response), responseTemplate, (int)strlen(errorResponse), errorResponse);
                    send(clientSocket, response, strlen(response), 0);
                } else {
                    int userID = generateUniqueID();
                    FILE *users = fopen("users.txt", "a");
                    if (users) {
                        fprintf(users, "%s:%s:%d\n", username, password, userID);
                        fclose(users);
                    }

                    const char *redirectTemplate = "HTTP/1.1 302 Found\r\nLocation: /signin\r\n\r\n";
                    send(clientSocket, redirectTemplate, strlen(redirectTemplate), 0);
                }
            } else {
                sendError(clientSocket, "Invalid input data.");
            }
        } else {
            sendError(clientSocket, "Invalid input data.");
        }
    } else if (strcmp(path, "/signin") == 0) {
        char username[256] = {0}, password[256] = {0};

        char *usernameStart = strstr(body, "username=");
        char *passwordStart = strstr(body, "password=");
        if (usernameStart && passwordStart) {
            sscanf(usernameStart, "username=%255[^&]", username);
            sscanf(passwordStart, "password=%255[^\r\n]", password);

            strtok(username, "\r\n");
            strtok(password, "\r\n");

            FILE *users = fopen("users.txt", "r");
            int authenticated = 0;
            if (users) {
                char line[512];
                while (fgets(line, sizeof(line), users)) {
                    char fileUser[256], filePass[256];
                    sscanf(line, "%255[^:]:%255[^:]:%*d", fileUser, filePass);
                    if (strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0) {
                        authenticated = 1;
                        break;
                    }
                }
                fclose(users);
            }

            if (authenticated) {
                const char *successResponse = "<html><body><h1>Authentication Successful</h1></body></html>";
                char response[MAX_BUFFER_SIZE];
                snprintf(response, sizeof(response), responseTemplate, (int)strlen(successResponse), successResponse);
                send(clientSocket, response, strlen(response), 0);
            } else {
                sendError(clientSocket, "Authentication Failed.");
            }
        } else {
            sendError(clientSocket, "Invalid input data.");
        }
    } else {
        send404(clientSocket);
    }
}

// Main client handler function
void clientHandler(SOCKET clientSocket) {
    char buffer[MAX_BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (bytesReceived <= 0) {
        closesocket(clientSocket);
        return;
    }
    buffer[bytesReceived] = '\0';

    char method[16], path[256];
    if (sscanf(buffer, "%15s %255s", method, path) != 2) {
        sendError(clientSocket, "Invalid HTTP request.");
        closesocket(clientSocket);
        return;
    }

    if (strcmp(method, "GET") == 0) {
        handleGetRequest(clientSocket, path);
    } else if (strcmp(method, "POST") == 0) {
        char *body = strstr(buffer, "\r\n\r\n");
        if (body) {
            body += 4;
            handlePostRequest(clientSocket, path, body);
        } else {
            sendError(clientSocket, "No body found in POST request.");
        }
    } else {
        send404(clientSocket);
    }

    closesocket(clientSocket);
}
