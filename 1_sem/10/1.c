#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t variable = 0;
volatile sig_atomic_t sig_exit = 1;

static void handler_usr1(int signum)
{
    ++variable;
    printf("%d ", variable);
    fflush(stdout);
}
static void handler_usr2(int signum)
{
    variable *= -1;
    printf("%d ", variable);
    fflush(stdout);
}
static void handler_term(int signum)
{
    sig_exit = 0;
}

int main(int argc, char* argv[])
{
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sigaction(
        SIGUSR1,
        &(struct sigaction){.sa_handler = handler_usr1, .sa_flags = SA_RESTART},
        NULL);
    sigaction(
        SIGUSR2,
        &(struct sigaction){.sa_handler = handler_usr2, .sa_flags = SA_RESTART},
        NULL);
    sigaction(
        SIGTERM,
        &(struct sigaction){.sa_handler = handler_term, .sa_flags = SA_RESTART},
        NULL);
    sigemptyset(&mask);
    printf("%d\n", getpid());
    fflush(stdout);
    scanf("%d", &variable);

    while (sig_exit) {
        sigsuspend(&mask);
    }
    return 0;
}
