For reference, when I say enter "cd /tmp", I mean to enter the phrase without the 
quotation marks. So for "cd /tmp", I mean for you to type cd /tmp

////How To Run////:
- There is a makefile where if "make" is entered, the files are compiled.
- Entering "make run" will actually run the program
- Entering "make clean" will clean the compiled files
- My makefile runs ./PrimaryLikesServer & ./ParentProcess, but for some reason, ctrl+c
  does not close PrimaryLikesServer after ParentProcess is completed
- ParentProcess will close out normally though once the time set is done (5 min)
- To close ./PrimaryLikesServer, you'll have to enter the command 
  "ps aux | grep ./PrimaryLikesServer". This will display something like:

vboxuser   17852  0.0  0.0   2364   556 pts/1    S    16:58   0:00 ./PrimaryLikesServer
vboxuser   18160  0.0  0.0   6376   704 pts/1    S+   17:05   0:00 grep ./PrimaryLikesServer

- You would need to look for ./PrimaryLikesServer all the way to the right, and in the 
  same row look for the first number starting from the left hand side (17852 in this 
  example)
- Then enter "kill PID", where you replace PID with the number from the previous 
  command (17852 in this example)
- If you don't do this, when doing a new run, the error message will pop up saying 
  failed to bind socket. However, the program will still run because the server socket 
  is still open from the previous run. As a result, likes from the previous run will 
  carry over to the new run

////Format of Log Files and How To Access Them////:
- Invalid data is not logged in the PrimaryLikes.log, however, it will be logged in
  the individual LikeServer logs. So if LikeServer 0 sent invalid data, the invalid 
  data would be logged in LikeServer0.log, but not PrimaryLikesServer.log
- Different runs are separated by a newline for readability
- The log files are stored in the /tmp directory, so you need to enter "cd /tmp" to 
  access all the log files
- The ParentProcessStatus.log contains information on each child process and states
  what time they start at, as well as what time they end and what their exit status
  was. Below is some sample output:

Child 0 started at Sat Nov 11 22:59:55 2023
Child 1 started at Sat Nov 11 22:59:56 2023
Child 2 started at Sat Nov 11 22:59:57 2023
Child 3 started at Sat Nov 11 22:59:58 2023
Child 4 started at Sat Nov 11 22:59:59 2023
Child 5 started at Sat Nov 11 23:00:01 2023
Child 6 started at Sat Nov 11 23:00:02 2023
Child 7 started at Sat Nov 11 23:00:03 2023
Child 8 started at Sat Nov 11 23:00:04 2023
Child 9 started at Sat Nov 11 23:00:05 2023
Child 0 finished with with exit status 0 at Sat Nov 11 23:00:07 2023
Child 1 finished with with exit status 0 at Sat Nov 11 23:00:08 2023
Child 2 finished with with exit status 0 at Sat Nov 11 23:00:08 2023
Child 3 finished with with exit status 0 at Sat Nov 11 23:00:09 2023
Child 4 finished with with exit status 0 at Sat Nov 11 23:00:09 2023
Child 5 finished with with exit status 0 at Sat Nov 11 23:00:13 2023
Child 6 finished with with exit status 0 at Sat Nov 11 23:00:13 2023
Child 7 finished with with exit status 0 at Sat Nov 11 23:00:15 2023
Child 8 finished with with exit status 0 at Sat Nov 11 23:00:17 2023
Child 9 finished with with exit status 0 at Sat Nov 11 23:00:17 2023

- The PrimaryLikes.log stores what data is being passed by each LikesServer, as well
  as the current total number of likes it has. Below is some sample output:

LikeServer2 4
Total: 4
LikeServer0 25
Total: 29
LikeServer3 27
Total: 56
LikeServer0 5
Total: 61
LikeServer2 22
Total: 83

- LikeServer2 4 indicates LikeServer 2 sent 4 likes. Total: 4 indicates the
  current total number of likes is 4, and this will change as more LikeServers send 
  more likes
- There are logs for each LikeServer with their names being LikeServer#.log, where 
  the hashtag gets replaced by the child server number. The logs for each LikeServer 
  logs the data they pass, as well as another number to indicate their current status. 
  Below is some sample input for LikeServer0.log:

LikeServer0 25 0
LikeServer0 5 0
LikeServer0 34 0
LikeServer0 27 0
LikeServer0 0 0

- LikeServer0 25 0 indicates LikeServer 0 sent 25 likes, and did not encounter any
  issues. If It was LikeServer 0 25 -1, then it would mean errors were encountered, 
  and the child process should end

////What It Does////:
- PrimaryLikesServer opens a socket and takes in data from ParentProcess
- ParentProcess creates 10 children, and the start times, end times, and exit status
  of these children will be logged
- For 5 min, the children will randomly generate a random number of likes between 0 and
  42.
- The children will then wait a random interval between 1 and 5 seconds and then send
  their likes over to PrimaryLikesServer
- The PrimaryLikesServer will parse this data, and if it is valid, logs it and adds
  the number of likes given to it's current running total number of likes
- Once this is done, the data sent by each children will be logged in their own logs

////AI Usage/Prompt////:
- I asked chatGPT for exmaple code and modified it to suit my project. Below is the
  prompt and the response:

My Question:
code to set up a socket between two c files, where one file sends a phrase to the 
other?


Response:
Setting up a socket connection between two C files where one sends a message to the 
other involves creating a server and a client. Here's a basic example of how you can 
achieve this using sockets in C. In this example, one C file acts as the server, and 
the other as the client.

Server (Receiving End):

c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    char buffer[1024] = {0};
    const char *response = "Hello from the server!";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding the socket to the specified address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Acceptance failed");
        exit(EXIT_FAILURE);
    }

    read(new_socket, buffer, sizeof(buffer));
    printf("Client: %s\n", buffer);

    send(new_socket, response, strlen(response), 0);
    printf("Response sent to the client\n");

    return 0;
}

Client (Sending End):

c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    const char *message = "Hello from the client!";

    // Creating socket file descriptor
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    send(client_socket, message, strlen(message), 0);
    printf("Message sent to the server\n");

    read(client_socket, buffer, sizeof(buffer));
    printf("Server: %s\n", buffer);

    return 0;
}

Make sure to compile the server and client separately and run the server first. This 
example assumes both the server and client are running on the same machine, and the 
server is listening on port 8080. You can modify the IP address and port as needed.
