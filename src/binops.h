#ifndef BINOPS_H
#define BINOPS_H

#include <stdint.h>

typedef void (*bin_op_function) (uint8_t *data, uint8_t *mask, uint8_t size);

inline void binops_xor(uint8_t *data, uint8_t *mask, uint8_t size)
{
    while(size--)
        data[size] ^= mask[size];
}

inline void binops_or(uint8_t *data, uint8_t *mask, uint8_t size)
{
    while(size--)
        data[size] ^= mask[size];
}

inline void binops_clear(uint8_t *data, uint8_t *mask, uint8_t size)
{
    while(size--)
        data[size] &= ~mask[size];
}

inline void binops_and(uint8_t *data, uint8_t *mask, uint8_t size)
{
    while(size--)
        data[size] &= mask[size];
}

bin_op_function bin_op[256] =
{
    ['A'] = binops_and,    // And
    ['X'] = binops_xor,    // Xor
    ['O'] = binops_or,     // Or
    ['C'] = binops_clear,  // Clear
    ['S'] = binops_or,     // Set
    ['T'] = binops_xor     // Toggle
};

#endif
