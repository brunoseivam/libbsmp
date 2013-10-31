#include "func.h"
#include "server_defs.h"
#include "sllp_server.h"

enum sllp_err func_check(struct sllp_func *func)
{
    if(!func)
        return SLLP_ERR_PARAM_INVALID;

    if(!func->func_p)
        return SLLP_ERR_PARAM_INVALID;

    if(func->info.input_size > SLLP_FUNC_MAX_INPUT)
        return SLLP_ERR_PARAM_OUT_OF_RANGE;

    if(func->info.output_size > SLLP_FUNC_MAX_OUTPUT)
        return SLLP_ERR_PARAM_OUT_OF_RANGE;

    return SLLP_SUCCESS;
}

SERVER_CMD_FUNCTION (func_query_list)
{
    // Check payload size
    if(recv_msg->payload_size != 0)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    MESSSAGE_SET_ANSWER(send_msg, CMD_FUNC_LIST);

    struct sllp_func_info *func_info;
    unsigned int i;
    for(i = 0; i < server->funcs.count; ++i)
    {
        func_info = &server->funcs.list[i]->info;

        send_msg->payload[i] = (func_info->input_size << 4) |
                               (func_info->output_size & 0x0F);
    }

    send_msg->payload_size = server->funcs.count;
}

SERVER_CMD_FUNCTION (func_execute)
{
    if(!recv_msg->payload_size)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    uint8_t func_id = recv_msg->payload[0];

    if(func_id >= server->funcs.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    struct sllp_func *func = server->funcs.list[func_id];

    if(recv_msg->payload_size != 1 + func->info.input_size)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    uint8_t ret;

    ret = func->func_p(&recv_msg->payload[1], &send_msg->payload[0]);

    if(ret)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_FUNC_ERROR);
        send_msg->payload[0] = ret;
        send_msg->payload_size = 1;
    }
    else
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_FUNC_RETURN);
        send_msg->payload_size = func->info.output_size;
    }
}
