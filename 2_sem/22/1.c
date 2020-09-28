#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char** argv) {
    char* password = argv[1];

    char heading[16];
    char salt[8];
    char key[32] = {0};
    char iv[16] = {0};

    read(STDIN_FILENO, heading, sizeof(heading));
    memcpy(salt, heading + 8, sizeof(salt));

    EVP_BytesToKey(
      EVP_aes_256_cbc(),
      EVP_sha256(),
      salt,
      password,
      strlen(password),
      1,
      key,
      iv
    );

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == 0) {
        exit(EXIT_FAILURE);
    }

    int decrypt_ret = EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    if (decrypt_ret == 0) {
        exit(EXIT_FAILURE);
    }

    int block_size = EVP_CIPHER_CTX_block_size(ctx);

    size_t read_symbol;
    int write_symbol;
    char in[block_size];
    char out[block_size];

    while ((read_symbol = read(STDIN_FILENO, in, sizeof(in))) > 0) {
        decrypt_ret = EVP_DecryptUpdate(ctx, out, &write_symbol, in, read_symbol);
        if (decrypt_ret == 0) {
            exit(EXIT_FAILURE);
        }
        printf("%.*s", write_symbol, out);
    }

    EVP_DecryptFinal_ex(ctx, out, &write_symbol);
    printf("%.*s", write_symbol, out);

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
