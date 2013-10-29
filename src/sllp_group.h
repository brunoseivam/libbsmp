#ifndef SLLP_GROUP_H
#define SLLP_GROUP_H

#include "sllp.h"

#include <stdint.h>

struct sllp_group
{
    uint8_t id;           // ID of the group
    bool    writable;     // Whether all variables in the group are writable
    uint8_t size;         // Sum of the sizes of all variables in the group

    // List of pointers to the variables of this group
    struct sllp_var_info_ptr_list vars;
};

SLLP_LIST_STRUCT (sllp_group_list,
                  struct sllp_group,
                  SLLP_MAX_GROUPS);

#endif
