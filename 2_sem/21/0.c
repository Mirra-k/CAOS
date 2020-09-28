#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
    char* host_name = argv[1];
    char* path = argv[2];

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    int s = getaddrinfo(host_name, "http", &hints, &result);
    if (s != 0) {
        exit(EXIT_FAILURE);
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        exit(EXIT_FAILURE);
    }

    int connect_ret = connect(socket_fd, result->ai_addr, result->ai_addrlen);
    if (connect_ret == -1) {
        exit(EXIT_FAILURE);
    }

    char request[2048];
    snprintf(request, sizeof(request),
            "GET %s HTTP/1.1\n"
            "Host: %s\n"
            "Connection: close\n"
            "\n",
            path, host_name);
    int write_ret = write(socket_fd, request, strnlen(request, sizeof(request)));
    if (connect_ret == -1) {
        exit(EXIT_FAILURE);
    }

    FILE* in = fdopen(socket_fd, "r");
    if (in == NULL) {
        exit(EXIT_FAILURE);
    }
    char buffer[4096];
    size_t context_size = 0;
    while (strcmp(buffer, "\r\n") != 0) {
        fgets(buffer, sizeof(buffer), in);
        sscanf(buffer, "Content-Length: %ld", &context_size);
    }

    char* context;
    context = calloc(context_size, sizeof(char));
    fread(context, sizeof(char), context_size, in);
    printf("%s", context);

    free(context);
    fclose(in);
}
