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
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Set answer's command_code and payload_size
    MESSSAGE_SET_ANSWER(send_msg, CMD_VAR_LIST);

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
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t var_id = recv_msg->payload[0];

    if(var_id >= server->vars.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
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
    MESSSAGE_SET_ANSWER(send_msg, CMD_VAR_VALUE);
    send_msg->payload_size = var->info.size;
    memcpy(send_msg->payload, var->data, var->info.size);
}

SERVER_CMD_FUNCTION (var_write)
{
    // Check payload size
    if(recv_msg->payload_size < 2)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t var_id = recv_msg->payload[0];

    if(var_id >= server->vars.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired var
    struct sllp_var *var = server->vars.list[var_id];

    // Check payload size
    if(recv_msg->payload_size != 1 + var->info.size)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check write permission
    if(!var->info.writable)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_READ_ONLY);
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
    MESSSAGE_SET_ANSWER(send_msg, CMD_OK);
}

SERVER_CMD_FUNCTION (var_write_read)
{
    // Check payload size
    if(recv_msg->payload_size < 3)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t var_wr_id = recv_msg->payload[0];
    uint8_t var_rd_id = recv_msg->payload[1];

    if(var_wr_id >= server->vars.count || var_rd_id >= server->vars.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired vars
    struct sllp_var *var_wr = server->vars.list[var_wr_id];
    struct sllp_var *var_rd = server->vars.list[var_rd_id];

    // Check payload size
    if(recv_msg->payload_size != 2 + var_wr->info.size)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check write permission
    if(!var_wr->info.writable)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_READ_ONLY);
        return;
    }

    // Everything is OK, perform WRITE operation
    memcpy(var_wr->data, recv_msg->payload + 2, var_wr->info.size);

    // Call hooks
    if(server->hook)
    {
        // Write hook
        server->modified_list[0] = var_wr;
        server->modified_list[1] = NULL;
        server->hook(SLLP_OP_WRITE, server->modified_list);

        server->modified_list[0] = var_rd;
        server->hook(SLLP_OP_READ, server->modified_list);
    }

    // Now perform READ operation
    MESSSAGE_SET_ANSWER(send_msg, CMD_VAR_VALUE);
    send_msg->payload_size = var_rd->info.size;
    memcpy(send_msg->payload, var_rd->data, var_rd->info.size);
}

SERVER_CMD_FUNCTION (var_bin_op)
{
    // Check if body has at least two bytes (ID + binary operation)
    if(recv_msg->payload_size < 2)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t var_id = recv_msg->payload[0];

    if(var_id >= server->vars.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired var
    struct sllp_var *var = server->vars.list[var_id];

    // Get operation
    unsigned char operation = recv_msg->payload[1];

    // Check operation
    if(!bin_op[operation])
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_OP_NOT_SUPPORTED);
        return;
    }

    // Check payload size
    if(recv_msg->payload_size != 2 + var->info.size)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check write permission
    if(!var->info.writable)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_READ_ONLY);
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
    MESSSAGE_SET_ANSWER(send_msg, CMD_OK);
    return;
}

