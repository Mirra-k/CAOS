#include <assert.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    for (int i = 1;; ++i) {
        int pid = fork();
        fflush(stdout);
        if (pid < 0) {
            printf("%d\n", i);
            return 0;
        }
        if (pid != 0) {
            int status;
            assert(waitpid(pid, &status, 0) != -1);
            break;
        }
    }
    return 0;
}
