#ifndef SERVER_DEFS_H
#define SERVER_DEFS_H

#include <stdint.h>
#include "sllp.h"
#include "sllp_server.h"
#include "defs.h"

struct message
{
    uint8_t  command_code;
    uint16_t payload_size;
    uint8_t  *payload;
};

struct generic_list
{
    uint32_t count;
    void *list[];
};

#define SERVER_CMD_FUNCTION(name) \
    void name (sllp_server_t *server, struct message *recv_msg, \
               struct message *send_msg)

typedef SERVER_CMD_FUNCTION((*command_function_t));

struct sllp_server
{
    struct sllp_var_ptr_list    vars;
    struct sllp_group_list      groups;
    struct sllp_curve_ptr_list  curves;
    struct sllp_func_ptr_list   funcs;

    struct sllp_var *modified_list[MAX_VARIABLES+1];
    sllp_hook_t hook;
};

inline void message_set_answer (struct message *msg, uint8_t code)
{
    msg->command_code = code;
    msg->payload_size = 0;
}

#endif
