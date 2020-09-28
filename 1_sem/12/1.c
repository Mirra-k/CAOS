#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    char input[4096];
    char content[4096];
    scanf("%[^\n]", input);
    int fd = open("myProgram.c", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        assert(close(fd) == 0);
        return 0;
    } // если возникла ошибка при открытии/создании, уходим
    int sizec = snprintf(
        content,
        sizeof(content),
        "#include<math.h>\n #include<stdio.h>\n int main() {"
        "int res = (%s); printf(\"%%i\", res); return 0;}",
        input);
    write(fd, content, sizec);
    assert(close(fd) == 0);

    pid_t pid = fork();
    if (pid == 0) {
        execlp("gcc", "gcc", "-o", "myProgram", "myProgram.c", NULL);
        perror("exec");
        return 0;
    } else {
        int status;
        assert(waitpid(pid, &status, 0) != -1);
    }
    execlp("./myProgram", "./myProgram", NULL);
    perror("exec");

    return 0;
}
