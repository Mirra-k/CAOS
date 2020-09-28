#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    char* cmd = argv[1];
    char* in = argv[2];
    int fd[2];
    int input = open(in, O_RDONLY);
    pipe(fd);
    pid_t pid_1;
    if ((pid_1 = fork()) == 0) {
        dup2(input, 0);
        close(input);
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        execlp(cmd, cmd, NULL);
        assert(0 && "Unreachable position in code if execlp succeeded");
    }

    int status;
    assert(waitpid(pid_1, &status, 0) != -1);
    dup2(fd[0], 0);
    close(fd[0]);
    close(fd[1]);
    int count = 0, size_read = 0;
    char buffer[4096];
    while((size_read = read(0, buffer, sizeof(buffer))) > 0) {
        count += size_read;
    }
    printf("%d", count);

    return 0;
}
