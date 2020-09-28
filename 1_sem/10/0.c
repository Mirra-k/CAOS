#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

sig_atomic_t sigint_count = 0;
sig_atomic_t sig_exit = 0;

static void handler_int(int signum)
{
    ++sigint_count;
    fflush(stdout);
}
static void handler_term(int signum)
{
    printf("%d\n", sigint_count);
    ++sig_exit;
    exit(0);
}

int main(int argc, char* argv[])
{
    sigaction(
        SIGTERM,
        &(struct sigaction){.sa_handler = handler_term, .sa_flags = SA_RESTART},
        NULL);
    sigaction(
        SIGINT,
        &(struct sigaction){.sa_handler = handler_int, .sa_flags = SA_RESTART},
        NULL);
    printf("%d\n", getpid());
    fflush(stdout);
    while (!sig_exit) {
        pause();
    }
    return 0;
}
