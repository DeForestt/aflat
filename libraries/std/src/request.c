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
        bzero(response, requestSize);
        read(clientSocket, &request, sizeof(request) - 1);
        requestHandler(request, response);
        send(clientSocket, *response, strlen(*response), 0);
        close(clientSocket);
        free(response);
    }
    return 0;
}

// int main(int argc, char *argv[])
// {
//     char *host = "api.aflatlang.com";
//     char *path = "/";
//     char *port = "80";
//     char *msg = "GET / HTTP/1.1\r\n"
//     "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
//     "Host: www.tutorialspoint.com\r\n"
//     "Accept-Language: en-us\r\n"
//     "Accept-Encoding: gzip, deflate\r\n"
//     "Connection: Keep-Alive\r\n\r\n";
//     char response[10000];
//     request(host, path, port, msg, response, 10000);
//     printf("%s", response);
//     return 0;
// }
