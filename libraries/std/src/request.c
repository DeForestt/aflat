// ping google.com

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

int _serve(int port, char *(*handler)(char *, void *), void * data){
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "Hello from server";
  
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
  
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, buffer, 1024);
    char * response = handler(buffer, data);

    send(new_socket, response, strlen(response), 0);
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

int serve(int port, char *(*handler)(char *, void *), void * data){
    while(1){
        _serve(port, handler, data);
    }
    return 0;
}