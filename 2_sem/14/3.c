#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <wait.h>
#include <sys/epoll.h>
#include <assert.h>


int server_main(int argc, char** argv, int stop_fd) {
    assert(argc >= 2);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (socket_fd == -1) {
        perror("can't initialize socket"); 
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(strtol(argv[1], NULL, 10));

    int bind_ret = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (bind_ret == -1) {
        perror("can't bind to unix socket"); 
        exit(EXIT_FAILURE);
    }

    int listen_ret = listen(socket_fd, 128);
    if (listen_ret == -1) {
        perror("can't listen to unix socket"); 
        exit(EXIT_FAILURE);
    }

    int dir_fd = open(argv[2], O_RDONLY);
    if (dir_fd < 0) {
        perror("can't open dir");
        exit(EXIT_FAILURE);
    }


    int epoll_fd = epoll_create1(0);
    struct epoll_event event = {
        .events = EPOLLIN | EPOLLERR | EPOLLHUP, 
        .data = {.fd = stop_fd}
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, stop_fd, &event);
    event.data.fd = socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);


    while (true) {
        struct epoll_event event;
        int epoll_ret = epoll_wait(epoll_fd, &event, 1, 1000);
        if (epoll_ret <= 0) {
            continue;
        }
        if (event.data.fd == stop_fd) {
            break;
        }

        int connection_fd = accept(socket_fd, NULL, NULL);
        if (connection_fd == -1) {
            perror("can't accept incoming connection"); 
            exit(EXIT_FAILURE);
        }
        
        char name_of_file[0x1000];
        int fd_clone = dup(connection_fd);
        FILE *stream = fdopen(fd_clone, "r");
        fscanf(stream, "GET %s HTTP/1.1", name_of_file);
        fclose(stream);

        if (faccessat(dir_fd, name_of_file, 0, 0) != 0) {
            write(connection_fd, "HTTP/1.1 404 Not Found\r\n\r\n", strlen("HTTP/1.1 404 Not Found\r\n\r\n"));
            close(connection_fd);
            continue;
        }

        if (faccessat(dir_fd, name_of_file, R_OK, 0) != 0) {
            write(connection_fd, "HTTP/1.1 403 Forbidden\r\n\r\n", strlen("HTTP/1.1 403 Forbidden\r\n\r\n"));
            close(connection_fd);
            continue;
        }

        write(connection_fd, "HTTP/1.1 200 OK\r\n", strlen("HTTP/1.1 200 OK\r\n"));

        int file = openat(dir_fd, name_of_file, O_RDONLY);
        if (faccessat(dir_fd, name_of_file, X_OK, 0) == 0) {
            char full_name[strlen(argv[2]) + strlen(name_of_file) + 2];
            sprintf(full_name, "%s/%s", argv[2], name_of_file);
            int real_stdout = dup(1);
            dup2(connection_fd, 1);
            system(full_name);
            dup2(real_stdout, 1);
        } else {
            struct stat st;
            fstat(file, &st);

            char size_print[0x1000 + 1];
            snprintf(size_print, 0x1000, "Content-Length: %ld\r\n\r\n", st.st_size);
            write(connection_fd, size_print, strlen(size_print));

            char buff[0x4096];
            int now_read = 0;
            while((now_read = read(file, buff, 0x4096)) > 0) {
                write(connection_fd, buff, now_read);
            }
        }
        shutdown(connection_fd, SHUT_RDWR);
        close(connection_fd);
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
        server_main(argc, argv, fds[0]);
        close(fds[0]);
        return 0;
    } else {
        close(fds[0]);
        while (1) {
            siginfo_t info;
            sigwaitinfo(&full_mask, &info);
            int received_signal = info.si_signo;
            if (received_signal == SIGTERM || received_signal == SIGINT) {
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
