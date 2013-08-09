#ifndef SLLP_H
#define SLLP_H

#include <stdint.h>
#include <stdbool.h>

#define SLLP_HEADER_SIZE        2
#define SLLP_CURVE_BLOCK_SIZE   16384
#define SLLP_MAX_PAYLOAD        (SLLP_CURVE_BLOCK_SIZE+2)
#define SLLP_MAX_MESSAGE        (SLLP_HEADER_SIZE+SLLP_MAX_PAYLOAD)

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

struct sllp_var_info
{
    uint8_t id;                     // ID of the variable, used in the protocol.
    bool    writable;               // Determine if the variable is writable.
    uint8_t size;                   // Indicates how many bytes 'data' contains.
};

struct sllp_curve_info
{
    uint8_t id;                     // ID of the curve, used in the protocol.
    bool    writable;               // Determine if the curve is writable.
    uint8_t nblocks;                // How many 16kB blocks the curve contains.
    uint8_t checksum[16];           // MD5 checksum of the curve
};

struct sllp_var
{
    struct sllp_var_info info;  // Information about the variable identification
    uint8_t              *data; // Pointer to the value of the variable.
    void                 *user; // The user can make use of this variable at
                                // will. It is not touched by SLLP.
};

struct sllp_curve
{
    struct sllp_curve_info info;   // Information about the curve identification

    // Read a SLLP_CURVE_BLOCK_SIZE bytes block into data
    void (*read_block) (struct sllp_curve *curve, uint8_t block, uint8_t *data);

    // Write a SLLP_CURVE_BLOCK_SIZE bytes block from data
    void (*write_block)(struct sllp_curve *curve, uint8_t block, uint8_t *data);

    void    *user;                 // The user can make use of this variable as
                                   // he wishes. It is not touched by SLLP.
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
