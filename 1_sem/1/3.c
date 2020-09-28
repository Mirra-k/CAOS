#include <stdio.h>
#include <string.h>

int min_bytes_for_hex (char* numbers)
{
    int zeros_count = 2;
    while (numbers[zeros_count] == '0')
    {
        ++zeros_count;
    }
    int not_zero = strlen(numbers) - zeros_count;
    if (!not_zero)
    {
        not_zero = 1;
    }

    return (not_zero / 2 + not_zero % 2);
}

int main (int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        printf("%d ", min_bytes_for_hex(argv[i]));
    }
}
