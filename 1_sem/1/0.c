#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    double x;
    int y;
    scanf("%lf %x", &x, &y);
    long z = strtol(argv[1], NULL, 27);
    printf("%.3f", x + y + z);
    return 0;
};
