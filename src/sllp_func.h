#ifndef SLLP_FUNC_H
#define SLLP_FUNC_H

#include "sllp.h"
#include <stdint.h>

#define SLLP_FUNC_MAX_INPUT     15
#define SLLP_FUNC_MAX_OUTPUT    15

struct sllp_func_info
{
    uint8_t id;                     // ID of the function, used in the protocol
    uint8_t input_size;             // How many bytes of input
    uint8_t output_size;            // How many bytes of output
};

typedef uint8_t (*sllp_func_t) (uint8_t *input, uint8_t *output);
struct sllp_func
{
    struct sllp_func_info info;     // Information about the function
    sllp_func_t           func_p;   // Pointer to the function to be executed
};

SLLP_LIST_STRUCT (sllp_func_info_list,
                  struct sllp_func_info,
                  SLLP_MAX_FUNCTIONS);

SLLP_LIST_STRUCT (sllp_func_ptr_list,
                  struct sllp_func*,
                  SLLP_MAX_FUNCTIONS);

#endif
