#ifndef VAR_H
#define VAR_H

#include "server_defs.h"
#include "sllp_var.h"

#define VAR_MIN_SIZE    SLLP_VAR_MIN_SIZE
#define VAR_MAX_SIZE    SLLP_VAR_MAX_SIZE

enum var_cmd
{
    CMD_VAR_QUERY_LIST      = 0x02,
    CMD_VAR_LIST            = 0x03,
    CMD_VAR_READ            = 0x10,
    CMD_VAR_VALUE           = 0x11,
    CMD_VAR_WRITE           = 0x20,
    CMD_VAR_BIN_OP          = 0x24,
    CMD_VAR_WRITE_READ      = 0x28,
};

enum sllp_err var_check (struct sllp_var *var);

SERVER_CMD_FUNCTION (var_query_list);
SERVER_CMD_FUNCTION (var_read);
SERVER_CMD_FUNCTION (var_write);
SERVER_CMD_FUNCTION (var_write_read);
SERVER_CMD_FUNCTION (var_bin_op);

#define VAR_CMD_POINTERS \
        [CMD_VAR_QUERY_LIST]    = var_query_list,\
        [CMD_VAR_READ]          = var_read,\
        [CMD_VAR_WRITE]         = var_write,\
        [CMD_VAR_BIN_OP]        = var_bin_op,\
        [CMD_VAR_WRITE_READ]    = var_write_read

#endif
