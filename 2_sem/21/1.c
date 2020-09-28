#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    char* host_name = argv[1];
    char* path = argv[2];
    char* file = argv[3];

    struct addrinfo hints = { 
        .ai_family = AF_INET, 
        .ai_socktype = SOCK_STREAM 
    };
    struct addrinfo *result = NULL;

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

    FILE* my_file = fopen(file, "r");

    fseek(my_file, 0, SEEK_END);
    long file_size = ftell(my_file);
    fseek(my_file, 0, SEEK_SET);

    char *string = malloc(file_size + 1);
    fread(string, sizeof(char), file_size, my_file);
    string[file_size] = '\0';

    int buf_size = file_size + 54321;
    char* request = (char*) malloc((buf_size)*sizeof(char));
    snprintf(request, buf_size,
            "POST %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: multipart/form-data\r\n"
            "Connection: close\r\n"
            "Content-Length: %d\r\n\r\n"
            "%s\r\n"
            "\r\n",
            path, host_name, file_size, string);

    write(socket_fd, request, strnlen(request, buf_size));

    FILE* in = fdopen(socket_fd, "r");
    if (in == NULL) {
        exit(EXIT_FAILURE);
    }

    char buffer[4096];
    int complete = 0;
    while (fgets(buffer, sizeof(buffer), in)) {
        if (strcmp(buffer, "\n") == 0 || strcmp(buffer, "\r\n") == 0) {
            complete = 1;
        }
        else if (complete) {
            printf("%s", buffer);
        }
    };

    fclose(in);
    fclose(my_file);
    free(request);
    free(string);
}
