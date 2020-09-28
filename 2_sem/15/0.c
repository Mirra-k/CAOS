#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <sys/epoll.h>

extern size_t read_data_and_count(size_t N, int in[N]) {
    size_t total_number_read_bytes = 0;
    int epoll_fd = epoll_create1(0);
    for (int i = 0; i < N; ++i) {
        struct epoll_event event = {
            .events = EPOLLIN | EPOLLERR | EPOLLHUP, 
            .data = {.u32 = i}
        };
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, in[i], &event);
    }
    int not_closed = N;
    while (not_closed > 0) {
        struct epoll_event event;
        int epoll_ret = epoll_wait(epoll_fd, &event, 1, 1000);
        if (epoll_ret <= 0) {
            continue;
        }
        int i = event.data.u32;
        
        char buf[1000];
        size_t read_bytes = 0;
        read_bytes = read(in[i], buf, sizeof(buf));
        if (read_bytes == 0) {
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, in[i], NULL);
            close(in[i]);
            in[i] = -1;
            not_closed -= 1;
        }
        total_number_read_bytes += read_bytes;
    }
    close(epoll_fd);
    return total_number_read_bytes;
}
