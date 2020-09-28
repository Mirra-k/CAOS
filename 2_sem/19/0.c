#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <dlfcn.h>

int main(int argc, char* argv[]) {
    char* lib = argv[1];
    char* function = argv[2];

    void *lib_handle = dlopen(lib, RTLD_NOW);
    if (!lib_handle) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        abort();
    }

    double (*func)(double) = dlsym(lib_handle, function);

    double arg;
    while (scanf("%lf", &arg) != EOF) {
        printf("%.3f ", func(arg));
    }

    dlclose(lib_handle);
    return 0;
}
