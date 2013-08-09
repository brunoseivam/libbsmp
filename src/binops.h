#ifndef BINOPS_H
#define BINOPS_H

#include <stdint.h>

typedef void (*bin_op_function) (uint8_t *data, uint8_t *mask, uint8_t size);

inline void xor(uint8_t *data, uint8_t *mask, uint8_t size)
{
    while(size--)
        data[size] ^= mask[size];
}

inline void or(uint8_t *data, uint8_t *mask, uint8_t size)
{
    while(size--)
        data[size] ^= mask[size];
}

inline void clear(uint8_t *data, uint8_t *mask, uint8_t size)
{
    while(size--)
        data[size] &= ~mask[size];
}

inline void and(uint8_t *data, uint8_t *mask, uint8_t size)
{
    while(size--)
        data[size] &= mask[size];
}

bin_op_function bin_op[256] =
{
    ['A'] = and,    // And
    ['X'] = xor,    // Xor
    ['O'] = or,     // Or
    ['C'] = clear,  // Clear
    ['S'] = or,     // Set
    ['T'] = xor     // Toggle
};

#endif
