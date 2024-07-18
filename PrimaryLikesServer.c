/*
project: 02
author: Hasnain Ali
email: hali6@umbc.edu
student id: jl69013
description: a server that takes in data from children processes created in
ParentProcess
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Function Declarations
void addNewLineToLog();
void createSocket();
void handleClient(int clientSocket);
void primaryLikesLog(const char* clientInfo, int likes);

// Global Variables
int totalLikes = 0;      // Used to keep track of total likes in the server

int main() {
    addNewLineToLog();
    createSocket();
    return 0;
}

// Adds a newline to the primary likes log for readablity between runs
void addNewLineToLog() {
    // Attempts to open the primary likes log in append mode
    FILE *logFile = fopen("/tmp/PrimaryLikes.log", "a");
    // adds a newline
    fprintf(logFile, "\n");
    fclose(logFile);
}

// Creates the socket that the children processes will connect to
void createSocket() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Error handling
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Set the SO_REUSEADDR option
    int enable = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        perror("Setsockopt failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    // Error handling
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Binding failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    // Error handling
    if (listen(serverSocket, 10) == -1) {
        perror("Listening failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port 8080...\n");
    while (1) {
        int clientSocket = accept(serverSocket, NULL, NULL);
	// Error handling
        if (clientSocket == -1) {
            perror("Acceptance failed");
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        // Handle the client in a separate function
        handleClient(clientSocket);
    }
    // Never gets to doing
    close(serverSocket);
}

// Handles the data a client sends
void handleClient(int clientSocket) {
    // Receive data from the client
    char clientInfo[1024];
    ssize_t bytesReceived = recv(clientSocket, clientInfo, sizeof(clientInfo) - 1, 0);
    // Null terminate the client info
    clientInfo[bytesReceived] = '\0';
    // Check the format of the received data
    int childNumber, likes;
    if (sscanf(clientInfo, "LikeServer%d %d", &childNumber, &likes) == 2) {
	// If the data is valid
        if (childNumber >= 0 && childNumber <= 9 && likes >= 0 && likes <= 42) {
	    printf("Server received valid data from the client: %s\n", clientInfo);
            // Data gets logged
	    primaryLikesLog(clientInfo, likes);
        }
	else {
	    printf("Server received invalid data from the client: %s\n", clientInfo);
	}
    }
    else {
	printf("Server received invalid data from the client: %s\n", clientInfo);
    }
    // Close the client socket
    close(clientSocket);
}

// Logs the info being added to the PrimaryLikesServer
void primaryLikesLog(const char* clientInfo, int likes) {
    totalLikes = totalLikes + likes;
    // Create a string with the total likes
    char totalLikesEntry[50];
    sprintf(totalLikesEntry, "Total: %d", totalLikes);
    // Attempts to open the primary likes log in append mode
    FILE *logFile = fopen("/tmp/PrimaryLikes.log", "a");
    // If it can't
    if (logFile == NULL) {
        // File doesn't exist, create it
        logFile = fopen("/tmp/PrimaryLikes.log", "w");
        // Error handling
        if (logFile == NULL) {
            perror("Error opening log file");
            exit(EXIT_FAILURE);
        }
    }
    // Appends the entry and the total likes entry
    fprintf(logFile, "%s\n", clientInfo);
    fprintf(logFile, "%s\n", totalLikesEntry);
    fclose(logFile);
}
