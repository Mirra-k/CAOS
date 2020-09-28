#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/signalfd.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    FILE* files[argc - 1];
    for (int i = 1; i < argc; ++i) {
        files[i - 1] = fopen(argv[i], "r");
    }

    assert(SIGRTMIN < SIGRTMAX);
    sigset_t mask;
    sigfillset(&mask);
    for (int i = 0; i <= argc - 1; ++i) {
        sigaddset(&mask, SIGRTMIN + i);
    }
    sigprocmask(SIG_BLOCK, &mask, NULL);

    while (1) {
        int signal;
        sigwait(&mask, &signal);
        int x = signal - SIGRTMIN;
        if (x == 0) {
            for (int i = 1; i < argc; ++i) {
                fclose(files[i - 1]);
            }
            return 0;
        }
        char buffer[4096];
        fgets(buffer, 4096, files[x - 1]);
        write(1, buffer, strlen(buffer));
    }
}
