#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        close(fd);
        return 1;
    }

    int only_digit = open(argv[2], O_WRONLY | O_CREAT, 0640);
    int other = open(argv[3], O_WRONLY | O_CREAT, 0640);
    if (only_digit == -1 || other == -1) {
        close(fd);
        close(only_digit);
        close(other);
        return 2;
    }

    ssize_t readin;
    char r_byte;
    int out;

    readin = read(fd, &r_byte, 1);
    while (readin > 0) {
        out = r_byte >= '0' && r_byte <= '9' ? only_digit : other;

        if (write(out, &r_byte, 1) == -1) {
            close(fd);
            close(only_digit);
            close(other);
            return 3;
        }
        readin = read(fd, &r_byte, 1);
    }

    close(fd);
    close(only_digit);
    close(other);
    return 0;
}
