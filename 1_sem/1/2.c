#include <stdint.h>

uint16_t satsum(uint16_t x, uint16_t y)
{
    if (x < y)
    {
        uint16_t t = x;
        x = y;
        y = t;
    }

    uint16_t res = x + y;

    if (res < x)
    {
        return (x - res - 1 + y);
    }

    return res;
}
