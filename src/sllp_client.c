#include "sllp_client.h"
#include "common.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct sllp_client
{
    bool                    initialized;
    sllp_comm_func_t        send, recv;
    struct sllp_vars_list   vars;
    struct sllp_groups_list groups;
    struct sllp_curves_list curves;
    struct sllp_status      status;
};

static char bin_op_code[BIN_OP_COUNT] =
{
    [BIN_OP_AND]    = 'A',
    [BIN_OP_OR]     = 'O',
    [BIN_OP_XOR]    = 'X',
    [BIN_OP_SET]    = 'S',
    [BIN_OP_CLEAR]  = 'C',
    [BIN_OP_TOGGLE] = 'T',
};

struct sllp_message
{
    enum command_code   code;
    uint32_t            payload_size;
    uint8_t             payload[MAX_PAYLOAD];
};

static bool vars_list_contains (struct sllp_vars_list *list,
                                struct sllp_var_info *var)
{
    unsigned int i;
    for(i = 0; i < list->count; ++i)
        if(list->list+i == var)
            return true;
    return false;
}

static bool groups_list_contains (struct sllp_groups_list *list,
                                  struct sllp_group *grp)
{
    unsigned int i;
    for(i = 0; i < list->count; ++i)
        if(list->list+i == grp)
            return true;
    return false;
}

static bool curves_list_contains (struct sllp_curves_list *list,
                                  struct sllp_curve_info *curve)
{
    unsigned int i;
    for(i = 0; i < list->count; ++i)
        if(list->list+i == curve)
            return true;
    return false;
}

static enum sllp_err command(sllp_client_t *client, struct sllp_message *request,
                             struct sllp_message *response)
{
    if(!client || !request || !response)
        return SLLP_ERR_PARAM_INVALID;

    struct
    {
        uint8_t data[MAX_MESSAGE];
        uint32_t size;
    }send_buf, recv_buf;

    // Prepare buffer with the message to be sent
    send_buf.data[0] = request->code;      // Code in the first byte

    if(request->payload_size < MAX_PAYLOAD_ENCODED)  // Size in the second byte
        send_buf.data[1] = request->payload_size;
    else if(request->payload_size == MAX_PAYLOAD)
        send_buf.data[1] = MAX_PAYLOAD_ENCODED;
    else
        return SLLP_ERR_PARAM_INVALID;

    // Payload in the subsequent bytes
    memcpy(&send_buf.data[2], request->payload, request->payload_size);

    // Send request
    send_buf.size = 2 + request->payload_size;
    if(client->send(send_buf.data, &send_buf.size))
        return SLLP_ERR_COMM;

    // Receive response
    if(client->recv(recv_buf.data, &recv_buf.size))
        return SLLP_ERR_COMM;

    // Must receive, at least, command and size
    if(recv_buf.size < 2)
        return SLLP_ERR_COMM;

    // Copy command code
    response->code = recv_buf.data[0];

    // Decode size
    if(recv_buf.data[1] == MAX_PAYLOAD_ENCODED)
        response->payload_size = MAX_PAYLOAD;
    else
        response->payload_size = recv_buf.data[1];

    // Copy response
    memcpy(response->payload, &recv_buf.data[2], recv_buf.size);

    return SLLP_SUCCESS;
}

static enum sllp_err update_vars_list(sllp_client_t *client)
{
    if(!client)
        return SLLP_ERR_PARAM_INVALID;

    struct sllp_message response, request =
    {
        .code = CMD_QUERY_VARS_LIST,
        .payload_size = 0
    };

    if(command(client, &request, &response) || response.code != CMD_VARS_LIST)
        return SLLP_ERR_COMM;

    // There are no variables in the server
    if(!response.payload_size)
        return SLLP_SUCCESS;

    // Number of bytes in the payload corresponds to the number of vars in the
    // server
    client->vars.count = response.payload_size;

    // If there was a previous list, free it
    if(client->vars.list)
        free(client->vars.list);

    client->vars.list = malloc(client->vars.count * sizeof(*client->vars.list));

    if(!client->vars.list)
        return SLLP_ERR_OUT_OF_MEMORY;

    unsigned int i;
    for(i = 0; i < client->vars.count; ++i)
    {
        client->vars.list[i].id       = i;
        client->vars.list[i].writable = response.payload[i] & WRITABLE_MASK;
        client->vars.list[i].size     = response.payload[i] & SIZE_MASK;
    }

    return SLLP_SUCCESS;
}

static enum sllp_err update_groups_list(sllp_client_t *client)
{
    if(!client)
        return SLLP_ERR_PARAM_INVALID;

    struct sllp_message response, request =
    {
        .code = CMD_QUERY_GROUPS_LIST,
        .payload_size = 0
    };

    if(command(client, &request, &response))
        return SLLP_ERR_COMM;

    if(response.code != CMD_GROUPS_LIST)
        return SLLP_ERR_COMM;           // TODO: better error code

    // Free previously allocated list
    if(client->groups.list)
    {
        free(client->groups.list);
        client->groups.list = NULL;
        client->groups.count = 0;
    }

    // There are no groups in the server
    if(!response.payload_size)
        return SLLP_SUCCESS;

    // Allocate new list
    client->groups.list =
            malloc(response.payload_size*sizeof(*client->groups.list));

    if(!client->groups.list)
        return SLLP_ERR_OUT_OF_MEMORY;

    // Zero list
    memset(client->groups.list, 0,
           response.payload_size*sizeof(*client->groups.list));

    // Number of bytes in the payload corresponds to the number of groups in the
    // server
    client->groups.count = response.payload_size;

    // Fill information for each group
    enum sllp_err err_code;
    unsigned int i;
    for(i = 0; i < client->groups.count; ++i)
    {
        // Fill in its info
        struct sllp_group *grp = &client->groups.list[i];

        grp->id         = i;
        grp->size       = 0;
        grp->writable   = response.payload[i] & WRITABLE_MASK;
        grp->vars.count = response.payload[i] & SIZE_MASK;
        grp->vars.list  = malloc(grp->vars.count * sizeof(*grp->vars.list));

        // Check vars list allocation
        if(!grp->vars.list)
        {
            err_code = SLLP_ERR_OUT_OF_MEMORY;
            goto err;
        }

        // Query each group's variables list
        struct sllp_message grp_response, grp_request = {
            .code           = CMD_QUERY_GROUP,
            .payload_size   = 1,
            .payload        = {i}
        };

        if(command(client, &grp_request, &grp_response) ||
                   grp_response.code != CMD_GROUP)
        {
            err_code = SLLP_ERR_COMM;
            goto err;
        }

        // Each byte in the response is a variable id
        unsigned int j;
        struct sllp_var_info *var;
        for(j = 0; j < grp_response.payload_size; ++j)
        {
            var = &client->vars.list[grp_response.payload[j]];
            grp->vars.list[j] = var;
            grp->size += var->size;
        }
    }

    return SLLP_SUCCESS;

err:
    while(i)
        if(client->groups.list[i].vars.list)
            free(client->groups.list[i--].vars.list);

    free(client->groups.list);
    client->groups.count = 0;
    client->groups.list  = NULL;

    return err_code;
}

static enum sllp_err update_curves_list(sllp_client_t *client)
{
    if(!client)
        return SLLP_ERR_PARAM_INVALID;

    struct sllp_message response, request =
    {
        .code = CMD_QUERY_CURVES_LIST,
        .payload_size = 0
    };

    if(command(client, &request, &response) || response.code != CMD_CURVES_LIST)
        return SLLP_ERR_COMM;

    // There are no curves in the server
    if(!response.payload_size)
        return SLLP_SUCCESS;

    // Each 18-byte block in the response correspond to a curve
    client->curves.count = response.payload_size/CURVE_INFO_SIZE;

    // If there was a previous list, free it
    if(client->curves.list)
        free(client->curves.list);

    client->curves.list = malloc(client->curves.count * sizeof(*client->curves.list));

    if(!client->curves.list)
        return SLLP_ERR_OUT_OF_MEMORY;

    unsigned int i;
    for(i = 0; i < client->curves.count; ++i)
    {
        struct sllp_curve_info *curve = &client->curves.list[i];
        uint8_t *curve_info = &response.payload[i*CURVE_INFO_SIZE];

        curve->id       = i;
        curve->writable = curve_info[0];
        curve->nblocks  = curve_info[1] + 1;
        memcpy(curve->checksum, &curve_info[2], sizeof(curve->checksum));
    }

    return SLLP_SUCCESS;
}

sllp_client_t *sllp_client_new (sllp_comm_func_t send_func,
                                sllp_comm_func_t recv_func)
{
    if(!send_func || !recv_func)
        return NULL;

    struct sllp_client *client = malloc(sizeof(*client));

    if(!client)
        return NULL;

    client->send = send_func;
    client->recv = recv_func;

    client->vars.count = 0;
    client->vars.list = NULL;

    client->groups.count = 0;
    client->groups.list = NULL;

    client->curves.count = 0;
    client->curves.list = NULL;

    client->status.status = 0;
    client->initialized = false;

    return client;
}

enum sllp_err sllp_client_destroy (sllp_client_t *client)
{
    if(!client)
        return SLLP_ERR_PARAM_INVALID;

    if(client->vars.list)
        free(client->vars.list);

    if(client->groups.list)
    {
        unsigned int i;
        for(i = 0; i < client->groups.count; ++i)
            if(client->groups.list[i].vars.list)
                free(client->groups.list[i].vars.list);
        free(client->groups.list);
    }

    if(client->curves.list)
        free(client->curves.list);

    free(client);

    return SLLP_SUCCESS;
}

enum sllp_err sllp_client_init(sllp_client_t *client)
{
    if(!client)
        return SLLP_ERR_PARAM_INVALID;

    enum sllp_err err;

    if((err = update_vars_list(client)))
        return err;

    if((err = update_groups_list(client)))
        return err;

    if((err = update_curves_list(client)))
        return err;

    client->initialized = true;
    return SLLP_SUCCESS;
}

enum sllp_err sllp_get_vars_list (sllp_client_t *client,
                                  struct sllp_vars_list **list)
{
    if(!client || !list)
        return SLLP_ERR_PARAM_INVALID;

    *list = &client->vars;
    return SLLP_SUCCESS;
}

enum sllp_err sllp_get_groups_list (sllp_client_t *client,
                                    struct sllp_groups_list **list)
{
    if(!client || !list)
        return SLLP_ERR_PARAM_INVALID;

    *list = &client->groups;
    return SLLP_SUCCESS;
}

enum sllp_err sllp_get_curves_list (sllp_client_t *client,
                                    struct sllp_curves_list **list)
{
    if(!client || !list)
        return SLLP_ERR_PARAM_INVALID;

    *list = &client->curves;
    return SLLP_SUCCESS;
}

enum sllp_err sllp_get_status (sllp_client_t* client,
                               struct sllp_status **status)
{
    client = NULL;
    status = NULL;
    return SLLP_SUCCESS;
}

enum sllp_err sllp_read_var (sllp_client_t *client, struct sllp_var_info *var,
                             uint8_t *value)
{
    if(!client || !var || !value)
        return SLLP_ERR_PARAM_INVALID;

    if(!vars_list_contains(&client->vars, var))
        return SLLP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct sllp_message response, request =
    {
        .code = CMD_READ_VAR,
        .payload = {var->id},
        .payload_size = 1

    };

    if(command(client, &request, &response))
        return SLLP_ERR_COMM;

    if(response.code != CMD_VAR_READING)
        return SLLP_ERR_COMM;   //TODO: better error?

    // Give back answer
    memcpy(value, response.payload, response.payload_size);

    return SLLP_SUCCESS;
}

enum sllp_err sllp_write_var (sllp_client_t *client, struct sllp_var_info *var,
                              uint8_t *value)
{
    //if(!client || !var || !value)
        //return SLLP_ERR_PARAM_INVALID;

    if(!client)
        return SLLP_ERR_PARAM_INVALID;

    if(!var)
        return SLLP_ERR_PARAM_INVALID;

    if(!value)
        return SLLP_ERR_PARAM_INVALID;

    if(!vars_list_contains(&client->vars, var))
        return SLLP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct sllp_message request = {
        .code = CMD_WRITE_VAR,
        .payload = {var->id},
        .payload_size = 1 + var->size
    }, response;

    memcpy(&request.payload[1], value, var->size);

    if(command(client, &request, &response))
       return SLLP_ERR_COMM;

    if(response.code != CMD_OK)
       return SLLP_ERR_COMM;   //TODO: better error?

    return SLLP_SUCCESS;
}

enum sllp_err sllp_read_group (sllp_client_t *client, struct sllp_group *grp,
                               uint8_t *values)
{
    if(!client || !grp || !values)
        return SLLP_ERR_PARAM_INVALID;

    if(!groups_list_contains(&client->groups, grp))
        return SLLP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct sllp_message response, request = {
        .code = CMD_READ_GROUP,
        .payload = {grp->id},
        .payload_size = 1
    };

    if(command(client, &request, &response))
        return SLLP_ERR_COMM;

    if(response.code != CMD_VAR_READING)
        return SLLP_ERR_COMM;   //TODO: better error?

    // Give back answer
    memcpy(values, response.payload, response.payload_size);

    return SLLP_SUCCESS;
}

enum sllp_err sllp_write_group (sllp_client_t *client, struct sllp_group *grp,
                                uint8_t *values)
{
    if(!client || !grp || !values)
        return SLLP_ERR_PARAM_INVALID;

    if(!groups_list_contains(&client->groups, grp))
        return SLLP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct sllp_message response, request = {
        .code = CMD_WRITE_GROUP,
        .payload = {grp->id},
        .payload_size = 1 + grp->size
    };

    memcpy(&request.payload[1], values, grp->size);

    if(command(client, &request, &response))
       return SLLP_ERR_COMM;

    if(response.code != CMD_OK)
       return SLLP_ERR_COMM;   //TODO: better error?

    return SLLP_SUCCESS;
}

enum sllp_err sllp_bin_op_var (sllp_client_t *client, enum sllp_bin_op op,
                               struct sllp_var_info *var, uint8_t *mask)
{
    if(!client || !var || !mask)
        return SLLP_ERR_PARAM_INVALID;

    if(!vars_list_contains(&client->vars, var))
        return SLLP_ERR_PARAM_INVALID;

    if(op >= BIN_OP_COUNT)
        return SLLP_ERR_PARAM_OUT_OF_RANGE;

    // Prepare message to be sent
    struct sllp_message response, request = {
        .code = CMD_BIN_OP_VAR,
        .payload = {var->id, bin_op_code[op]},
        .payload_size = 2 + var->size
    };

    memcpy(&request.payload[2], mask, var->size);

    if(command(client, &request, &response))
       return SLLP_ERR_COMM;

    if(response.code != CMD_OK)
       return SLLP_ERR_COMM;   //TODO: better error?

    return SLLP_SUCCESS;
}

enum sllp_err sllp_bin_op_group (sllp_client_t *client, enum sllp_bin_op op,
                                 struct sllp_group *grp, uint8_t *mask)
{
    if(!client || !grp || !mask)
        return SLLP_ERR_PARAM_INVALID;

    if(!groups_list_contains(&client->groups, grp))
        return SLLP_ERR_PARAM_INVALID;

    if(op >= BIN_OP_COUNT)
        return SLLP_ERR_PARAM_OUT_OF_RANGE;

    // Prepare message to be sent
    struct sllp_message response, request = {
        .code = CMD_BIN_OP_GROUP,
        .payload = {grp->id, bin_op_code[op]},
        .payload_size = 2 + grp->size
    };

    memcpy(&request.payload[2], mask, grp->size);

    if(command(client, &request, &response))
       return SLLP_ERR_COMM;

    if(response.code != CMD_OK)
       return SLLP_ERR_COMM;   //TODO: better error?

    return SLLP_SUCCESS;
}

enum sllp_err sllp_create_group (sllp_client_t *client,
                                 struct sllp_var_info **vars_list)
{
    if(!client || !vars_list || !(*vars_list))
        return SLLP_ERR_PARAM_INVALID;

    // Prepare message to be sent
    struct sllp_message request = {
        .code = CMD_CREATE_GROUP,
        .payload_size = 0
    }, response;

    struct sllp_var_info *varp = vars_list[request.payload_size];

    while(varp && (request.payload_size < client->vars.count))
    {
        if(!vars_list_contains(&client->vars, varp))
            return SLLP_ERR_PARAM_INVALID;

        request.payload[request.payload_size++] = (varp++)->id;
    }

    if(!request.payload_size)
        return SLLP_ERR_PARAM_INVALID;

    if(!command(client, &request, &response))
        return SLLP_ERR_COMM;

    if(response.code != CMD_OK)
        return SLLP_ERR_COMM;

    update_groups_list(client);

    return SLLP_SUCCESS;
}

enum sllp_err sllp_remove_all_groups (sllp_client_t *client)
{
    if(!client)
        return SLLP_ERR_PARAM_INVALID;

    struct sllp_message response, request = {
        .code = CMD_REMOVE_ALL_GROUPS,
        .payload_size = 0
    };

    if(!command(client, &request, &response) || response.code != CMD_OK)
        return SLLP_ERR_COMM;

    return SLLP_SUCCESS;
}

enum sllp_err sllp_request_curve_block (sllp_client_t *client,
                                        struct sllp_curve_info *curve,
                                        uint8_t offset, uint8_t *data)
{
    if(!client || !curve || !data)
        return SLLP_ERR_PARAM_INVALID;

    if(!curves_list_contains(&client->curves, curve))
            return SLLP_ERR_PARAM_INVALID;

    if(offset >= curve->nblocks)
        return SLLP_ERR_PARAM_OUT_OF_RANGE;

    struct sllp_message response, request = {
        .code = CMD_CURVE_TRANSMIT,
        .payload = {curve->id, offset},
        .payload_size = 2
    };

    if(!command(client, &request, &response) || response.code != CMD_CURVE_BLOCK)
        return SLLP_ERR_COMM;

    memcpy(data, response.payload + 2, CURVE_BLOCK);

    return SLLP_SUCCESS;
}

enum sllp_err sllp_send_curve_block (sllp_client_t *client,
                                     struct sllp_curve_info *curve,
                                     uint8_t offset, uint8_t *data)
{
    if(!client || !curve || !data)
        return SLLP_ERR_PARAM_INVALID;

    if(!curves_list_contains(&client->curves, curve))
        return SLLP_ERR_PARAM_INVALID;

    if(offset >= curve->nblocks)
        return SLLP_ERR_PARAM_OUT_OF_RANGE;

    struct sllp_message response, request = {
        .code = CMD_CURVE_BLOCK,
        .payload = {curve->id, offset},
        .payload_size = MAX_PAYLOAD
    };

    memcpy(data, &request.payload[2], CURVE_BLOCK);

    if(!command(client, &request, &response) || response.code != CMD_OK)
        return SLLP_ERR_COMM;

    return SLLP_SUCCESS;
}

enum sllp_err sllp_recalc_checksum (sllp_client_t *client,
                                    struct sllp_curve_info *curve)
{
    if(!client || !curve)
        return SLLP_ERR_PARAM_INVALID;

    if(!curves_list_contains(&client->curves, curve))
        return SLLP_ERR_PARAM_INVALID;

    struct sllp_message response, request = {
        .code = CMD_CURVE_RECALC_CSUM,
        .payload = {curve->id},
        .payload_size = 1
    };

    if(command(client, &request, &response))
        return SLLP_ERR_COMM;

    if(response.code != CMD_OK)
        return SLLP_ERR_COMM;

    update_curves_list(client);

    return SLLP_SUCCESS;
}
