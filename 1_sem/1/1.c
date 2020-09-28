#include <stdint.h>

typedef enum
{
    PlusZero = 0x00,
    MinusZero = 0x01,
    PlusInf = 0xF0,
    MinusInf = 0xF1,
    PlusRegular = 0x10,
    MinusRegular = 0x11,
    PlusDenormal = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN = 0x30,
    QuietNaN = 0x31
} float_class_t;

extern float_class_t classify(double* value_ptr)
{
    uint64_t bytes = *((uint64_t*)value_ptr);
    int64_t mantisa = (bytes << 12) >> 12;
    int64_t exponent = (bytes << 1) >> 53;
    int64_t sign = bytes >> 63;

    float_class_t result;

    if (exponent == 0)
    {
        if (mantisa == 0)
        {
            result = sign == 0 ? PlusZero : MinusZero;
        }
        else
        {
            result = sign == 0 ? PlusDenormal : MinusDenormal;
        }
    }
    else if (exponent == 0x7FF)
    {
        if (mantisa == 0)
        {
            result = sign == 0 ? PlusInf : MinusInf;
        }
        else
        {
            return (mantisa >> 51) ? QuietNaN : SignalingNaN;
        }
    }
    else
    {
        result = sign == 0 ? PlusRegular : MinusRegular;
    }
    return result;
}
