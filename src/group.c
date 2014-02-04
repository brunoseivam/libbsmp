#include "group.h"
#include "server_defs.h"
#include "sllp_server.h"

#include <stdlib.h>
#include <string.h>

void group_init (struct sllp_group *grp, uint8_t id)
{
    memset(grp, 0, sizeof(*grp));
    grp->id       = id;
    grp->writable = true;
}

void group_add_var (struct sllp_group *grp, struct sllp_var *var)
{
    grp->vars.list[grp->vars.count++]  = &var->info;
    grp->size                         += var->info.size;
    grp->writable                     &= var->info.writable;
}

static void group_to_mod_list (sllp_server_t *server, struct sllp_group *grp)
{
    unsigned int i;
    for(i = 0; i < grp->vars.count; ++i)
        server->modified_list[i] = server->vars.list[grp->vars.list[i]->id];
    server->modified_list[i] = NULL;
}

SERVER_CMD_FUNCTION (group_query_list)
{
    // Check payload size
    if(recv_msg->payload_size != 0)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Set answer's command_code and payload_size
    MESSSAGE_SET_ANSWER(send_msg, CMD_GROUP_LIST);

    // Add each group to the response
    struct sllp_group *grp;

    unsigned int i;
    for(i = 0; i < server->groups.count; ++i)
    {
        grp = &server->groups.list[i];
        send_msg->payload[i]  = grp->writable ? WRITABLE : READ_ONLY;
        send_msg->payload[i] += grp->vars.count;
    }
    send_msg->payload_size = server->groups.count;
}

SERVER_CMD_FUNCTION (group_query)
{
    // Check payload size
    if(recv_msg->payload_size != 1)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Set answer code
    MESSSAGE_SET_ANSWER(send_msg, CMD_GROUP);

    // Check ID
    uint8_t group_id = recv_msg->payload[0];
    if(group_id >= server->groups.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired group
    struct sllp_group *grp = &server->groups.list[group_id];

    unsigned int i;
    for(i = 0; i < grp->vars.count; ++i)
        send_msg->payload[i] = grp->vars.list[i]->id;

    send_msg->payload_size = grp->vars.count;
}

SERVER_CMD_FUNCTION (group_read)
{
    // Check payload size
    if(recv_msg->payload_size != 1)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check group ID
    uint8_t group_id = recv_msg->payload[0];

    if(group_id >= server->groups.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired group
    struct sllp_group *grp = &server->groups.list[group_id];

    // Call hook
    if(server->hook)
    {
        group_to_mod_list(server, grp);
        server->hook(SLLP_OP_READ, server->modified_list);
    }

    // Iterate over group's variables
    MESSSAGE_SET_ANSWER(send_msg, CMD_GROUP_VALUES);

    struct sllp_var *var;
    unsigned int i;
    uint8_t *payloadp = send_msg->payload;
    for(i = 0; i < grp->vars.count; ++i)
    {
        var = server->vars.list[grp->vars.list[i]->id];
        memcpy(payloadp, var->data, var->info.size);
        payloadp += var->info.size;
    }
    send_msg->payload_size = grp->size;
}

SERVER_CMD_FUNCTION (group_write)
{
    // Check if body has at least one byte (ID)
    if(recv_msg->payload_size < 1)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t group_id = recv_msg->payload[0];

    if(group_id >= server->groups.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired group
    struct sllp_group *grp = &server->groups.list[group_id];

    // Check payload size
    if(recv_msg->payload_size != 1 + grp->size)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check write permission
    if(!grp->writable)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_READ_ONLY);
        return;
    }

    // Everything is OK, iterate
    struct sllp_var *var;
    uint8_t *payloadp = recv_msg->payload + 1;
    unsigned int i;
    bool check_failed = false;

    for(i = 0; i < grp->vars.count; ++i)
    {
        var = server->vars.list[grp->vars.list[i]->id];

        // Check payload value
        if(var->value_ok && !var->value_ok(var, payloadp))
            check_failed = true;
        else
            memcpy(var->data, payloadp, var->info.size);
        payloadp += var->info.size;
    }

    // Call hook
    if(server->hook)
    {
        group_to_mod_list(server, grp);
        server->hook(SLLP_OP_WRITE, server->modified_list);
    }

    MESSSAGE_SET_ANSWER(send_msg, check_failed ? CMD_ERR_INVALID_VALUE:CMD_OK);
}

SERVER_CMD_FUNCTION (group_bin_op)
{
    // Check if body has at least two bytes (ID + binary operation)
    if(recv_msg->payload_size < 2)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t group_id = recv_msg->payload[0];

    if(group_id >= server->groups.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired group
    struct sllp_group *grp = &server->groups.list[group_id];

    // Get operation
    unsigned char operation = recv_msg->payload[1];

    // Check operation
    if(!bin_op[operation])
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_OP_NOT_SUPPORTED);
        return;
    }

    // Check payload size
    if(recv_msg->payload_size != 2 + grp->size)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check write permission
    if(!grp->writable)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_READ_ONLY);
        return;
    }

    // Everything is OK, iterate
    struct sllp_var *var;
    uint8_t *payloadp = recv_msg->payload + 2;

    unsigned int i;
    for(i = 0; i < grp->vars.count; ++i)
    {
        var = server->vars.list[grp->vars.list[i]->id];
        bin_op[operation](var->data, payloadp, var->info.size);
        payloadp += var->info.size;
    }

    // Call hook
    if(server->hook)
    {
        group_to_mod_list(server, grp);
        server->hook(SLLP_OP_WRITE, server->modified_list);
    }

    MESSSAGE_SET_ANSWER(send_msg, CMD_OK);
}

SERVER_CMD_FUNCTION (group_create)
{
    // Check if there's at least one variable to put on the group
    if(recv_msg->payload_size < 1 ||
       recv_msg->payload_size > server->vars.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check if there's available space for the new group
    if(server->groups.count == MAX_GROUPS)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INSUFFICIENT_MEMORY);
        return;
    }

    struct sllp_group *grp = &server->groups.list[server->groups.count];

    // Initialize group id
    group_init(grp, server->groups.count);

    // Populate group
    int i;
    for(i = 0; i < recv_msg->payload_size; ++i)
    {
        // Check var ID
        uint8_t var_id = recv_msg->payload[i];

        if(var_id >= server->vars.count)
        {
            MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
            return;
        }

        if(i && (var_id <= grp->vars.list[i-1]->id))
        {
            MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
            return;
        }

        // Add var by ID
        group_add_var(grp, server->vars.list[var_id]);
    }

    // Group created
    ++server->groups.count;

    // Prepare answer
    MESSSAGE_SET_ANSWER(send_msg, CMD_OK);
    return;
}

SERVER_CMD_FUNCTION (group_remove_all)
{
    if(recv_msg->payload_size != 0)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    server->groups.count = GROUP_STANDARD_COUNT;
    MESSSAGE_SET_ANSWER(send_msg, CMD_OK);
}
