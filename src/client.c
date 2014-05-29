#include "bsmp_priv.h"
#include "../include/client.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static char bin_op_code[BIN_OP_COUNT] =
{
    [BIN_OP_AND]    = 'A',
    [BIN_OP_OR]     = 'O',
    [BIN_OP_XOR]    = 'X',
    [BIN_OP_SET]    = 'S',
    [BIN_OP_CLEAR]  = 'C',
    [BIN_OP_TOGGLE] = 'T',
};

struct bsmp_message
{
    uint8_t     code;
    uint16_t    payload_size;
    uint8_t     payload[BSMP_MAX_PAYLOAD];
};

#define LIST_CONTAINS(name, list_type, item_type)\
    static bool name##_list_contains(list_type *list, item_type *item){\
        unsigned int i;\
        for(i = 0; i < list->count; ++i)\
            if(list->list+i == item)\
                return true;\
        return false;\
    }

LIST_CONTAINS(vars,     struct bsmp_var_info_list,      struct bsmp_var_info)
LIST_CONTAINS(groups,   struct bsmp_group_list,         struct bsmp_group)
LIST_CONTAINS(curves,   struct bsmp_curve_info_list,    struct bsmp_curve_info)
LIST_CONTAINS(funcs,    struct bsmp_func_info_list,     struct bsmp_func_info)

static enum bsmp_err command(bsmp_client_t *client, struct bsmp_message *request,
                             struct bsmp_message *response)
{
    if(!client || !request || !response)
        return BSMP_ERR_PARAM_INVALID;

    struct
    {
        uint8_t data[BSMP_MAX_MESSAGE];
        uint32_t size;
    }send_buf, recv_buf;

    // Prepare buffer with the message to be sent
    send_buf.data[0] = request->code;      // Code in the first byte
    send_buf.data[1] = request->payload_size >> 8;
    send_buf.data[2] = request->payload_size;

    // Payload in the subsequent bytes
    memcpy(&send_buf.data[BSMP_HEADER_SIZE], request->payload,
           request->payload_size);

    // Send request
    send_buf.size = BSMP_HEADER_SIZE + request->payload_size;
    if(client->send(send_buf.data, &send_buf.size))
        return BSMP_ERR_COMM;

    // Receive response
    if(client->recv(recv_buf.data, &recv_buf.size))
        return BSMP_ERR_COMM;

    // Must receive, at least, command and size
    if(recv_buf.size < 2)
        return BSMP_ERR_COMM;

    // Copy command code and get payload size
    response->code = recv_buf.data[0];
    response->payload_size = (recv_buf.data[1] << 8) | recv_buf.data[2];

    // Copy response
    memcpy(response->payload, &recv_buf.data[BSMP_HEADER_SIZE], recv_buf.size);

    return BSMP_SUCCESS;
}

static enum bsmp_err get_version(bsmp_client_t *client)
{
    if(!client)
        return BSMP_ERR_PARAM_INVALID;

    struct bsmp_message response, request =
    {
        .code = CMD_QUERY_VERSION,
        .payload_size = 0
    };

    if(command(client, &request, &response))
        return BSMP_ERR_COMM;

    // Special case: v1.0
    if(response.code == CMD_ERR_OP_NOT_SUPPORTED)
    {
        client->server_version.major    = 1;
        client->server_version.minor    = 0;
        client->server_version.revision = 0;
    }
    else
    {
        client->server_version.major    = response.payload[0];
        client->server_version.minor    = response.payload[1];
        client->server_version.revision = response.payload[2];
    }

    struct bsmp_version *v = &client->server_version;
    snprintf(v->str, BSMP_VERSION_STR_MAX_LEN, "%d.%02d.%03d", v->major,
             v->minor, v->revision);

    return BSMP_SUCCESS;
}

static enum bsmp_err update_vars_list(bsmp_client_t *client)
{
    if(!client)
        return BSMP_ERR_PARAM_INVALID;

    struct bsmp_message response, request =
    {
        .code = CMD_VAR_QUERY_LIST,
        .payload_size = 0
    };

    if(command(client, &request, &response) || response.code != CMD_VAR_LIST)
        return BSMP_ERR_COMM;

    // Zero list
    memset(&client->vars, 0, sizeof(client->vars));

    // Number of bytes in the payload corresponds to the number of vars in the
    // server
    client->vars.count = response.payload_size;

    unsigned int i;
    for(i = 0; i < client->vars.count; ++i)
    {
        client->vars.list[i].id       = i;
        client->vars.list[i].writable = response.payload[i] & WRITABLE_MASK;
        client->vars.list[i].size     = response.payload[i] & SIZE_MASK;

        if(!client->vars.list[i].size)
            client->vars.list[i].size = BSMP_VAR_MAX_SIZE;
    }

    return BSMP_SUCCESS;
}

static enum bsmp_err update_groups_list(bsmp_client_t *client)
{
    if(!client)
        return BSMP_ERR_PARAM_INVALID;

    struct bsmp_message response, request =
    {
        .code = CMD_GROUP_QUERY_LIST,
        .payload_size = 0
    };

    if(command(client, &request, &response))
        return BSMP_ERR_COMM;

    if(response.code != CMD_GROUP_LIST)
        return BSMP_ERR_COMM;           // TODO: better error code

    // Zero list
    memset(&client->groups, 0, sizeof(client->groups));

    // Number of bytes in the payload corresponds to the number of groups in the
    // server
    client->groups.count = response.payload_size;

    // Fill information for each group
    enum bsmp_err err_code;
    unsigned int i;
    for(i = 0; i < client->groups.count; ++i)
    {
        // Fill in its info
        struct bsmp_group *grp = &client->groups.list[i];

        grp->id         = i;
        grp->size       = 0;
        grp->writable   = response.payload[i] & WRITABLE_MASK;
        grp->vars.count = response.payload[i] & SIZE_MASK;

        // Query each group's variables list
        struct bsmp_message grp_response, grp_request = {
            .code           = CMD_GROUP_QUERY,
            .payload_size   = 1,
            .payload        = {i}
        };

        if(command(client, &grp_request, &grp_response) ||
                   grp_response.code != CMD_GROUP)
        {
            err_code = BSMP_ERR_COMM;
            goto err;
        }

        // Each byte in the response is a variable id
        unsigned int j;
        struct bsmp_var_info *var;
        for(j = 0; j < grp_response.payload_size; ++j)
        {
            var = &client->vars.list[grp_response.payload[j]];
            grp->vars.list[j] = var;
            grp->size += var->size;
        }
    }

    return BSMP_SUCCESS;

err:
    client->groups.count = 0;
    return err_code;
}

static enum bsmp_err update_curves_list(bsmp_client_t *client)
{
    if(!client)
        return BSMP_ERR_PARAM_INVALID;

    struct bsmp_message response, request =
    {
        .code = CMD_CURVE_QUERY_LIST,
        .payload_size = 0
    };

    if(command(client, &request, &response) || response.code != CMD_CURVE_LIST)
        return BSMP_ERR_COMM;

    // Zero list
    memset(&client->curves, 0, sizeof(client->curves));

    // Each 3-byte block in the response correspond to a curve
    client->curves.count = response.payload_size/BSMP_CURVE_LIST_INFO;

    unsigned int i;
    uint8_t *payloadp = response.payload;
    for(i = 0; i < client->curves.count; ++i)
    {
        struct bsmp_curve_info *curve = &client->curves.list[i];

        curve->id            = i;
        curve->writable      = *(payloadp++);
        curve->block_size    = *(payloadp++) << 8;
        curve->block_size   += *(payloadp++);
        curve->nblocks       = *(payloadp++) << 8;
        curve->nblocks      += *(payloadp++);

        if(!curve->nblocks)
            curve->nblocks = BSMP_CURVE_MAX_BLOCKS;

        struct bsmp_message response_csum, request_csum =
        {
            .code = CMD_CURVE_QUERY_CSUM,
            .payload = {i},
            .payload_size = 1
        };

        if(command(client, &request_csum, &response_csum) ||
           response_csum.code != CMD_CURVE_CSUM)
            continue;

        memcpy(curve->checksum, response_csum.payload, BSMP_CURVE_CSUM_SIZE);
    }

    return BSMP_SUCCESS;
}

static enum bsmp_err update_funcs_list(bsmp_client_t *client)
{
    if(!client)
        return BSMP_ERR_PARAM_INVALID;

    struct bsmp_message response, request =
    {
        .code = CMD_FUNC_QUERY_LIST,
        .payload_size = 0
    };

    if(command(client, &request, &response) || response.code != CMD_FUNC_LIST)
        return BSMP_ERR_COMM;

    // Zero list
    memset(&client->funcs, 0, sizeof(client->funcs));

    // Number of bytes in the payload corresponds to the number of funcs in the
    // server
    client->funcs.count = response.payload_size;

    unsigned int i;
    for(i = 0; i < client->funcs.count; ++i)
    {
        client->funcs.list[i].id            = i;
        client->funcs.list[i].input_size    = (response.payload[i] & 0xF0) >> 4;
        client->funcs.list[i].output_size   = (response.payload[i] & 0x0F);
    }

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_client_init (bsmp_client_t *client,
                                bsmp_comm_func_t send_func,
                                bsmp_comm_func_t recv_func)
{
    if(!client)
        return BSMP_ERR_PARAM_INVALID;

    client->send = send_func;
    client->recv = recv_func;

    client->vars.count = 0;
    memset(&client->vars, 0, sizeof(client->vars));

    client->groups.count = 0;
    memset(&client->groups, 0, sizeof(client->groups));

    client->curves.count = 0;
    memset(&client->curves, 0, sizeof(client->curves));

    client->funcs.count = 0;
    memset(&client->funcs, 0, sizeof(client->funcs));

    enum bsmp_err err;

    if((err = get_version(client)))
        return err;

    if((err = update_vars_list(client)))
        return err;

    if((err = update_groups_list(client)))
        return err;

    if((err = update_curves_list(client)))
        return err;

    if((err = update_funcs_list(client)))
        return err;

    return BSMP_SUCCESS;
}

#define BSMP_GET_LIST(name, type)\
    enum bsmp_err bsmp_get_##name##_list (bsmp_client_t *client, type **list) {\
        if(!client || !list)\
            return BSMP_ERR_PARAM_INVALID;\
        *list = &client->name;\
        return BSMP_SUCCESS;\
    }

BSMP_GET_LIST(vars,     struct bsmp_var_info_list)
BSMP_GET_LIST(groups,   struct bsmp_group_list)
BSMP_GET_LIST(curves,   struct bsmp_curve_info_list)
BSMP_GET_LIST(funcs,    struct bsmp_func_info_list)

struct bsmp_version *bsmp_get_version(bsmp_client_t *client)
{
    if(client)
        return &client->server_version;
    return NULL;
}

enum bsmp_err bsmp_read_var (bsmp_client_t *client, struct bsmp_var_info *var,
                             uint8_t *value)
{
    if(!client || !var || !value)
        return BSMP_ERR_PARAM_INVALID;

    if(!vars_list_contains(&client->vars, var))
        return BSMP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct bsmp_message response, request =
    {
        .code = CMD_VAR_READ,
        .payload = {var->id},
        .payload_size = 1

    };

    if(command(client, &request, &response))
        return BSMP_ERR_COMM;

    if(response.code != CMD_VAR_VALUE)
        return BSMP_ERR_COMM;   //TODO: better error?

    // Give back answer
    memcpy(value, response.payload, response.payload_size);

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_write_var (bsmp_client_t *client, struct bsmp_var_info *var,
                              uint8_t *value)
{
    if(!client || !var || !value)
        return BSMP_ERR_PARAM_INVALID;

    if(!vars_list_contains(&client->vars, var))
        return BSMP_ERR_PARAM_INVALID;

    if(!var->writable)
        return BSMP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct bsmp_message request = {
        .code = CMD_VAR_WRITE,
        .payload = {var->id},
        .payload_size = 1 + var->size
    }, response;

    memcpy(&request.payload[1], value, var->size);

    if(command(client, &request, &response))
       return BSMP_ERR_COMM;

    if(response.code != CMD_OK)
       return BSMP_ERR_COMM;   //TODO: better error?

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_write_read_vars (bsmp_client_t *client,
                                    struct bsmp_var_info *write_var,
                                    uint8_t *write_value,
                                    struct bsmp_var_info *read_var,
                                    uint8_t *read_value)
{
    if(!(client && write_var && write_value && read_var && read_value))
        return BSMP_ERR_PARAM_INVALID;

    if(!vars_list_contains(&client->vars, write_var) || !write_var->writable)
        return BSMP_ERR_PARAM_INVALID;

    if(!vars_list_contains(&client->vars, read_var))
        return BSMP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct bsmp_message request = {
        .code = CMD_VAR_WRITE_READ,
        .payload = {write_var->id, read_var->id},
        .payload_size = 2 + write_var->size
    }, response;

    memcpy(&request.payload[2], write_value, write_var->size);

    if(command(client, &request, &response))
       return BSMP_ERR_COMM;

    if(response.code != CMD_VAR_VALUE)
       return BSMP_ERR_COMM;   //TODO: better error?

    memcpy(read_value, response.payload, read_var->size);

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_read_group (bsmp_client_t *client, struct bsmp_group *grp,
                               uint8_t *values)
{
    if(!client || !grp || !values)
        return BSMP_ERR_PARAM_INVALID;

    if(!groups_list_contains(&client->groups, grp))
        return BSMP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct bsmp_message response, request = {
        .code = CMD_GROUP_READ,
        .payload = {grp->id},
        .payload_size = 1
    };

    if(command(client, &request, &response))
        return BSMP_ERR_COMM;

    if(response.code != CMD_GROUP_VALUES)
        return BSMP_ERR_COMM;   //TODO: better error?

    // Give back answer
    memcpy(values, response.payload, response.payload_size);

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_write_group (bsmp_client_t *client, struct bsmp_group *grp,
                                uint8_t *values)
{
    if(!client || !grp || !values)
        return BSMP_ERR_PARAM_INVALID;

    if(!groups_list_contains(&client->groups, grp))
        return BSMP_ERR_PARAM_INVALID;

    if(!grp->writable)
        return BSMP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct bsmp_message response, request = {
        .code = CMD_GROUP_WRITE,
        .payload = {grp->id},
        .payload_size = 1 + grp->size
    };

    memcpy(&request.payload[1], values, grp->size);

    if(command(client, &request, &response))
       return BSMP_ERR_COMM;

    if(response.code != CMD_OK)
       return BSMP_ERR_COMM;   //TODO: better error?

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_bin_op_var (bsmp_client_t *client, enum bsmp_bin_op op,
                               struct bsmp_var_info *var, uint8_t *mask)
{
    if(!client || !var || !mask)
        return BSMP_ERR_PARAM_INVALID;

    if(!vars_list_contains(&client->vars, var))
        return BSMP_ERR_PARAM_INVALID;

    if(!var->writable)
        return BSMP_ERR_PARAM_INVALID;

    if(op >= BIN_OP_COUNT)
        return BSMP_ERR_PARAM_OUT_OF_RANGE;

    // Prepare message to be sent
    struct bsmp_message response, request = {
        .code = CMD_VAR_BIN_OP,
        .payload = {var->id, bin_op_code[op]},
        .payload_size = 2 + var->size
    };

    memcpy(&request.payload[2], mask, var->size);

    if(command(client, &request, &response))
       return BSMP_ERR_COMM;

    if(response.code != CMD_OK)
       return BSMP_ERR_COMM;   //TODO: better error?

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_bin_op_group (bsmp_client_t *client, enum bsmp_bin_op op,
                                 struct bsmp_group *grp, uint8_t *mask)
{
    if(!client || !grp || !mask)
        return BSMP_ERR_PARAM_INVALID;

    if(!groups_list_contains(&client->groups, grp))
        return BSMP_ERR_PARAM_INVALID;

    if(!grp->writable)
        return BSMP_ERR_PARAM_INVALID;

    if(op >= BIN_OP_COUNT)
        return BSMP_ERR_PARAM_OUT_OF_RANGE;

    // Prepare message to be sent
    struct bsmp_message response, request = {
        .code = CMD_GROUP_BIN_OP,
        .payload = {grp->id, bin_op_code[op]},
        .payload_size = 2 + grp->size
    };

    memcpy(&request.payload[2], mask, grp->size);

    if(command(client, &request, &response))
       return BSMP_ERR_COMM;

    if(response.code != CMD_OK)
       return BSMP_ERR_COMM;   //TODO: better error?

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_create_group (bsmp_client_t *client,
                                 struct bsmp_var_info **list)
{
    if(!client || !list || !(*list))
        return BSMP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct bsmp_message request = {
        .code = CMD_GROUP_CREATE,
        .payload_size = 0
    }, response;

    while(*list)
    {
        if(!vars_list_contains(&client->vars, *list))
            return BSMP_ERR_PARAM_INVALID;

        request.payload[request.payload_size++] = (*(list++))->id;
    }

    if(!request.payload_size)
        return BSMP_ERR_PARAM_INVALID;

    if(command(client, &request, &response))
        return BSMP_ERR_COMM;

    if(response.code != CMD_OK)
        return BSMP_ERR_COMM;

    update_groups_list(client);

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_remove_all_groups (bsmp_client_t *client)
{
    if(!client)
        return BSMP_ERR_PARAM_INVALID;

    struct bsmp_message response, request = {
        .code = CMD_GROUP_REMOVE_ALL,
        .payload_size = 0
    };

    if(command(client, &request, &response) || response.code != CMD_OK)
        return BSMP_ERR_COMM;

    update_groups_list(client);

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_request_curve_block (bsmp_client_t *client,
                                        struct bsmp_curve_info *curve,
                                        uint16_t offset, uint8_t *data,
                                        uint16_t *len)
{
    if(!client || !curve || !data || !len)
        return BSMP_ERR_PARAM_INVALID;

    if(!curves_list_contains(&client->curves, curve))
        return BSMP_ERR_PARAM_INVALID;

    if(offset > curve->nblocks)
        return BSMP_ERR_PARAM_OUT_OF_RANGE;

    struct bsmp_message response, request = {
        .code = CMD_CURVE_BLOCK_REQUEST,
        .payload = {curve->id, offset >> 8, offset},
        .payload_size = BSMP_CURVE_BLOCK_INFO
    };

    if(command(client, &request, &response) || response.code !=CMD_CURVE_BLOCK)
        return BSMP_ERR_COMM;

    *len = response.payload_size - BSMP_CURVE_BLOCK_INFO;
    memcpy(data, response.payload + BSMP_CURVE_BLOCK_INFO, *len);

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_read_curve (bsmp_client_t *cli, struct bsmp_curve_info *cur,
                               uint8_t *buf, uint32_t *len)
{
    // Check parameters
    if(!cli || !cur || !buf || !len)
        return BSMP_ERR_PARAM_INVALID;

    if(!curves_list_contains(&cli->curves, cur))
        return BSMP_ERR_PARAM_INVALID;

    enum bsmp_err err;          // Error code
    uint16_t      blk;          // Current block offset
    uint8_t       *bufp = buf;  // Pointer to the current region of the buffer
    uint16_t      blklen;       // Length of the last returned block

    // Iterate over blocks
    *len = 0;
    for(blk = 0; blk < cur->nblocks; ++blk)
    {
        if((err = bsmp_request_curve_block(cli, cur, blk, bufp, &blklen)))
        {
            *len = 0;
            return err;
        }

        *len += blklen;

        if(blklen < cur->block_size)
            break;

        bufp += cur->block_size;
    }

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_send_curve_block (bsmp_client_t *client,
                                     struct bsmp_curve_info *curve,
                                     uint16_t offset, uint8_t *data,
                                     uint16_t len)
{
    if(!client || !curve || !data)
        return BSMP_ERR_PARAM_INVALID;

    if(!curves_list_contains(&client->curves, curve))
        return BSMP_ERR_PARAM_INVALID;

    if(!curve->writable)
        return BSMP_ERR_PARAM_INVALID;

    if(offset > curve->nblocks)
        return BSMP_ERR_PARAM_OUT_OF_RANGE;

    if(len > curve->block_size)
        return BSMP_ERR_PARAM_OUT_OF_RANGE;

    struct bsmp_message response, request = {
        .code = CMD_CURVE_BLOCK,
        .payload = {curve->id, offset >> 8, offset},
        .payload_size = len + BSMP_CURVE_BLOCK_INFO,
    };

    memcpy(request.payload + BSMP_CURVE_BLOCK_INFO, data, len);

    if(command(client, &request, &response) || response.code != CMD_OK)
        return BSMP_ERR_COMM;

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_write_curve (bsmp_client_t *cli, struct bsmp_curve_info *cur,
                                uint8_t *buf, uint32_t len)
{
    // Check parameters
    if(!cli || !cur || !buf)
        return BSMP_ERR_PARAM_INVALID;

    if(!curves_list_contains(&cli->curves, cur))
        return BSMP_ERR_PARAM_INVALID;

    enum bsmp_err err;          // Error code
    uint16_t      blk;          // Current block offset
    uint8_t       *bufp = buf;  // Pointer to the current region of the buffer
    uint16_t      blklen;       // Length of the current block

    // Iterate over blocks
    for(blk = 0; blk < cur->nblocks; ++blk)
    {
        blklen = len < cur->block_size ? len : cur->block_size;

        if((err = bsmp_send_curve_block(cli, cur, blk, bufp, blklen)))
            return err;

        len -= blklen;

        bufp += blklen;
    }

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_recalc_checksum (bsmp_client_t *client,
                                    struct bsmp_curve_info *curve)
{
    if(!client || !curve)
        return BSMP_ERR_PARAM_INVALID;

    if(!curves_list_contains(&client->curves, curve))
        return BSMP_ERR_PARAM_INVALID;

    struct bsmp_message response, request = {
        .code = CMD_CURVE_RECALC_CSUM,
        .payload = {curve->id},
        .payload_size = 1
    };

    if(command(client, &request, &response))
        return BSMP_ERR_COMM;

    if(response.code != CMD_OK)
        return BSMP_ERR_COMM;

    update_curves_list(client);

    return BSMP_SUCCESS;
}

enum bsmp_err bsmp_func_execute (bsmp_client_t *client,
                                 struct bsmp_func_info *func, uint8_t *error,
                                 uint8_t *input, uint8_t *output)
{
    if(!client || !func || !error)
        return BSMP_ERR_PARAM_INVALID;

    if(!funcs_list_contains(&client->funcs, func))
        return BSMP_ERR_PARAM_INVALID;

    if(func->input_size && !input)
        return BSMP_ERR_PARAM_INVALID;

    if(func->output_size && !output)
        return BSMP_ERR_PARAM_INVALID;

    struct bsmp_message response, request = {
        .code = CMD_FUNC_EXECUTE,
        .payload = {func->id},
        .payload_size = 1 + func->input_size
    };

    if(func->input_size)
        memcpy(&request.payload[1], input, func->input_size);

    if(command(client, &request, &response))
        return BSMP_ERR_COMM;

    if(response.code == CMD_FUNC_RETURN)
    {
        *error = 0;
        if(func->output_size)
            memcpy(output, response.payload, func->output_size);
        return BSMP_SUCCESS;
    }
    else if(response.code == CMD_FUNC_ERROR)
    {
        *error = response.payload[0];
        return BSMP_SUCCESS;
    }
    else
        return BSMP_ERR_COMM;
}
