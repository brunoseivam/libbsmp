#ifndef SLLP_VAR_H
#define SLLP_VAR_H

#include "sllp.h"
#include <stdint.h>
#include <stdbool.h>

#define SLLP_VAR_MIN_SIZE       1
#define SLLP_VAR_MAX_SIZE       127

struct sllp_var_info
{
    uint8_t id;                 // ID of the variable, used in the protocol.
    bool    writable;           // Determine if the variable is writable.
    uint8_t size;               // Indicates how many bytes 'data' contains.
};

struct sllp_var
{
    struct sllp_var_info info;  // Information about the variable identification
    bool                 (*value_ok) (struct sllp_var *);    // Value checker
    uint8_t              *data; // Pointer to the value of the variable.
    void                 *user; // The user can make use of this pointer at
                                // will. It is not touched by SLLP.
};

SLLP_LIST_STRUCT (sllp_var_info_list,
                  struct sllp_var_info,
                  SLLP_MAX_VARIABLES);

SLLP_LIST_STRUCT (sllp_var_info_ptr_list,
                  struct sllp_var_info *,
                  SLLP_MAX_VARIABLES);

SLLP_LIST_STRUCT (sllp_var_ptr_list,
                  struct sllp_var *,
                  SLLP_MAX_VARIABLES);

#endif
