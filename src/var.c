#include "var.h"
#include "server_defs.h"
#include "sllp_server.h"

#include <stdlib.h>
#include <string.h>

enum sllp_err var_check (struct sllp_var *var)
{
    if(!var)
        return SLLP_ERR_PARAM_INVALID;

    // Check variable fields
    if(var->info.size < VAR_MIN_SIZE || var->info.size > VAR_MAX_SIZE)
        return SLLP_ERR_PARAM_OUT_OF_RANGE;

    if(!var->data)
        return SLLP_ERR_PARAM_INVALID;

    return SLLP_SUCCESS;
}

SERVER_CMD_FUNCTION (var_query_list)
{
    // Check payload size
    if(recv_msg->payload_size != 0)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Set answer's command_code and payload_size
    message_set_answer(send_msg, CMD_VAR_LIST);

    // Variables are in order of their ID's
    struct sllp_var *var;

    // Add each variable to the response
    unsigned int i;
    for(i = 0; i < server->vars.count; ++i)
    {
        var = server->vars.list[i];
        send_msg->payload[i]  = var->info.writable ? WRITABLE : READ_ONLY;
        send_msg->payload[i] += var->info.size;
    }
    send_msg->payload_size = server->vars.count;
}

SERVER_CMD_FUNCTION (var_read)
{
    // Check payload size
    if(recv_msg->payload_size != 1)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t var_id = recv_msg->payload[0];

    if(var_id >= server->vars.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired variable
    struct sllp_var *var = server->vars.list[var_id];

    if(server->hook)
    {
        server->modified_list[0] = var;
        server->modified_list[1] = NULL;
        server->hook(SLLP_OP_READ, server->modified_list);
    }

    // Set answer
    message_set_answer(send_msg, CMD_VAR_VALUE);
    send_msg->payload_size = var->info.size;
    memcpy(send_msg->payload, var->data, var->info.size);
}

SERVER_CMD_FUNCTION (var_write)
{
    // Check if body has at least one byte bytes (ID)
    // Check payload size
    if(recv_msg->payload_size < 2)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t var_id = recv_msg->payload[0];

    if(var_id >= server->vars.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired var
    struct sllp_var *var = server->vars.list[var_id];

    // Check payload size
    if(recv_msg->payload_size != 1 + var->info.size)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check write permission
    if(!var->info.writable)
    {
        message_set_answer(send_msg, CMD_ERR_READ_ONLY);
        return;
    }

    // Everything is OK, perform operation
    memcpy(var->data, recv_msg->payload + 1, var->info.size);

    // Call hook
    if(server->hook)
    {
        server->modified_list[0] = var;
        server->modified_list[1] = NULL;
        server->hook(SLLP_OP_WRITE, server->modified_list);
    }

    // Set answer code
    message_set_answer(send_msg, CMD_OK);
}

SERVER_CMD_FUNCTION (var_bin_op)
{
    // Check if body has at least two bytes (ID + binary operation)
    if(recv_msg->payload_size < 2)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t var_id = recv_msg->payload[0];

    if(var_id >= server->vars.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired var
    struct sllp_var *var = server->vars.list[var_id];

    // Get operation
    unsigned char operation = recv_msg->payload[1];

    // Check operation
    if(!bin_op[operation])
    {
        message_set_answer(send_msg, CMD_ERR_OP_NOT_SUPPORTED);
        return;
    }

    // Check payload size
    if(recv_msg->payload_size != 2 + var->info.size)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check write permission
    if(!var->info.writable)
    {
        message_set_answer(send_msg, CMD_ERR_READ_ONLY);
        return;
    }

    // Everything is OK, perform operation
    bin_op[operation](var->data, recv_msg->payload + 2, var->info.size);

    // Call hook
    if(server->hook)
    {
        server->modified_list[0] = var;
        server->modified_list[1] = NULL;
        server->hook(SLLP_OP_WRITE, server->modified_list);
    }

    // Set answer code
    message_set_answer(send_msg, CMD_OK);
    return;
}

