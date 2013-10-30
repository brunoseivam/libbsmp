#include "curve.h"
#include "server_defs.h"
#include "sllp_server.h"
#include "md5/md5.h"

#include <stdlib.h>
#include <string.h>

enum sllp_err curve_check(struct sllp_curve *curve)
{
    if(!curve)
        return SLLP_ERR_PARAM_INVALID;

    if(!curve->read_block)
        return SLLP_ERR_PARAM_INVALID;

    if(curve->info.writable && !curve->write_block)
        return SLLP_ERR_PARAM_INVALID;

    return SLLP_SUCCESS;
}

SERVER_CMD_FUNCTION (curve_query_list)
{
    // Check payload size
    if(recv_msg->payload_size != 0)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    MESSSAGE_SET_ANSWER(send_msg, CMD_CURVE_LIST);

    struct sllp_curve_info *curve;
    unsigned int i;
    for(i = 0; i < server->curves.count; ++i)
    {
        curve = &server->curves.list[i]->info;

        send_msg->payload[i*CURVE_INFO_SIZE]     = curve->writable;
        send_msg->payload[i*CURVE_INFO_SIZE + 1] = curve->nblocks >> 8;
        send_msg->payload[i*CURVE_INFO_SIZE + 2] = curve->nblocks & 0xFF;
    }

    send_msg->payload_size = server->curves.count*CURVE_INFO_SIZE;
}

SERVER_CMD_FUNCTION (curve_query_csum)
{
    // Check payload size
    if(recv_msg->payload_size != 1)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    uint8_t curve_id = recv_msg->payload[0];

    if(curve_id >= server->curves.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    struct sllp_curve *curve = server->curves.list[curve_id];

    MESSSAGE_SET_ANSWER(send_msg, CMD_CURVE_CSUM);
    memcpy(send_msg->payload, curve->info.checksum, CURVE_CSUM_SIZE);
    send_msg->payload_size = CURVE_CSUM_SIZE;
}

SERVER_CMD_FUNCTION (curve_block_request)
{
    // Check payload size
    if(recv_msg->payload_size != CURVE_INFO_SIZE)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check curve ID
    uint8_t curve_id = recv_msg->payload[0];

    if(curve_id >= server->curves.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get curve
    struct sllp_curve *curve = server->curves.list[curve_id];

    uint16_t block_offset = (recv_msg->payload[1] << 8) + recv_msg->payload[2];

    if(block_offset > curve->info.nblocks)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_VALUE);
        return;
    }

    MESSSAGE_SET_ANSWER(send_msg, CMD_CURVE_BLOCK);
    send_msg->payload[0] = recv_msg->payload[0];    // Curve ID
    send_msg->payload[1] = recv_msg->payload[1];    // Offset (most sig.)
    send_msg->payload[2] = recv_msg->payload[2];    // Offset (less sig.)

    curve->read_block(curve, block_offset, send_msg->payload + CURVE_INFO_SIZE);
    send_msg->payload_size = CURVE_PKT_SIZE;
}

SERVER_CMD_FUNCTION (curve_block)
{
    if(recv_msg->payload_size != CURVE_PKT_SIZE)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check curve ID
    uint8_t curve_id = recv_msg->payload[0];

    if(curve_id >= server->curves.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get curve
    struct sllp_curve *curve = server->curves.list[curve_id];

    uint16_t block_offset = (recv_msg->payload[1] << 8) + recv_msg->payload[2];
    if(block_offset > curve->info.nblocks)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_VALUE);
        return;
    }
    curve->write_block(curve, block_offset, recv_msg->payload+CURVE_INFO_SIZE);
    MESSSAGE_SET_ANSWER(send_msg, CMD_OK);
}

SERVER_CMD_FUNCTION (curve_recalc_csum)
{
    if(recv_msg->payload_size != 1)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_PAYLOAD_SIZE);
        return;
    }

    // Check curve ID
    uint8_t curve_id = recv_msg->payload[0];

    if(curve_id >= server->curves.count)
    {
        MESSSAGE_SET_ANSWER(send_msg, CMD_ERR_INVALID_ID);
        return;
    }

    // Get curve
    struct sllp_curve *curve = server->curves.list[curve_id];

    // Calculate checksum (this might take a while)
    unsigned int nblocks = curve->info.nblocks + 1;
    uint8_t block[CURVE_BLOCK_SIZE];
    MD5_CTX md5ctx;

    MD5Init(&md5ctx);

    unsigned int i;
    for(i = 0; i < nblocks; ++i)
    {
        curve->read_block(curve, (uint16_t)i, block);
        MD5Update(&md5ctx, block, CURVE_BLOCK_SIZE);
    }
    MD5Final(curve->info.checksum, &md5ctx);

    MESSSAGE_SET_ANSWER(send_msg, CMD_CURVE_CSUM);
    memcpy(send_msg->payload, curve->info.checksum, CURVE_CSUM_SIZE);
    send_msg->payload_size = CURVE_CSUM_SIZE;
}
