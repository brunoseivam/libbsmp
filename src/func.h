#ifndef FUNC_H
#define FUNC_H

#include "server_defs.h"
#include "sllp_func.h"

enum func_cmd
{
    CMD_FUNC_QUERY_LIST = 0x0C,
    CMD_FUNC_LIST       = 0x0D,
    CMD_FUNC_EXECUTE    = 0x50,
    CMD_FUNC_RETURN     = 0x51,
    CMD_FUNC_ERROR      = 0x53
};

enum sllp_err func_check (struct sllp_func *func);

SERVER_CMD_FUNCTION (func_query_list);
SERVER_CMD_FUNCTION (func_execute);

#define FUNC_CMD_POINTERS \
        [CMD_FUNC_QUERY_LIST]   = func_query_list,\
        [CMD_FUNC_EXECUTE]      = func_execute

#endif
