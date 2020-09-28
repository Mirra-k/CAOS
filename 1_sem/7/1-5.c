#include <windows.h>
#include <stdio.h>
#include <stdint.h>

struct Item {
    int value;
    uint32_t next_pointer;
};

int main(int args, char* argv[])
{
    struct Item item;
    HANDLE fileHandle = CreateFileA(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle < 0) {
        DeleteFileA(argv[1]);
        return 1;
    }

    DWORD bytes_read;
    BOOL success;
    success = ReadFile(fileHandle, &item, sizeof(item), &bytes_read, NULL);
    if (!success) {
        DeleteFileA(argv[1]);
        return 1;
    }
    if (!bytes_read) {
        DeleteFileA(argv[1]);
        return 1;
    }

    printf("%d\n", item.value);
    while (item.next_pointer > 0) {
        SetFilePointer(fileHandle, item.next_pointer, NULL, FILE_BEGIN);
        ReadFile(fileHandle, &item, sizeof(item), NULL, NULL);
        printf("%d\n", item.value);
    }
    DeleteFileA(argv[1]);
    return 0;
}
