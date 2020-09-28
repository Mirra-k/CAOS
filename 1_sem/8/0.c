#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    char* file = argv[1];
    char* pattern = argv[2];

    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        return 0;
    }

    struct stat s;
    assert(fstat(fd, &s) == 0);
    if (s.st_size == 0) {
        close(fd);
        return 0;
    }

    char* mapped = mmap(NULL, s.st_size, PROT_READ, MAP_SHARED, fd, 0);
    assert(mapped != MAP_FAILED);

    char* buf = mapped;

    while (NULL != (buf = strstr(buf, pattern))) {
        size_t ans = buf - mapped;
        printf("%u ", ans);
        ++buf;
    }

    assert(munmap(mapped, s.st_size) == 0);
    assert(close(fd) == 0);
    return 0;
}
