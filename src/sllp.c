#include "sllp.h"

static char* SLLP_PROTO_VERSION = "0.9";

static char* error_str[SLLP_ERR_MAX] =
{
    [SLLP_SUCCESS]                  = "Success",
    [SLLP_ERR_PARAM_INVALID]        = "An invalid parameter was passed",
    [SLLP_ERR_PARAM_OUT_OF_RANGE]   = "A parameter was out of the acceptable"
                                      " range",
    [SLLP_ERR_OUT_OF_MEMORY]        = "Not enough memory to complete request",
    [SLLP_ERR_DUPLICATE]            = "Object already registered",
    [SLLP_ERR_COMM]                 = "Sending or receiving a message failed"
};

char *sllp_error_str (enum sllp_err error)
{
    return error_str[error];
}

char *sllp_proto_version(void)
{
    return SLLP_PROTO_VERSION;
}
