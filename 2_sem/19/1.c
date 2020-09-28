#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <semaphore.h>
#include <string.h>

#define pa_assert(stmt) if (stmt) {} else { printf("'" #stmt "' failed\n"); exit(EXIT_FAILURE); }

typedef struct {
    sem_t request_ready;  // начальное значение 0
    sem_t response_ready; // начальное значение 0
    char func_name[20];
    double value;
    double result;
} shared_data_t;

int main(int argc, char* argv[]) {
    char* lib = argv[1];
    void *lib_handle = dlopen(lib, RTLD_NOW);
    if (!lib_handle) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        abort();
    }

    char* shm_name = "/diht82707";
    int fd = shm_open(shm_name, O_RDWR | O_CREAT, 0644);
    pa_assert(fd >= 0);
    ftruncate(fd, sizeof(shared_data_t));
    shared_data_t* data = mmap(
        NULL, 
        sizeof(shared_data_t), 
        PROT_READ | PROT_WRITE, 
        MAP_SHARED,
        fd,
        0
    );
    pa_assert(data != MAP_FAILED);

    pa_assert(sem_init(&data->request_ready, 1, 0) == 0);
    pa_assert(sem_init(&data->response_ready, 1, 0) == 0);

    printf("%s\n", shm_name);
    fflush(stdout);

    sem_wait(&data->request_ready);
    while(strlen(data->func_name) > 0) {
        double (*func)(double) = dlsym(lib_handle, data->func_name);
        data->result = func(data->value);
        sem_post(&data->response_ready);
        sem_wait(&data->request_ready);
    }

    close(fd);
    shm_unlink(shm_name);
    sem_destroy(&data->request_ready);
    sem_destroy(&data->request_ready);
    munmap(data, sizeof(shared_data_t));
    dlclose(lib_handle);

    return 0;
}
