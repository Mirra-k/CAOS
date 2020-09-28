#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        perror("can't initialize socket"); 
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(atoi(argv[1])),
        .sin_addr = {
            .s_addr = htonl(INADDR_LOOPBACK)
        }
    };

    int connect_ret = connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (connect_ret == -1) {
        perror("can't connect");
        exit(EXIT_FAILURE);
    }

    unsigned to, from;
    while (scanf("%u", &to) > 0) {
        int written_bytes = send(socket_fd, &to, sizeof(to), 0);
        if (written_bytes > 0) {
            int bytes_read = recv(socket_fd, &from, sizeof(from), MSG_WAITALL);
            if (bytes_read > 0) {
                printf("%u ", from);
            } else {
                break;
            }
        } else {
            break;
        }
    }

    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
    return 0;
}
