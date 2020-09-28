#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

#define assert(stmt) if (stmt) {} else {printf("failed"); exit(EXIT_FAILURE); }

typedef struct {
    int fd;
    int delta;
} thread_arg;

static void *thread_func(void *your_arg) {
    //printf("th1: start \n");
    thread_arg arg = *(thread_arg*)your_arg;
    int fd = arg.fd;
    int delta = arg.delta;
    int n;
    while (read(fd, &n, sizeof(int)) && n != 0 && n <= 100) {
        //printf("th1: %d \n", n);
        n += delta;
        printf("%d ", n);
        write(fd, &n, sizeof(int));
    }
    close(fd);
    //printf("th1: finish \n");
    return NULL;
}

int main(int argc, char* argv[]) {
    int N = strtol(argv[1], NULL, 10);

    //printf("create socket \n");
    int arr_fd[2];
    assert(socketpair(AF_UNIX, SOCK_STREAM, 0, arr_fd) == 0);
    //printf("write in socket \n");
    write(arr_fd[1], &N, sizeof(int));

    thread_arg arg1;
    arg1.fd = arr_fd[0];
    arg1.delta = -3;

    thread_arg arg2;
    arg2.fd = arr_fd[1];
    arg2.delta = 5;

    pthread_t thread[2];
    assert(pthread_create(&thread[0], NULL, thread_func, (void*)&arg1) == 0);
    assert(pthread_create(&thread[1], NULL, thread_func, (void*)&arg2) == 0);
    assert(pthread_join(thread[0], NULL) == 0);
    assert(pthread_join(thread[1], NULL) == 0);

    return 0;
}
