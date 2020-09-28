#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct Item {
    int value;
    uint32_t next_pointer;
};

int main(int argc, char* argv[])
{
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        assert(close(fd) == 0);
        return 0;
    }

    struct stat s;
    assert(fstat(fd, &s) == 0);
    if (s.st_size == 0) {
        assert(close(fd) == 0);
        return 0;
    }

    void* mapped = mmap(NULL, s.st_size, PROT_READ, MAP_SHARED, fd, 0);
    assert(mapped != MAP_FAILED);
    char* buf = mapped;

    struct Item* item = (void*)(buf);
    printf("%d ", item->value);
    while (item->next_pointer) {
        struct Item* help = (void*)(buf + item->next_pointer);
        printf("%d ", help->value);
        item = help;
    }

    assert(munmap(mapped, s.st_size) == 0);
    assert(close(fd) == 0);
    return 0;
}
