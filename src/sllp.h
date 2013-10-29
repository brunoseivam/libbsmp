#ifndef SLLP_H
#define SLLP_H

#include <stdint.h>
#include <stdbool.h>

#define SLLP_HEADER_SIZE        2
#define SLLP_MAX_PAYLOAD        SLLP_CURVE_BLOCK_PKT
#define SLLP_MAX_MESSAGE        (SLLP_HEADER_SIZE+SLLP_MAX_PAYLOAD)

#define SLLP_MAX_VARIABLES      128
#define SLLP_MAX_GROUPS         8
#define SLLP_MAX_CURVES         128
#define SLLP_MAX_FUNCTIONS      128

#define SLLP_LIST_STRUCT(name, type, max)\
    struct name {\
        uint32_t count;\
        type list[max];\
    }


enum sllp_err
{
    SLLP_SUCCESS,                   // Operation executed successfully
    SLLP_ERR_PARAM_INVALID,         // An invalid parameter was passed
    SLLP_ERR_PARAM_OUT_OF_RANGE,    // A param not in the acceptable range was
                                    // passed
    SLLP_ERR_OUT_OF_MEMORY,         // Not enough memory to complete operation
    SLLP_ERR_DUPLICATE,             // Trying to register an already registered
                                    // object
    SLLP_ERR_COMM,                  // There was a communication error reported
                                    // by one of the communication functions.

    SLLP_ERR_MAX
};

/**
 * Returns an error string associated with the given error code
 *
 * @param error [input] The error code
 *
 * @return A string that describes the error
 */
char * sllp_error_str (enum sllp_err error);

/**
 * Returns a string that represents the SLLP version number supported by
 * this library version
 *
 * @return String containing the version supported.
 */
char *sllp_proto_version (void);

#endif
