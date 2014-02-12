/* Public definitions for both server and client */

#ifndef SLLP_H
#define SLLP_H

#include <stdint.h>
#include <stdbool.h>

/* Library-wide limits */

#define SLLP_HEADER_SIZE            3       // Command code + 2 bytes for size
#define SLLP_MAX_PAYLOAD            65535
#define SLLP_MAX_MESSAGE            (SLLP_HEADER_SIZE+SLLP_MAX_PAYLOAD)

#define SLLP_MAX_VARIABLES          128
#define SLLP_MAX_GROUPS             8
#define SLLP_MAX_CURVES             128
#define SLLP_MAX_FUNCTIONS          128

/* Version info */

#define SLLP_VERSION_STR_MAX_LEN    20
struct sllp_version
{
    uint8_t major;
    uint8_t minor;
    uint8_t revision;
    char    str[SLLP_VERSION_STR_MAX_LEN];
};

/* Standard group ID */

enum group_id
{
    GROUP_ALL_ID,
    GROUP_READ_ID,
    GROUP_WRITE_ID,

    GROUP_STANDARD_COUNT,
};

/* Binary operations */

enum sllp_bin_op
{
    BIN_OP_AND,
    BIN_OP_OR,
    BIN_OP_XOR,
    BIN_OP_SET,
    BIN_OP_CLEAR,
    BIN_OP_TOGGLE,

    BIN_OP_COUNT,   // Number of binary operations
};

typedef void (*bin_op_function) (uint8_t *data, uint8_t *mask, uint8_t size);
extern bin_op_function bin_op[256];

/* Error codes */

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

/**** Entities ****/

/* Limits */

#define SLLP_VAR_MAX_SIZE           128

#define SLLP_CURVE_MAX_BLOCKS       65536
#define SLLP_CURVE_BLOCK_MAX_SIZE   65520
#define SLLP_CURVE_LIST_INFO        5
#define SLLP_CURVE_BLOCK_INFO       3
#define SLLP_CURVE_CSUM_SIZE        16

#define SLLP_FUNC_MAX_INPUT         15
#define SLLP_FUNC_MAX_OUTPUT        15

/**** Structures and lists ****/

/* Variable */

struct sllp_var_info
{
    uint8_t id;                 // ID of the variable, used in the protocol.
    bool    writable;           // Determine if the variable is writable.
    uint8_t size;               // Indicates how many bytes 'data' contains.
};

struct sllp_var
{
    struct sllp_var_info info;  // Information about the variable identification
    bool                 (*value_ok) (struct sllp_var *, uint8_t *);  // Checker
    uint8_t              *data; // Pointer to the value of the variable.
    void                 *user; // The user can make use of this pointer at
                                // will. It is not touched by SLLP.
};

struct sllp_var_info_list
{
    uint32_t count;
    struct sllp_var_info list[SLLP_MAX_VARIABLES];
};

struct sllp_var_info_ptr_list
{
    uint32_t count;
    struct sllp_var_info *list[SLLP_MAX_VARIABLES];
};

struct sllp_var_ptr_list
{
    uint32_t count;
    struct sllp_var *list[SLLP_MAX_VARIABLES];
};

/* Group */

struct sllp_group
{
    uint8_t id;           // ID of the group
    bool    writable;     // Whether all variables in the group are writable
    uint16_t size;        // Sum of the sizes of all variables in the group

    // List of pointers to the variables of this group
    struct sllp_var_info_ptr_list vars;
};

struct sllp_group_list
{
    uint32_t count;
    struct sllp_group list[SLLP_MAX_GROUPS];
};

/* Curve */

struct sllp_curve_info
{
    uint8_t  id;                    // ID of the curve, used in the protocol.
    bool     writable;              // Determine if the curve is writable.
    uint32_t nblocks;               // How many blocks the curve contains.
    uint16_t block_size;            // Maximum number of bytes in a block
    uint8_t  checksum[16];          // MD5 checksum of the curve
};

struct sllp_curve;

typedef void (*sllp_curve_read_t)  (struct sllp_curve *curve, uint16_t block,
                                    uint8_t *data, uint16_t *len);
typedef void (*sllp_curve_write_t) (struct sllp_curve *curve, uint16_t block,
                                    uint8_t *data, uint16_t len);
struct sllp_curve
{
    // Info about the curve identification
    struct sllp_curve_info info;

    // Functions to read/write a block
    void (*read_block)(struct sllp_curve *curve, uint16_t block, uint8_t *data,
                       uint16_t *len);

    void (*write_block)(struct sllp_curve *curve, uint16_t block, uint8_t *data,
                        uint16_t len);

    // The user can make use of this variable as he wishes. It is not touched by
    // SLLP
    void *user;
};

struct sllp_curve_info_list
{
    uint32_t count;
    struct sllp_curve_info list[SLLP_MAX_CURVES];
};

struct sllp_curve_ptr_list
{
    uint32_t count;
    struct sllp_curve *list[SLLP_MAX_CURVES];
};

/* Function */

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


struct sllp_func_info_list
{
    uint32_t count;
    struct sllp_func_info list[SLLP_MAX_FUNCTIONS];
};

struct sllp_func_ptr_list
{
    uint32_t count;
    struct sllp_func *list[SLLP_MAX_FUNCTIONS];
};

/**
 * Returns an error string associated with the given error code
 *
 * @param error [input] The error code
 *
 * @return A string that describes the error
 */
char * sllp_error_str (enum sllp_err error);

#endif
