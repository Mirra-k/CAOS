#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define MAX 1000
#define pa_assert(stmt) if (stmt) {} else { printf("'" #stmt "' failed\n"); exit(EXIT_FAILURE); }

int main(int argc, char** argv)
{
    char name[MAX];
    char mem[MAX];
    int N;
    scanf("%s %s %d", &name, &mem, &N);

    sem_t* init_semaphore = sem_open(name, 0);
    pa_assert(init_semaphore != SEM_FAILED);
    sem_wait(init_semaphore);

    int fd = shm_open(mem, O_RDONLY, 0);
    pa_assert(fd >= 0);
    //pa_assert(ftruncate(fd, N*sizeof(int)) == 0);
    int* mapped = mmap(
        NULL, 
        N*sizeof(int), 
        PROT_READ, 
        MAP_SHARED,
        fd,
        0
    );
    pa_assert(mapped != MAP_FAILED);

    for (int i = 0; i < N; ++i) {
        printf("%d ", mapped[i]);
    }

    pa_assert(munmap(mapped, N*sizeof(int)) == 0);
    close(fd);
    sem_close(init_semaphore);

    return 0;
}
