include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc == 1) {
        return 0;
    }

    int fd1[2];
    int fd2[2];

    int *in = fd1;
    int *out = fd2;
    int stdout = dup(1);

    pipe(in);
    dup2(in[1], 1);
    close(in[1]);
    
    system(argv[1]);

    for (int i = 2; i < argc - 1; ++i) {
        pipe(out);

        dup2(out[1], 1);
        close(out[1]);
        dup2(in[0], 0);
        close(in[0]);
        
        system(argv[i]);

        int *c = out;
        out = in;
        in = c;
    }

    dup2(in[0], 0);
    close(in[0]);
    
    dup2(stdout, 1);
    system(argv[argc - 1]);

    return 0;
}
