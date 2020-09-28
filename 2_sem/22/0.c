#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    SHA512_CTX ctx;
    int ret = SHA512_Init(&ctx);
    if (ret == 0) {
        exit(EXIT_FAILURE);
    }

    char buffer[64];
    size_t read_symbol;
    while ((read_symbol = read(0, buffer, sizeof(buffer))) > 0) {
        ret = SHA512_Update(&ctx, buffer, read_symbol);
        if (ret == 0) {
            exit(EXIT_FAILURE);
        }
    }

    char hash[64];
    ret = SHA512_Final(hash, &ctx);
    if (ret == 0) {
        exit(EXIT_FAILURE);
    }

    printf("0x");
    for (int i = 0; i < sizeof(hash); ++i) {
        printf("%02x", hash[i] & 0xff);
    }

    return 0;
}
