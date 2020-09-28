#include <signal.h>
#include <string.h>

int main(int argc, char* argv[])
{
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    while (1) {
        siginfo_t info;
        sigwaitinfo(&mask, &info);
        int received_signal = info.si_signo;
        if (received_signal == SIGRTMIN) {
            if (info.si_value.sival_int == 0) {
                return 0;
            }
            union sigval val;
            val.sival_int = info.si_value.sival_int - 1;
            sigqueue(info.si_pid, SIGRTMIN, val);
        }
    }
}
