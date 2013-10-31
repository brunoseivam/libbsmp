#include "sllp.h"

static char* SLLP_PROTO_VERSION = "1.00";

static char* error_str[SLLP_ERR_MAX] =
{
    [SLLP_SUCCESS]                  = "Success",
    [SLLP_ERR_PARAM_INVALID]        = "An invalid parameter was passed",
    [SLLP_ERR_PARAM_OUT_OF_RANGE]   = "A parameter was out of the acceptable"
                                      " range",
    [SLLP_ERR_OUT_OF_MEMORY]        = "Not enough memory to complete request",
    [SLLP_ERR_DUPLICATE]            = "Entity already registered",
    [SLLP_ERR_COMM]                 = "Sending or receiving a message failed"
};

#define BINOPS_FUNC(name, operation)\
    void binops_##name (uint8_t *data, uint8_t *mask, uint8_t size) {\
        while(size--)\
            data[size] operation mask[size];\
    }

BINOPS_FUNC(and, &=)
BINOPS_FUNC(or, |=)
BINOPS_FUNC(xor, ^=)
BINOPS_FUNC(clear, &=~)

bin_op_function bin_op[256] =
{
    ['A'] = binops_and,    // AND
    ['X'] = binops_xor,    // XOR
    ['O'] = binops_or,     // OR
    ['C'] = binops_clear,  // CLEAR BITS
    ['S'] = binops_or,     // SET BITS
    ['T'] = binops_xor     // TOGGLE BITS
};

char *sllp_error_str (enum sllp_err error)
{
    return error_str[error];
}

char *sllp_proto_version(void)
{
    return SLLP_PROTO_VERSION;
}
