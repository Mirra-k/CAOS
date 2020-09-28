#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <assert.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <ctype.h>

#define MAXSIZE 1000

volatile sig_atomic_t work = 1;

int server_main(char* port, int stop_fd) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (socket_fd == -1) {
        perror("can't initialize socket"); 
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(strtol(port, NULL, 10));

    int bind_ret = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (bind_ret == -1) {
        perror("can't bind to unix socket"); 
        exit(EXIT_FAILURE);
    }

    int listen_ret = listen(socket_fd, SOMAXCONN);
    if (listen_ret == -1) {
        perror("can't listen to unix socket"); 
        exit(EXIT_FAILURE);
    }

    int epoll_fd = epoll_create1(0);
    int fds[] = {stop_fd, socket_fd, -1};
    for (int* fd = fds; *fd != -1; ++fd) {
        struct epoll_event event = {
                .events = EPOLLIN | EPOLLHUP,
                .data = {.fd = *fd}
        };
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, *fd, &event);
    }

    struct epoll_event events[MAXSIZE];
    while (work) {
        int connect_count = epoll_wait(epoll_fd, events, MAXSIZE, MAXSIZE);
        if (connect_count <= 0) {
            continue;
        }
        for (int i = 0; i < connect_count; ++i) {
            if (events[i].data.fd == stop_fd) {
                break;
            }
            if (events[i].data.fd == socket_fd) {
                int connection_fd = accept(socket_fd, NULL, NULL);
                if (connection_fd == -1) {
                    perror("can't accept incoming connection"); 
                    exit(EXIT_FAILURE);
                }
                fcntl(connection_fd, F_SETFL, fcntl(connection_fd, F_GETFL, 0) | O_NONBLOCK);
                struct epoll_event action = {
                    .events = EPOLLIN | EPOLLET,
                    .data.fd = connection_fd
                };
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_fd, &action);
            } else {
                int connection_fd = events[i].data.fd;
                size_t read_bytes = 0;
                char buf[4096];
                while ((read_bytes = read(connection_fd, buf, sizeof(buf))) > 0) {
                    for (int j = 0; j < read_bytes; ++j) {
                        buf[j] = toupper(buf[j]);
                    }
                    write(connection_fd, buf, read_bytes);
                }

                if (read_bytes == 0 || !work) {
                    shutdown(connection_fd, SHUT_RDWR);
                    close(connection_fd);
                }
            }
        }
    }
    close(epoll_fd);
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
    return 0;
}

int main(int argc, char* argv[]) {
    sigset_t full_mask;
    sigfillset(&full_mask);
    sigprocmask(SIG_BLOCK, &full_mask, NULL);
 
    int fds[2];
    assert(pipe(fds) == 0);
 
    int child_pid = fork();
    assert(child_pid >= 0);
    if (child_pid == 0) {
        close(fds[1]);
        server_main(argv[1], fds[0]);
        close(fds[0]);
        return 0;
    } else {
        close(fds[0]);
        while (1) {
            siginfo_t info;
            sigwaitinfo(&full_mask, &info);
            int received_signal = info.si_signo;
            if (received_signal == SIGTERM) {
                int written = write(fds[1], "X", 1);
                if (written != 1) {
                    perror("writing failed");
                    exit(EXIT_FAILURE);
                }
                close(fds[1]);
                break;
            }
        }
        int status;
        assert(waitpid(child_pid, &status, 0) != -1);
    }
    return 0;
}
