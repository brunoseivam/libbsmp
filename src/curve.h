#ifndef CURVE_H
#define CURVE_H

#include "server_defs.h"
#include "sllp_curve.h"

#define CURVE_INFO_SIZE         SLLP_CURVE_BLOCK_INFO
#define CURVE_BLOCK_SIZE        SLLP_CURVE_BLOCK_SIZE
#define CURVE_PKT_SIZE          SLLP_CURVE_BLOCK_PKT
#define CURVE_CSUM_SIZE         16

enum curve_cmd
{
    CMD_CURVE_QUERY_LIST    = 0x08,
    CMD_CURVE_LIST          = 0x09,
    CMD_CURVE_QUERY_CSUM    = 0x0A,
    CMD_CURVE_CSUM          = 0x0B,
    CMD_CURVE_BLOCK_REQUEST = 0x40,
    CMD_CURVE_BLOCK         = 0x41,
    CMD_CURVE_RECALC_CSUM   = 0x42,
};

enum sllp_err curve_check (struct sllp_curve *curve);

SERVER_CMD_FUNCTION (curve_query_list);
SERVER_CMD_FUNCTION (curve_query_csum);
SERVER_CMD_FUNCTION (curve_block_request);
SERVER_CMD_FUNCTION (curve_block);
SERVER_CMD_FUNCTION (curve_recalc_csum);

#define CURVE_CMD_POINTERS \
        [CMD_CURVE_QUERY_LIST]      = curve_query_list,\
        [CMD_CURVE_QUERY_CSUM]      = curve_query_csum,\
        [CMD_CURVE_BLOCK_REQUEST]   = curve_block_request,\
        [CMD_CURVE_BLOCK]           = curve_block,\
        [CMD_CURVE_RECALC_CSUM]     = curve_recalc_csum

#endif
