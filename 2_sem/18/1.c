#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <semaphore.h>
#include <wait.h>

#define MAX 1000
#define pa_assert(stmt) if (stmt) {} else { printf("'" #stmt "' failed\n"); exit(EXIT_FAILURE); }

typedef double (*function_t)(double);

double* pmap_process(function_t func, const double *in, size_t count) {
    int N = get_nprocs();
    pid_t processes[N];
    
    sem_t* sem = mmap(
        NULL, 
        sizeof(sem_t),
        PROT_READ | PROT_WRITE, 
        MAP_SHARED | MAP_ANONYMOUS, 
        -1, 
        0
    );
    sem_init(sem, 1, 0);
    pa_assert(sem != MAP_FAILED);

    double *mapped = mmap(
        NULL, 
        sizeof(double) * count,
        PROT_READ | PROT_WRITE, 
        MAP_SHARED | MAP_ANONYMOUS, 
        -1, 
        0
    );
    pa_assert(mapped != MAP_FAILED);

    for (int i = 0; i < N; ++i) {
        processes[i] = fork();
        if (processes[i] == 0) {
            for (int j = 0; j * N + i < count; ++j) {
                mapped[j*N+i] = func(in[j*N+i]);
            }
            sem_post(sem);
            exit(0);
        }
    }
    for(int i = 0; i < N; ++i) {
        sem_wait(sem);
    }

    pa_assert(sem_destroy(sem) == 0);
    pa_assert(munmap(sem, sizeof(sem_t)) == 0);
    for (int i = 0; i < N; ++i) {
        int status;
        pa_assert(waitpid(processes[i], &status, 0) != -1)
        pa_assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    }

    return mapped;
}

void pmap_free(double *ptr, size_t count) {
    munmap(ptr, sizeof(double) * count);
}
