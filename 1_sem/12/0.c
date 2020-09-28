#include <stdio.h>
#include <unistd.h>

int main()
{
    char input[4096];
    char content[4096];
    scanf("%[^\n]", input);
    snprintf(content, 4096, "print(%s)", input);
    execlp("python3", "python3", "-c", content, NULL);
    perror("exec");
    return 0;
}
