#include "sllp_server.h"
#include "common.h"
#include "md5/md5.h"
#include "binops.h"

#include <stdlib.h>
#include <string.h>

#define VARIABLE_MIN_SIZE       1
#define VARIABLE_MAX_SIZE       127

#define MAX_VARIABLES           128
#define MAX_GROUPS              8
#define MAX_CURVES              64

// Private server group structure
struct server_group
{
    uint8_t id;                         // ID of the group
    bool    writable;                   // Whether all variables in the group
                                        // are writable
    struct
    {
        struct sllp_var_info *list[MAX_VARIABLES]; // List of variables
        uint8_t              count;                // Number of variables
    }vars;

    uint8_t size;                       // Sum of the sizes of all variables in
                                        // the group
};

static void group_init        (struct server_group *grp, uint8_t id);
static void group_add_var     (struct server_group *grp, struct sllp_var *var);
static void group_to_mod_list (sllp_server_t *server, struct server_group *grp);


struct sllp_server
{
    struct
    {
        struct sllp_var *list[MAX_VARIABLES];
        uint32_t count;
    }vars;

    struct
    {
        struct server_group list[MAX_GROUPS];
        uint32_t count;
    }groups;

    struct
    {
        struct sllp_curve *list[MAX_CURVES];
        uint32_t count;
    }curves;

    struct sllp_var *modified_list[MAX_VARIABLES+1];
    sllp_hook_t hook;
};

sllp_server_t *sllp_server_new (void)
{
    struct sllp_server *server = (struct sllp_server*) malloc(sizeof(*server));

    if(!server)
        return NULL;

    memset(server, 0, sizeof(*server));

    server->groups.count = GROUP_STANDARD_COUNT;

    group_init(&server->groups.list[GROUP_ALL_ID],   GROUP_ALL_ID);
    group_init(&server->groups.list[GROUP_READ_ID],  GROUP_ALL_ID);
    group_init(&server->groups.list[GROUP_WRITE_ID], GROUP_ALL_ID);

    server->groups.count = GROUP_STANDARD_COUNT;

    return server;
}

enum sllp_err sllp_server_destroy (sllp_server_t* server)
{
    if(!server)
        return SLLP_ERR_PARAM_INVALID;

    free(server);

    return SLLP_SUCCESS;
}

enum sllp_err sllp_register_variable (sllp_server_t *server,
                                      struct sllp_var *var)
{
    if(!server || !var)
        return SLLP_ERR_PARAM_INVALID;

    // Check variable fields
    if(var->info.size < VARIABLE_MIN_SIZE || var->info.size > VARIABLE_MAX_SIZE)
        return SLLP_ERR_PARAM_OUT_OF_RANGE;

    if(!var->data)
        return SLLP_ERR_PARAM_INVALID;

    // Check vars limit
    if(server->vars.count == MAX_VARIABLES)
        return SLLP_ERR_OUT_OF_MEMORY;

    // Check if the variable is already in the list
    unsigned int i;
    for(i = 0; i < server->vars.count; ++i)
        if(server->vars.list[i] == var)
            return SLLP_ERR_DUPLICATE;

    // Add to the variables list
    server->vars.list[server->vars.count] = var;

    // Adjust var id
    var->info.id = server->vars.count++;

    // Add to the group containing all variables
    group_add_var(&server->groups.list[GROUP_ALL_ID], var);

    // Add either to the WRITABLE or to the READ_ONLY group
    if(var->info.writable)
        group_add_var(&server->groups.list[GROUP_WRITE_ID], var);
    else
        group_add_var(&server->groups.list[GROUP_READ_ID], var);

    return SLLP_SUCCESS;
}

enum sllp_err sllp_register_curve (sllp_server_t *sllp,
                                   struct sllp_curve *curve)
{
    if(!sllp || !curve)
        return SLLP_ERR_PARAM_INVALID;

    // Check variable fields
    if(!curve->read_block)
        return SLLP_ERR_PARAM_INVALID;

    if(curve->info.writable && !curve->write_block)
        return SLLP_ERR_PARAM_INVALID;

    // Check curves limit
    if(sllp->curves.count == MAX_CURVES)
        return SLLP_ERR_OUT_OF_MEMORY;

    // Check if the curve is already in the list
    unsigned int i;
    for(i = 0; i < sllp->curves.count; ++i)
        if(sllp->curves.list[i] == curve)
            return SLLP_ERR_DUPLICATE;

    // Add to the curves list
    sllp->curves.list[sllp->curves.count] = curve;

    // Adjust curve id
    curve->info.id = sllp->curves.count++;

    return SLLP_SUCCESS;
}

enum sllp_err sllp_register_hook(sllp_server_t* sllp, sllp_hook_t hook)
{
    if(!sllp || !hook)
        return SLLP_ERR_PARAM_INVALID;

    sllp->hook = hook;

    return SLLP_SUCCESS;
}

struct raw_message
{
    uint8_t command_code;
    uint8_t encoded_size;
    uint8_t payload[];
}__attribute__((packed));

struct message
{
    enum command_code command_code;
    uint16_t payload_size;
    uint8_t *payload;
};

static void message_set_answer(struct message *msg, enum command_code code);

static void query_vars_list (sllp_server_t *server, struct message *recv_msg,
                             struct message *send_msg)
{
    // Check payload size
    if(recv_msg->payload_size != 0)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Set answer's command_code and payload_size
    message_set_answer(send_msg, CMD_VARS_LIST);

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

static void query_groups_list (sllp_server_t *server, struct message *recv_msg,
                               struct message *send_msg)
{
    // Check payload size
    if(recv_msg->payload_size != 0)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Set answer's command_code and payload_size
    message_set_answer(send_msg, CMD_GROUPS_LIST);

    // Add each group to the response
    struct server_group *grp;

    unsigned int i;
    for(i = 0; i < server->groups.count; ++i)
    {
        grp = &server->groups.list[i];
        send_msg->payload[i]  = grp->writable ? WRITABLE : READ_ONLY;
        send_msg->payload[i] += grp->vars.count;
    }
    send_msg->payload_size = server->groups.count;
}

static void query_group (sllp_server_t *server, struct message *recv_msg,
                         struct message *send_msg)
{
    // Check payload size
    if(recv_msg->payload_size != 1)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Set answer code
    message_set_answer(send_msg, CMD_GROUP);

    // Check ID
    uint8_t group_id = recv_msg->payload[0];
    if(group_id >= server->groups.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired group
    struct server_group *grp = &server->groups.list[group_id];

    unsigned int i;
    for(i = 0; i < grp->vars.count; ++i)
        send_msg->payload[i] = grp->vars.list[i]->id;

    send_msg->payload_size = grp->vars.count;
}

static void query_curves_list (sllp_server_t *server, struct message *recv_msg,
                               struct message *send_msg)
{
    // Check payload size
    if(recv_msg->payload_size != 0)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    message_set_answer(send_msg, CMD_CURVES_LIST);

    struct sllp_curve *curve;
    uint8_t *payloadp = send_msg->payload;
    unsigned int i;
    for(i = 0; i < server->curves.count; ++i)
    {
        curve = server->curves.list[i];

        (*payloadp++) = curve->info.writable;
        (*payloadp++) = curve->info.nblocks;
        memcpy(payloadp, curve->info.checksum, sizeof(curve->info.checksum));
        payloadp += sizeof(curve->info.checksum);
    }
    send_msg->payload_size = server->curves.count*CURVE_INFO_SIZE;
}

static void read_var (sllp_server_t *server, struct message *recv_msg,
                      struct message *send_msg)
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
    message_set_answer(send_msg, CMD_VAR_READING);
    send_msg->payload_size = var->info.size;
    memcpy(send_msg->payload, var->data, var->info.size);
}

static void read_group (sllp_server_t *server, struct message *recv_msg,
                        struct message *send_msg)
{
    // Check payload size
    if(recv_msg->payload_size != 1)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check group ID
    uint8_t group_id = recv_msg->payload[0];

    if(group_id >= server->groups.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired group
    struct server_group *grp = &server->groups.list[group_id];

    // Call hook
    if(server->hook)
    {
        group_to_mod_list(server, grp);
        server->hook(SLLP_OP_READ, server->modified_list);
    }

    // Iterate over group's variables
    message_set_answer(send_msg, CMD_GROUP_READING);

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

static void write_var (sllp_server_t *server, struct message *recv_msg,
                       struct message *send_msg)
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

static void bin_op_var (sllp_server_t *server, struct message *recv_msg,
                        struct message *send_msg)
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

static void write_group (sllp_server_t *server, struct message *recv_msg,
                         struct message *send_msg)
{
    // Check if body has at least one byte (ID)
    if(recv_msg->payload_size < 1)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t group_id = recv_msg->payload[0];

    if(group_id >= server->groups.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired group
    struct server_group *grp = &server->groups.list[group_id];

    // Check payload size
    if(recv_msg->payload_size != 1 + grp->size)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check write permission
    if(!grp->writable)
    {
        message_set_answer(send_msg, CMD_ERR_READ_ONLY);
        return;
    }

    // Everything is OK, iterate
    struct sllp_var *var;
    uint8_t *payloadp = recv_msg->payload + 1;
    unsigned int i;

    for(i = 0; i < grp->vars.count; ++i)
    {
        var = server->vars.list[grp->vars.list[i]->id];
        memcpy(var->data, payloadp, var->info.size);
        payloadp += var->info.size;
    }

    // Call hook
    if(server->hook)
    {
        group_to_mod_list(server, grp);
        server->hook(SLLP_OP_WRITE, server->modified_list);
    }

    message_set_answer(send_msg, CMD_OK);
}

static void bin_op_group (sllp_server_t *server, struct message *recv_msg,
                          struct message *send_msg)
{
    // Check if body has at least two bytes (ID + binary operation)
    if(recv_msg->payload_size < 2)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check ID
    uint8_t group_id = recv_msg->payload[0];

    if(group_id >= server->groups.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get desired group
    struct server_group *grp = &server->groups.list[group_id];

    // Get operation
    unsigned char operation = recv_msg->payload[1];

    // Check operation
    if(!bin_op[operation])
    {
        message_set_answer(send_msg, CMD_ERR_OP_NOT_SUPPORTED);
        return;
    }

    // Check payload size
    if(recv_msg->payload_size != 2 + grp->size)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check write permission
    if(!grp->writable)
    {
        message_set_answer(send_msg, CMD_ERR_READ_ONLY);
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

    message_set_answer(send_msg, CMD_OK);
}

static void create_group (sllp_server_t *server, struct message *recv_msg,
                          struct message *send_msg)
{
    // Check if there's at least one variable to put on the group
    if(recv_msg->payload_size < 1 ||
       recv_msg->payload_size > server->vars.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check if there's available space for the new group
    if(server->groups.count == MAX_GROUPS)
    {
        message_set_answer(send_msg, CMD_ERR_INSUFFICIENT_MEMORY);
        return;
    }

    struct server_group *grp = &server->groups.list[server->groups.count];

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
            message_set_answer(send_msg, CMD_ERR_INVALID_ID);
            return;
        }

        // Add var by ID
        group_add_var(grp, server->vars.list[var_id]);
    }

    // Group created
    ++server->groups.count;

    // Prepare answer
    message_set_answer(send_msg, CMD_OK);
    return;
}

static void remove_groups (sllp_server_t *server, struct message *recv_msg,
                           struct message *send_msg)
{
    if(recv_msg->payload_size != 0)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    server->groups.count = GROUP_STANDARD_COUNT;
    message_set_answer(send_msg, CMD_OK);
}

static void request_curve_block (sllp_server_t *server,
                                 struct message *recv_msg,
                                 struct message *send_msg)
{
    // Check payload size
    if(recv_msg->payload_size != 2)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check curve ID
    uint8_t curve_id = recv_msg->payload[0];

    if(curve_id >= server->curves.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get curve
    struct sllp_curve *curve = server->curves.list[curve_id];

    uint8_t block_offset = recv_msg->payload[1];

    if(block_offset > curve->info.nblocks)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_VALUE);
        return;
    }

    message_set_answer(send_msg, CMD_CURVE_BLOCK);
    send_msg->payload[0] = curve->info.id;
    send_msg->payload[1] = block_offset;

    curve->read_block(curve, block_offset, send_msg->payload + 2);
    send_msg->payload_size = 2 + CURVE_BLOCK;
}

static void curve_block (sllp_server_t *server, struct message *recv_msg,
                         struct message *send_msg)
{
    if(recv_msg->payload_size != 2 + CURVE_BLOCK)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check curve ID
    uint8_t curve_id = recv_msg->payload[0];

    if(curve_id >= server->curves.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get curve
    struct sllp_curve *curve = server->curves.list[curve_id];

    uint8_t block_offset = recv_msg->payload[1];
    if(block_offset > curve->info.nblocks)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_VALUE);
        return;
    }
    curve->write_block(curve, block_offset, recv_msg->payload + 2);
    message_set_answer(send_msg, CMD_OK);
}

static void recalc_curve_csum (sllp_server_t *server, struct message *recv_msg,
                               struct message *send_msg)
{
    if(recv_msg->payload_size != 1)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check curve ID
    uint8_t curve_id = recv_msg->payload[0];

    if(curve_id >= server->curves.count)
    {
        message_set_answer(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get curve
    struct sllp_curve *curve = server->curves.list[curve_id];

    // Calculate checksum (this might take a while)
    unsigned int nblocks = curve->info.nblocks + 1;
    uint8_t block[CURVE_BLOCK];
    MD5_CTX md5ctx;

    MD5Init(&md5ctx);

    unsigned int i;
    for(i = 0; i < nblocks; ++i)
    {
        curve->read_block(curve, (uint8_t)i, block);
        MD5Update(&md5ctx, block, CURVE_BLOCK);
    }
    MD5Final(curve->info.checksum, &md5ctx);

    message_set_answer(send_msg, CMD_OK);
}

static void message_set_answer (struct message *msg, enum command_code code)
{
    msg->command_code = code;
    msg->payload_size = 0;
}


// Process packet infrastructure

typedef void (*command_function) (sllp_server_t *server,
                                  struct message *recv_msg,
                                  struct message *send_msg);

static command_function command[256] = {
    [CMD_QUERY_VARS_LIST]       = query_vars_list,
    [CMD_QUERY_GROUPS_LIST]     = query_groups_list,
    [CMD_QUERY_GROUP]           = query_group,
    [CMD_QUERY_CURVES_LIST]     = query_curves_list,
    [CMD_READ_VAR]              = read_var,
    [CMD_READ_GROUP]            = read_group,
    [CMD_WRITE_VAR]             = write_var,
    [CMD_WRITE_GROUP]           = write_group,
    [CMD_BIN_OP_VAR]            = bin_op_var,
    [CMD_BIN_OP_GROUP]          = bin_op_group,
    [CMD_CREATE_GROUP]          = create_group,
    [CMD_REMOVE_ALL_GROUPS]     = remove_groups,
    [CMD_CURVE_TRANSMIT]        = request_curve_block,
    [CMD_CURVE_BLOCK]           = curve_block,
    [CMD_CURVE_RECALC_CSUM]     = recalc_curve_csum
};

enum sllp_err sllp_process_packet (sllp_server_t *server,
                                    struct sllp_raw_packet *request,
                                    struct sllp_raw_packet *response)
{
    if(!server || !request || !response)
        return SLLP_ERR_PARAM_INVALID;

    // Interpret packet payload as a message
    struct raw_message *recv_raw_msg = (struct raw_message *) request->data;
    struct raw_message *send_raw_msg = (struct raw_message *) response->data;

    // Create proper messages from the raw messages
    struct message recv_msg, send_msg;

    recv_msg.command_code = (enum command_code) recv_raw_msg->command_code;
    recv_msg.payload      = recv_raw_msg->payload;

    // Decode received payload size
    if(recv_raw_msg->encoded_size == MAX_PAYLOAD_ENCODED)
        recv_msg.payload_size = MAX_PAYLOAD;
    else
        recv_msg.payload_size = recv_raw_msg->encoded_size;

    send_msg.payload = send_raw_msg->payload;

    server->modified_list[0] = NULL;

    // Check inconsistency between the size of the received data and the size
    // specified in the message header
    if(request->len < SLLP_HEADER_SIZE ||
       request->len != recv_msg.payload_size + SLLP_HEADER_SIZE)
        message_set_answer(&send_msg, CMD_ERR_MALFORMED_MESSAGE);

    // Check existence of the requested command
    else if(!command[recv_msg.command_code])
        message_set_answer(&send_msg, CMD_ERR_OP_NOT_SUPPORTED);
    else
        command[recv_msg.command_code](server, &recv_msg, &send_msg);

    send_raw_msg->command_code = send_msg.command_code;

    if(send_msg.payload_size == MAX_PAYLOAD)
        send_raw_msg->encoded_size = MAX_PAYLOAD_ENCODED;
    else
        send_raw_msg->encoded_size = send_msg.payload_size;
    response->len = send_msg.payload_size + 2;

    return SLLP_SUCCESS;
}

static void group_init (struct server_group *grp, uint8_t id)
{
    memset(grp, 0, sizeof(*grp));
    grp->id = id;
    grp->writable = true;
}

static void group_add_var (struct server_group *grp, struct sllp_var *var)
{
    grp->vars.list[grp->vars.count++] = &var->info;
    grp->size += var->info.size;
    grp->writable &= var->info.writable;
}

static void group_to_mod_list (sllp_server_t *server, struct server_group *grp)
{
    unsigned int i;
    for(i = 0; i < grp->vars.count; ++i)
        server->modified_list[i] = server->vars.list[grp->vars.list[i]->id];
    server->modified_list[i] = NULL;
}
