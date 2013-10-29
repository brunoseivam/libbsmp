#ifndef DEFS_H
#define DEFS_H

#include <stdbool.h>
#include <stdint.h>

#include "sllp.h"

#define HEADER_SIZE             SLLP_HEADER_SIZE

#define MAX_PAYLOAD             SLLP_MAX_PAYLOAD
#define MAX_MESSAGE             SLLP_MAX_MESSAGE
#define MAX_PAYLOAD_ENCODED     255

#define WRITABLE_MASK           0x80
#define SIZE_MASK               0x7F

#define WRITABLE                0x80
#define READ_ONLY               0x00

#define MAX_VARIABLES           SLLP_MAX_VARIABLES
#define MAX_GROUPS              SLLP_MAX_GROUPS
#define MAX_CURVES              SLLP_MAX_CURVES
#define MAX_FUNCTIONS           SLLP_MAX_FUNCTIONS

enum command_code
{
    CMD_OK = 0xE0,
    CMD_ERR_MALFORMED_MESSAGE,
    CMD_ERR_OP_NOT_SUPPORTED,
    CMD_ERR_INVALID_ID,
    CMD_ERR_INVALID_VALUE,
    CMD_ERR_INVALID_PAYLOAD_SIZE,
    CMD_ERR_READ_ONLY,
    CMD_ERR_INSUFFICIENT_MEMORY,

    CMD_MAX
};

#endif  /* COMMON_H */

