#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static int _parse_content_length(const char *buffer, size_t header_len,
                                 size_t *content_length) {
  const char *needle = "content-length:";
  size_t needle_len = strlen(needle);

  for (size_t i = 0; i + needle_len <= header_len; ++i) {
    size_t j = 0;
    for (; j < needle_len; ++j) {
      unsigned char current = buffer[i + j];
      if (isupper(current)) current = (unsigned char)tolower(current);
      if (current != (unsigned char)needle[j]) break;
    }
    if (j == needle_len) {
      size_t pos = i + needle_len;
      while (pos < header_len &&
             (buffer[pos] == ' ' || buffer[pos] == '\t'))  // skip ws
        pos++;

      size_t value = 0;
      int found_digit = 0;
      while (pos < header_len && buffer[pos] >= '0' && buffer[pos] <= '9') {
        found_digit = 1;
        value = value * 10 + (size_t)(buffer[pos] - '0');
        pos++;
      }
      if (found_digit) {
        *content_length = value;
        return 1;
      }
      break;
    }
  }

  *content_length = 0;
  return 0;
}

static ssize_t _aflat_read_request(int socket_fd, size_t initial_size,
                                   char **out_request) {
  size_t capacity = initial_size > 0 ? initial_size : 4096;
  if (capacity < 1024) capacity = 1024;

  char *buffer = malloc(capacity);
  if (!buffer) return -1;

  size_t total = 0;
  ssize_t header_end = -1;
  size_t content_length = 0;

  while (1) {
    if (total + 1 >= capacity) {
      size_t new_capacity = capacity * 2;
      char *tmp = realloc(buffer, new_capacity);
      if (!tmp) {
        free(buffer);
        return -1;
      }
      buffer = tmp;
      capacity = new_capacity;
    }

    ssize_t bytes = read(socket_fd, buffer + total, capacity - total - 1);
    if (bytes < 0) {
      free(buffer);
      return -1;
    }
    if (bytes == 0) break;

    total += (size_t)bytes;
    buffer[total] = '\0';

    if (header_end == -1) {
      char *header_boundary = strstr(buffer, "\r\n\r\n");
      if (header_boundary != NULL) {
        header_end = (ssize_t)(header_boundary - buffer + 4);
        _parse_content_length(buffer, (size_t)header_end, &content_length);
      }
    }

    if (header_end != -1) {
      size_t required_total = (size_t)header_end + content_length;
      if (total >= required_total) break;
    }
  }

  buffer[total] = '\0';
  *out_request = buffer;
  return (ssize_t)total;
}

void error(const char *msg) {
  perror(msg);
  exit(0);
}

int request(char *host, char *path, char *port, char *msg, char *response,
            int response_size) {
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  portno = atoi(port);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");
  server = gethostbyname(host);
  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host");
    return 0;
  };
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting");

  n = write(sockfd, msg, strlen(msg));
  if (n < 0) error("ERROR writing to socket");

  // read response
  bzero(response, response_size);
  n = read(sockfd, response, response_size - 1);
  if (n < 0) error("ERROR reading from socket");
  close(sockfd);
  return 1;
}

#define SIZE 1024
#define BACKLOG 10  // Passed to listen()

int _aflat_server_spinUp(short port, int requestSize,
                         char *(*requestHandler)(char *, char **)) {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr =
      htonl(INADDR_LOOPBACK);  // inet_addr("127.0.0.1");
  bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

  int listening = listen(serverSocket, BACKLOG);
  if (listening < 0) {
    printf("Error: The server is not listening.\n");
    return 1;
  }
  int clientSocket;

  while (1) {
    clientSocket = accept(serverSocket, NULL, NULL);
    char *request = NULL;
    char **response = malloc(sizeof(char *));
    if (_aflat_read_request(clientSocket, (size_t)requestSize, &request) < 0) {
      close(clientSocket);
      free(response);
      continue;
    }
    requestHandler(request, response);
    send(clientSocket, *response, strlen(*response), 0);
    free(request);
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

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
             sizeof(opt));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  listen(server_fd, 3);

  new_socket =
      accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
  if (new_socket < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  char *request = NULL;
  if (_aflat_read_request(new_socket, 4096, &request) < 0) {
    close(new_socket);
    close(server_fd);
    return -1;
  }
  char *response = handler(request, data);
  send(new_socket, response, strlen(response), 0);
  free(request);
  close(new_socket);
  close(server_fd);
  return 0;
}

int serve(int port, char *(*handler)(char *, void *), void *data) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
             sizeof(opt));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  listen(server_fd, 3);

  while (1) {
    int new_socket =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
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
      char *request = NULL;
      if (_aflat_read_request(new_socket, 4096, &request) < 0) {
        close(new_socket);
        exit(EXIT_FAILURE);
      }
      char *response = handler(request, data);
      send(new_socket, response, strlen(response), 0);
      free(request);
      close(new_socket);
      exit(0);  // End child process
    } else {
      close(new_socket);  // Parent process closes the new socket
    }
  }
  close(server_fd);
  return 0;
}

int serve_sync(int port, char *(*handler)(char *, void *), void *data) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
             sizeof(opt));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  listen(server_fd, 3);

  while (1) {
    int new_socket =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0) {
      perror("accept");
      continue;
    }

    char *request = NULL;
    if (_aflat_read_request(new_socket, 4096, &request) < 0) {
      close(new_socket);
      continue;
    }
    char *response = handler(request, data);
    send(new_socket, response, strlen(response), 0);
    free(request);
    close(new_socket);
  }
  close(server_fd);
  return 0;
}
