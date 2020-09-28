#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int n = strtol(argv[1], NULL, 10);
    int count = n - 1;
    pid_t pid;
    pid = fork();
    while (pid == 0 && count) {
        --count;
        pid = fork();
    }
    if (pid != 0) {
        int status;
        assert(waitpid(pid, &status, 0) != -1);
        count == n - 1 ? printf("%d\n", count + 1) : printf("%d ", count + 1);
    }

    return 0;
}
