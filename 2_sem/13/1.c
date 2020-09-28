#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    char* cmd1 = argv[1];
    char* cmd2 = argv[2];
    int fd[2];
    pipe(fd);
    pid_t pid_1, pid_2;
    if ((pid_1 = fork()) == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        execlp(cmd1, cmd1, NULL);
        assert(0 && "Unreachable position in code if execlp succeeded");
    }
    if ((pid_2 = fork()) == 0) {
        dup2(fd[0], 0);
        close(fd[0]);
        close(fd[1]);
        execlp(cmd2, cmd2, NULL);
        assert(0 && "Unreachable position in code if execlp succeeded");
    }
    close(fd[0]);
    close(fd[1]);
    int status;
    assert(waitpid(pid_1, &status, 0) != -1);
    assert(waitpid(pid_2, &status, 0) != -1);
    return 0;
}
