#ifndef GROUP_H
#define GROUP_H

#include "server_defs.h"
#include "sllp_var.h"
#include "sllp_group.h"

#include <stdint.h>

enum group_cmd
{
    CMD_GROUP_QUERY_LIST    = 0x04,
    CMD_GROUP_LIST          = 0x05,
    CMD_GROUP_QUERY         = 0x06,
    CMD_GROUP               = 0x07,
    CMD_GROUP_READ          = 0x12,
    CMD_GROUP_VALUES        = 0x13,
    CMD_GROUP_WRITE         = 0x22,
    CMD_GROUP_BIN_OP        = 0x26,
    CMD_GROUP_CREATE        = 0x30,
    CMD_GROUP_REMOVE_ALL    = 0x32,
};

enum group_id
{
    GROUP_ALL_ID,
    GROUP_READ_ID,
    GROUP_WRITE_ID,

    GROUP_STANDARD_COUNT,
};

void group_init (struct sllp_group *grp, uint8_t id);
void group_add_var (struct sllp_group *grp, struct sllp_var *var);

SERVER_CMD_FUNCTION (group_query_list);
SERVER_CMD_FUNCTION (group_query);
SERVER_CMD_FUNCTION (group_read);
SERVER_CMD_FUNCTION (group_write);
SERVER_CMD_FUNCTION (group_bin_op);
SERVER_CMD_FUNCTION (group_create);
SERVER_CMD_FUNCTION (group_remove_all);

#define GROUP_CMD_POINTERS \
        [CMD_GROUP_QUERY_LIST]  = group_query_list,\
        [CMD_GROUP_QUERY]       = group_query,\
        [CMD_GROUP_READ]        = group_read,\
        [CMD_GROUP_WRITE]       = group_write,\
        [CMD_GROUP_BIN_OP]      = group_bin_op,\
        [CMD_GROUP_CREATE]      = group_create,\
        [CMD_GROUP_REMOVE_ALL]  = group_remove_all

#endif
