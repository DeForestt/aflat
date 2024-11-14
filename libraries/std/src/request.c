#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int request(char *host, char *path, char *port, char * msg, char * response, int response_size){
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = atoi(port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host");
        return 0;
    };
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    n = write(sockfd,msg,strlen(msg));
    if (n < 0)
         error("ERROR writing to socket");
    
    // read response
    bzero(response,response_size);
    n = read(sockfd,response,response_size-1);
    if (n < 0)
         error("ERROR reading from socket");
    close(sockfd);
    return 1;
}

#define SIZE 1024
#define BACKLOG 10  // Passed to listen()

int _aflat_server_spinUp(short port, int requestSize, char* (*requestHandler)(char*, char**)) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //inet_addr("127.0.0.1");
    bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    int listening = listen(serverSocket, BACKLOG);
    if (listening < 0) {
        printf("Error: The server is not listening.\n");
        return 1;
    }
    int clientSocket;

    while(1) {
        clientSocket = accept(serverSocket, NULL, NULL);
        char request[requestSize];
        char** response = malloc(sizeof(char*));
        bzero(request, requestSize);
        read(clientSocket, &request, sizeof(request) - 1);
        requestHandler(request, response);
        send(clientSocket, *response, strlen(*response), 0);
        free(response);
    }
    return 0;
}

// Server side C/C++ program to demonstrate Socket
// programming

#include <unistd.h>
#define PORT 8080

int _serve(int port, char *(*handler)(char *, void *), void *data) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    read(new_socket, buffer, 1024);
    char *response = handler(buffer, data);
    send(new_socket, response, strlen(response), 0);
    close(new_socket);
    close(server_fd);
    return 0;
}

int serve(int port, char *(*handler)(char *, void *), void *data) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    while (1) {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {  // Child process
            close(server_fd);
            char buffer[1024] = {0};
            read(new_socket, buffer, 1024);
            char *response = handler(buffer, data);
            send(new_socket, response, strlen(response), 0);
            close(new_socket);
            exit(0);  // End child process
        } else {
            close(new_socket);  // Parent process closes the new socket
        }
    }
    close(server_fd);
    return 0;
}