#ifndef SLLP_CURVE_H
#define SLLP_CURVE_H

#include "sllp.h"
#include <stdint.h>

#define SLLP_CURVE_MIN_BLOCKS   1
#define SLLP_CURVE_MAX_BLOCKS   65536

#define SLLP_CURVE_BLOCK_SIZE   16384
#define SLLP_CURVE_BLOCK_INFO   3
#define SLLP_CURVE_BLOCK_PKT    (SLLP_CURVE_BLOCK_INFO + SLLP_CURVE_BLOCK_SIZE)

struct sllp_curve_info
{
    uint8_t  id;                    // ID of the curve, used in the protocol.
    bool     writable;              // Determine if the curve is writable.
    uint32_t nblocks;               // How many 16kB blocks the curve contains.
    uint8_t  checksum[16];          // MD5 checksum of the curve
};

struct sllp_curve
{
    struct sllp_curve_info info;   // Information about the curve identification

    // Read a SLLP_CURVE_BLOCK_SIZE bytes block into data
    void (*read_block) (struct sllp_curve *curve, uint16_t block,uint8_t *data);

    // Write a SLLP_CURVE_BLOCK_SIZE bytes block from data
    void (*write_block)(struct sllp_curve *curve, uint16_t block,uint8_t *data);

    void    *user;                 // The user can make use of this variable as
                                   // he wishes. It is not touched by SLLP.
};

SLLP_LIST_STRUCT (sllp_curve_info_list,
                  struct sllp_curve_info,
                  SLLP_MAX_CURVES);

SLLP_LIST_STRUCT (sllp_curve_ptr_list,
                  struct sllp_curve *,
                  SLLP_MAX_CURVES);

#endif
