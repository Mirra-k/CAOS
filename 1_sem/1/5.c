#include <stdio.h>

int checkAscii(int c) {
    return (c & (1 << 7)) == 0;
}
int findOktet(int c){
    if ((c >> 5) == 6)
        return 2;
    if ((c >> 4) == 14)
        return 3;
    if ((c >> 3) == 30)
        return 4;
    return 0;
}
int checkOtherOktet(int c) {
    return (c >> 6) == 2;
}


int main() {
    int ascii_compat = 0;
    int ascii_imcompat = 0;

    int current_char;
    while((current_char = getchar()) != EOF) {
        if (checkAscii(current_char)) {
            ascii_compat++;
        }
        else {
            int oktet = findOktet(current_char);
            if (oktet == 4) {
                int next;
                int next2;
                int next3;

                if ((next = getchar()) == EOF || (next2 = getchar()) == EOF ||(next3 = getchar()) == EOF
                    || !checkOtherOktet(next) || !checkOtherOktet(next2) || !checkOtherOktet(next3)) {
                    printf("%i %i", ascii_compat, ascii_imcompat);
                    return 1;
                }
                ++ascii_imcompat;
            }
            else if (oktet == 3){
                int next;
                int next2;

                if ((next = getchar()) == EOF || (next2 = getchar()) == EOF || !checkOtherOktet(next)
                    || !checkOtherOktet(next2)){
                    printf("%i %i", ascii_compat, ascii_imcompat);
                    return 1;
                }
                ++ascii_imcompat;
            }
            else if (oktet == 2){
                int next;

                if ((next = getchar()) == EOF || ! checkOtherOktet(next)){
                    printf("%i %i", ascii_compat, ascii_imcompat);
                    return 1;
                }
                ++ascii_imcompat;
            }
            else{
                printf("%i %i", ascii_compat, ascii_imcompat);
                return 1;
            }

        }
    }

    printf("%i %i", ascii_compat, ascii_imcompat);
    return 0;
}
