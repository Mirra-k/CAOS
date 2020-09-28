#include <stdint.h>
#include <stdlib.h>
#include <cstring>
#include <dirent.h>
#include <iostream>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

#define FUSE_USE_VERSION 30
#include <fuse.h>

char* folders_merg;
std::vector<std::string> folders;

std::string get_new_path(const char* char_path) {
    std::string path(char_path);
    std::string new_folder;
    time_t new_time = 0;
    struct stat stats;

    for (std::string& str : folders) {
        std::string file_path = str + path;

        stat(file_path.c_str(), &stats);
        if (new_time < stats.st_mtime) {
            new_time = stats.st_mtime;
            new_folder = std::move(file_path);
        }
    }
    return new_folder;
}

int getattr_callback(const char* char_path, struct stat* stbuf, struct fuse_file_info *fi) {
    std::string path = get_new_path(char_path);

    if (path != "") {
        stat(path.c_str(), stbuf);

        if (S_ISDIR(stbuf->st_mode)) {
            stbuf->st_mode = S_IFDIR | 0555;
        } else {
            stbuf->st_mode = S_IFREG | 0444;
        }

        return 0;
    }
    return -ENOENT;
}

int readdir_callback(const char* char_path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
    std::string path(char_path);
    std::set<std::string> tree;

    filler(buf, ".", NULL, 0, fuse_fill_dir_flags());
    filler(buf, "..", NULL, 0, fuse_fill_dir_flags());

    for (std::string& folder : folders) {
        std::string folder_path = folder + path;

        DIR* dir = opendir(folder_path.c_str());
        if (dir != NULL) {
            dirent* it = readdir(dir);
            while (it != NULL) {
                tree.emplace(it->d_name);
                it = readdir(dir);
            }
        }
        closedir(dir);
    }

    for (const std::string& str : tree) {
        filler(buf, str.c_str(), NULL, 0, fuse_fill_dir_flags());
    }
    return 0;
}

int read_callback(const char* char_path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    std::string path = get_new_path(char_path);

    struct stat stats;
    stat(path.c_str(), &stats);
    if (offset >= stats.st_size) {
        return 0;
    }

    int file = open(path.c_str(), O_RDONLY);
    lseek(file, offset, SEEK_SET);

    int all_read = 0;
    int is_read = 0;
    int have_read = size;
    while ((is_read = read(file, buf + all_read, have_read)) > 0) {
        have_read -= is_read;
        all_read += is_read;
    }
    close(file);
    return all_read;
}

struct fuse_operations fuse_operations;

void parse_fold(char* folders_merg) {
    char* pch = strtok (folders_merg, ":");
    while (pch != NULL) {
        char fold_path[2048];
        realpath(pch, fold_path);
        folders.emplace_back(fold_path);
        pch = strtok (NULL, ":");
    }
}

int main(int argc, char** argv) {
    fuse_operations.getattr = getattr_callback;
    fuse_operations.readdir = readdir_callback;
    fuse_operations.read = read_callback;

    struct fuse_opt opt_specs[] = {
        { "--src %s", 0, 0 },
        { NULL, 0, 0}
    };

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    fuse_opt_parse(&args, &folders_merg, opt_specs, NULL);

    parse_fold(folders_merg);

    int res = fuse_main(args.argc, args.argv, &fuse_operations, NULL);
    fuse_opt_free_args(&args);
    return res;
}
