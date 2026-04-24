#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <spawn.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

extern char **environ;

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

static int _header_is(const char *header, const char *name) {
  size_t name_len = strlen(name);
  for (size_t i = 0; i < name_len; ++i) {
    unsigned char a = (unsigned char)header[i];
    unsigned char b = (unsigned char)name[i];
    if (tolower(a) != tolower(b)) return 0;
  }
  return header[name_len] == ':';
}

static char *_dup_range(const char *start, size_t len) {
  char *out = malloc(len + 1);
  if (!out) return NULL;
  memcpy(out, start, len);
  out[len] = '\0';
  return out;
}

static char *_dup_trimmed_line(const char *start, size_t len) {
  while (len > 0 && (start[len - 1] == '\r' || start[len - 1] == '\n')) len--;
  return _dup_range(start, len);
}

static char *_build_url(const char *host, const char *port, const char *path) {
  const char *safe_path = path ? path : "/";
  const char *safe_port = port ? port : "";
  if (safe_path[0] == '\0') safe_path = "/";
  if (strncmp(safe_path, "http://", 7) == 0 ||
      strncmp(safe_path, "https://", 8) == 0) {
    return _dup_range(safe_path, strlen(safe_path));
  }

  size_t need_slash = safe_path[0] == '/' ? 0 : 1;
  if (strncmp(host, "http://", 7) == 0 || strncmp(host, "https://", 8) == 0) {
    size_t len = strlen(host) + need_slash + strlen(safe_path) + 1;
    char *url = malloc(len);
    if (!url) return NULL;
    char *write = url;
    size_t host_len = strlen(host);
    memcpy(write, host, host_len);
    write += host_len;
    if (need_slash) {
      *write++ = '/';
    }
    size_t path_len = strlen(safe_path);
    memcpy(write, safe_path, path_len);
    write += path_len;
    *write = '\0';
    return url;
  }

  const char *scheme =
      (safe_port[0] != '\0' && strcmp(safe_port, "80") == 0) ? "http"
                                                              : "https";
  int include_port =
      safe_port[0] != '\0' &&
      !((strcmp(scheme, "http") == 0 && strcmp(safe_port, "80") == 0) ||
        (strcmp(scheme, "https") == 0 && strcmp(safe_port, "443") == 0));
  size_t len = strlen(scheme) + 3 + strlen(host) +
               (include_port ? 1 + strlen(safe_port) : 0) + need_slash +
               strlen(safe_path) + 1;
  char *url = malloc(len);
  if (!url) return NULL;
  char *write = url;
  size_t scheme_len = strlen(scheme);
  size_t host_len = strlen(host);
  size_t path_len = strlen(safe_path);
  size_t port_len = strlen(safe_port);

  memcpy(write, scheme, scheme_len);
  write += scheme_len;
  memcpy(write, "://", 3);
  write += 3;
  memcpy(write, host, host_len);
  write += host_len;
  if (include_port) {
    *write++ = ':';
    memcpy(write, safe_port, port_len);
    write += port_len;
  }
  if (need_slash) {
    *write++ = '/';
  }
  memcpy(write, safe_path, path_len);
  write += path_len;
  *write = '\0';
  return url;
}

int request(char *host, char *path, char *port, char *msg, char *response,
            int response_size) {
  int pipefd[2];
  pid_t pid;
  int status = 0;
  ssize_t bytes = 0;
  size_t total = 0;
  const char *header_end;
  const char *body;
  const char *line_start;
  const char *line_end;
  char *msg_copy = NULL;
  char *method = NULL;
  char *url = NULL;
  char *headers[128];
  char *argv[270];
  int header_count = 0;
  int argc = 0;

  if (host == NULL || port == NULL || msg == NULL || response == NULL ||
      response_size <= 0) {
    fprintf(stderr, "ERROR, invalid request arguments\n");
    return 0;
  }

  bzero(response, response_size);
  for (size_t i = 0; i < sizeof(headers) / sizeof(headers[0]); ++i) {
    headers[i] = NULL;
  }

  header_end = strstr(msg, "\r\n\r\n");
  body = header_end ? header_end + 4 : msg + strlen(msg);

  line_end = strstr(msg, "\r\n");
  if (!line_end) {
    fprintf(stderr, "ERROR, invalid HTTP request line\n");
    return 0;
  }

  msg_copy = _dup_range(msg, (size_t)(line_end - msg));
  if (!msg_copy) {
    fprintf(stderr, "ERROR, out of memory\n");
    return 0;
  }
  method = strtok(msg_copy, " ");
  if (!method) {
    fprintf(stderr, "ERROR, invalid HTTP method\n");
    free(msg_copy);
    return 0;
  }

  url = _build_url(host, port, path);
  if (!url) {
    fprintf(stderr, "ERROR, out of memory\n");
    free(msg_copy);
    return 0;
  }

  line_start = line_end + 2;
  while (header_end && line_start < header_end && header_count < 127) {
    line_end = strstr(line_start, "\r\n");
    if (!line_end || line_end == line_start) break;

    char *header = _dup_trimmed_line(line_start, (size_t)(line_end - line_start));
    if (!header) break;
    if (!_header_is(header, "Host") && !_header_is(header, "Content-Length")) {
      headers[header_count++] = header;
    } else {
      free(header);
    }
    line_start = line_end + 2;
  }

  argv[argc++] = "curl";
  argv[argc++] = "--silent";
  argv[argc++] = "--show-error";
  argv[argc++] = "--include";
  argv[argc++] = "--http1.1";
  argv[argc++] = "--request";
  argv[argc++] = method;
  argv[argc++] = "--url";
  argv[argc++] = url;

  for (int i = 0; i < header_count; ++i) {
    argv[argc++] = "-H";
    argv[argc++] = headers[i];
  }

  if (header_end && body[0] != '\0') {
    argv[argc++] = "--data-binary";
    argv[argc++] = (char *)body;
  }

  argv[argc] = NULL;

  if (pipe(pipefd) != 0) {
    perror("pipe");
    free(url);
    free(msg_copy);
    for (int i = 0; i < header_count; ++i) free(headers[i]);
    return 0;
  }

  posix_spawn_file_actions_t actions;
  if (posix_spawn_file_actions_init(&actions) != 0) {
    perror("posix_spawn_file_actions_init");
    close(pipefd[0]);
    close(pipefd[1]);
    free(url);
    free(msg_copy);
    for (int i = 0; i < header_count; ++i) free(headers[i]);
    return 0;
  }

  posix_spawn_file_actions_adddup2(&actions, pipefd[1], STDOUT_FILENO);
  posix_spawn_file_actions_adddup2(&actions, pipefd[1], STDERR_FILENO);
  posix_spawn_file_actions_addclose(&actions, pipefd[0]);
  posix_spawn_file_actions_addclose(&actions, pipefd[1]);

  int spawn_status = posix_spawnp(&pid, "curl", &actions, NULL, argv, environ);
  posix_spawn_file_actions_destroy(&actions);
  if (spawn_status != 0) {
    errno = spawn_status;
    perror("posix_spawnp");
    close(pipefd[0]);
    close(pipefd[1]);
    free(url);
    free(msg_copy);
    for (int i = 0; i < header_count; ++i) free(headers[i]);
    return 0;
  }

  close(pipefd[1]);
  while (total + 1 < (size_t)response_size) {
    bytes = read(pipefd[0], response + total, (size_t)response_size - total - 1);
    if (bytes < 0) {
      close(pipefd[0]);
      free(url);
      free(msg_copy);
      for (int i = 0; i < header_count; ++i) free(headers[i]);
      return 0;
    }
    if (bytes == 0) break;
    total += (size_t)bytes;
  }
  response[total] = '\0';
  close(pipefd[0]);

  waitpid(pid, &status, 0);

  free(url);
  free(msg_copy);
  for (int i = 0; i < header_count; ++i) free(headers[i]);

  return WIFEXITED(status) && WEXITSTATUS(status) == 0;
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
