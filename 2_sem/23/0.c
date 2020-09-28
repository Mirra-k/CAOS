#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define FUSE_USE_VERSION 30
#include <fuse.h>

typedef struct {
    char name[1024];
    int size;
    int offset;
} file_t;

FILE* dir;
file_t* direct_files;
int direct_size;
char* dir_file_name;

void parse_tree() {
    dir = fopen(dir_file_name, "r");

    int number_of_files;
    fscanf(dir, "%d", &number_of_files);
    direct_files = calloc(number_of_files, sizeof(file_t));
    
    int offset = 0;
    int file_size;
    for (int i = 0; i < number_of_files; ++i) {
        fscanf(dir, "%s", direct_files[i].name);
        fscanf(dir, "%d", &file_size);
        direct_files[i].size = file_size;
        direct_files[i].offset = offset;
        offset += file_size;
    }

    char end[2];
    fread(end, 2, 1, dir);
    direct_size = number_of_files;
}

int getattr_callback(const char* path, struct stat* stbuf, struct fuse_file_info *fi) {
    if (strcmp(path, "/") == 0) {
        *stbuf = (struct stat) {.st_mode = S_IFDIR | 0444, .st_nlink = 2};
        return 0;
    }
    if (path[0] != '/') {
        return -ENOENT;
    }
    for (int i = 0; i < direct_size; ++i) {
        if (strcmp(direct_files[i].name, path + 1) == 0) {
            *stbuf = (struct stat) {.st_mode = S_IFREG | 0444, .st_nlink = 1, .st_size = direct_files[i].size};
            return 0;
        }
    }
}

int readdir_callback(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags) {

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    for (int i = 0; i < direct_size; ++i) {
        filler(buf, direct_files[i].name, NULL, 0, 0);
    }
    return 0;
}

int read_callback(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    if (path[0] == '/') {
        int cur = -1;
        for (int i = 0; i < direct_size; ++i) {
            if (strcmp(path + 1, direct_files[i].name) == 0) {
                int offset = direct_files[i].offset;
                int file_size = direct_files[i].size;
                fseek(dir, offset, SEEK_CUR);
                fread(buf, file_size, 1, dir);
                fseek(dir, -(offset + file_size), SEEK_CUR);
                return direct_files[i].size;
            }
        }
    }
    return -ENOENT;
}

struct fuse_operations fuse_operations = {
    .getattr = getattr_callback,
    .read = read_callback,
    .readdir = readdir_callback,
};

struct fuse_opt opt_specs[] = {
    { "--src %s", 0, 0 },
    { NULL, 0, 0}
};

int main(int argc, char** argv) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    fuse_opt_parse(&args, &dir_file_name, opt_specs, NULL);

    parse_tree();
    int ret = fuse_main(args.argc, args.argv, &fuse_operations, NULL);

    free(direct_files);
    fclose(dir);
    fuse_opt_free_args(&args);
    return ret;
}
