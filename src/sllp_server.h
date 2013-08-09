#ifndef SLLP_SERVER_H
#define SLLP_SERVER_H

#include "sllp.h"

// Types

// Handle to a server instance
typedef struct sllp_server sllp_server_t;

// Hook function. Called before the values of a set of variables are read and
// after values of a set of variables are written.
enum sllp_operation
{
    SLLP_OP_READ,                   // Read command arrived
    SLLP_OP_WRITE,                  // Write command arrived
};

typedef void (*sllp_hook_t) (enum sllp_operation op, struct sllp_var **list);

// Structures

// Represent a packet that either was received or is to be sent
struct sllp_raw_packet
{
    uint8_t *data;
    uint16_t len;
};

/**
 * Allocate a new server instance, returning a handle to it. This instance
 * should be deallocated with sllp_destroy after its use.
 *
 * @return A handle to a server or NULL if there wasn't enough memory to do the
 *         allocation.
 */
sllp_server_t *sllp_server_new (void);

/**
 * Deallocate a server instance
 * 
 * @param server [input] Handle to the instance to be deallocated.
 * 
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: server is a NULL pointer. </li>
 * </ul>
 */
enum sllp_err sllp_server_destroy (sllp_server_t *server);

/**
 * Register a variable with a server instance. The memory pointed by the var
 * parameter must remain valid throughout the entire lifespan of the server
 * instance. The id field of the var parameter will be written by the SLLP lib.
 *
 * The fields writable, size and data of the var parameter must be filled
 * correctly.
 *
 * The user field is untouched.
 *
 * @param server [input] Handle to the instance.
 * @param var [input] Structure describing the variable to be registered.
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li> SLLP_ERR_PARAM_INVALID: server, data or id is a NULL pointer. </li>
 *   <li> SLLP_PARAM_OUT_OF_RANGE: size is less than SLLP_VAR_SIZE_MIN or
 *                                 greater than SLLP_VAR_SIZE_MAX. </li>
 * </ul>
 */
enum sllp_err sllp_register_variable (sllp_server_t *server,
                                      struct sllp_var *var);

/**
 * Register a curve with a SLLP instance. The memory pointed by the curve
 * parameter must remain valid throughout the entire lifespan of the server
 * instance. The id field of the curve parameter will be written by the SLLP
 * lib.
 *
 * The fields writable, nblocks and read_block must be filled correctly.
 * If writable is true, the field write_block must also be filled correctly.
 * Otherwise, write_block must be NULL.
 *
 * NOTE: The field nblocks contains the number of blocks of the curve, minus 1.
 * So, if the curve has 8 blocks, for example, nblocks = 7.
 *
 * The user field is untouched.
 *
 * @param server [input] Handle to the server instance.
 * @param curve [input] Poiner to the curve to be registered.
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: either server or curve is a NULL pointer.</li>
 *   <li>SLLP_ERR_PARAM_INVALID: curve->read_block is NULL.</li>
 *   <li>SLLP_ERR_PARAM_INVALID: curve->writable is true and curve->write_block
 *                               is NULL.</li>
 *   <li>SLLP_ERR_PARAM_INVALID: curve->writable is false and curve->write_block
 *                               is not NULL.</li>
 * </ul>
 */
enum sllp_err sllp_register_curve (sllp_server_t *server,
                                   struct sllp_curve *curve);

/**
 * Register a function that will be called in two moments:
 *
 * 1. When a command that reads one or more variables arrives, the hook function
 * is called right BEFORE reading the variables and putting the read values in
 * the response message.
 *
 * 2. When a command that writes to one or more variables arrives, the hook
 * function is called AFTER writing the values of the variables.
 *
 * Other commands don't call the hook function. It's not required to register a
 * hook function. It's possible to deregister a previously registered hook
 * function by simply passing a NULL pointer in the hook parameter.
 *
 * A hook function receives two parameters: the first one indicating the type
 * of operation being performed (specified in enum sllp_operation) and the
 * second one containing the list of variables being affected by that operation.
 *
 * @param server [input] Handle to a SLLP instance.
 * @param hook [input] Hook function
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li> SLLP_ERR_INVALID_PARAM: sllp is a NULL pointer.
 * </ul>
 */
enum sllp_err sllp_register_hook (sllp_server_t *server, sllp_hook_t hook);

/**
 * Process a received message and prepare an answer.
 *
 * @param server [input] Handle to a server instance.
 * @param request [input] The message to be processed.
 * @param response [output] The answer to be sent
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li> SSLP_ERR_PARAM_INVALID: Either server, request, or response is
 *                                a NULL pointer.</li>
 * </ul>
 */
enum sllp_err sllp_process_packet (sllp_server_t *server,
                                   struct sllp_raw_packet *request,
                                   struct sllp_raw_packet *response);

#endif

