/*
project: 02
author: Hasnain Ali
email: hali6@umbc.edu
student id: jl69013
description: Creates 10 children processes that send data to PrimaryLikesServer
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Function Declarations
void addNewLineToLogs();
void createChildren();
void sendDataToServer(const char* data, int ServerNumber);
void parentProcessLog(const char* entry);
void likeServerLog(const char* entry, int serverNumber, int status);

int main() {
    addNewLineToLogs();
    createChildren();
    return 0;
}

// Adds a newline in each log file for each new run (makes readibility better)
void addNewLineToLogs() {
    // Open the parent process log in append mode
    FILE *logFile = fopen("/tmp/ParentProcessStatus.log", "a");
    // Adds a newline
    fprintf(logFile, "\n");
    fclose(logFile);
    for (int i = 0; i < 10; i++) {
        // Attempts to open the LikesServer log for a specific likesServer in append
        // mode
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "/tmp/LikeServer%d.log", i);
        logFile = fopen(filepath, "a");
        // Adds a newline
        fprintf(logFile, "\n");
        fclose(logFile);
    }
}

// Forks 10 children
void createChildren() {
    int i;
    for (i = 0; i < 10; i++) {
        // If we are not forking the first child, have a 1 second delay
        if (i > 0) {
            sleep(1);
        }
        pid_t child_pid = fork();
        // Error handling
        if (child_pid < 0) {
            perror("Error when forkng child process");
            exit(EXIT_FAILURE);
        }
        // In the child process
        else if (child_pid == 0) {
            time_t startTime, currentTime;
            // Get the time the process starts
            startTime = time(NULL);
	    srand((unsigned int)(time(NULL) + getpid()));
            // Enter a loop that occurs for a set amount of time (5 min)
            while (1) {
                // Get the current time and compare the child runtime to 5 min
                currentTime = time(NULL);
                if (currentTime - startTime >= 300) {
                    break;
                }
                // Generate random number of likes between 0 and 42
                int likes = rand() % 43;
                // Generate random interval between 1 and 5 seconds
                int interval = (rand() % 5) + 1;
                char data[100];
                snprintf(data, sizeof(data), "LikeServer%d %d", i, likes);
                sleep(interval);
		printf("%s\n", data);
                sendDataToServer(data, i);
		// If there were errors, the child process will already be terminated
		int status = 0;
		likeServerLog(data, i, status);
            }
	    // If no errors were encountered for the child, exits with a 0
            exit(EXIT_SUCCESS);
        }
        // In the parent process
        else {
            time_t currentTime = time(NULL);
            char logEntry[100];
            snprintf(logEntry, sizeof(logEntry), "Child %d started at %s", i, ctime(&currentTime));
            parentProcessLog(logEntry);
        }
    }
    // Wait for all child processes to finish
    for (int j = 0; j < 10; j++) {
        int status;
	pid_t terminated_pid = wait(&status);
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            time_t currentTime = time(NULL);
            char logEntry[100];
            snprintf(logEntry, sizeof(logEntry), "Child %d finished with with exit status %d at %s", j, exit_status, ctime(&currentTime));
            parentProcessLog(logEntry);
        }
    }
    printf("All child processes have finished.\n");
}

// Function to send data to the server using sockets
void sendDataToServer(const char* data, int serverNumber) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
	int status = -1;
	likeServerLog(data, serverNumber, status);
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Connection failed");
        close(clientSocket);
        int status = -1;
        likeServerLog(data, serverNumber, status);
        exit(EXIT_FAILURE);
    }
    // Send data to the server
    send(clientSocket, data, strlen(data), 0);
    close(clientSocket);
}

// Takes care of creating/appending to /tmp/ParentProcess.log
void parentProcessLog(const char* entry) {
    // Attempts to open the parent process log in append mode
    FILE *logFile = fopen("/tmp/ParentProcessStatus.log", "a");
    // If it can't
    if (logFile == NULL) {
        // File doesn't exist, create it
        logFile = fopen("/tmp/ParentProcessStatus.log", "w");
	// Error handling
        if (logFile == NULL) {
            perror("Error opening log file");
            exit(EXIT_FAILURE);
        }
    }
    // Appends the entry
    fprintf(logFile, "%s", entry);
    fclose(logFile);
}

// Takes care of creating/appending to /tmp/LikesServer(#).log
void likeServerLog(const char* entry, int serverNumber, int status) {
    // Attempts to open the LikesServer log for a specific likesServer in append mode
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "/tmp/LikeServer%d.log", serverNumber);
    FILE *logFile = fopen(filepath, "a");
    // If it can't
    if (logFile == NULL) {
        // File doesn't exist, create it
        logFile = fopen(filepath, "w");
    }
    // Appends the entry
    fprintf(logFile, "%s %d\n", entry, status);
    fclose(logFile);
}

