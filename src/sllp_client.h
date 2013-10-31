#ifndef SLLP_CLIENT_H
#define SLLP_CLIENT_H

#include "sllp.h"
#include "sllp_var.h"
#include "sllp_group.h"
#include "sllp_curve.h"
#include "sllp_func.h"

// Types

// Handle to a client instance
typedef struct sllp_client sllp_client_t;

// Communication function (send and receive data). Must return 0 if successful
// and anything but 0 otherwise.
typedef int (*sllp_comm_func_t) (uint8_t* data, uint32_t *count);

/**
 * Allocate a new SLLP Client instance, returning a handle to it. This instance
 * should be deallocated with sllp_client_destroy after its use.
 *
 * @param send_func [input] Function used to send a message
 * @param recv_func [input] Function used to receive a message
 *
 * @return A handle to an instance of the SLLP Client lib or NULL if there
 *         wasn't enough memory to do the allocation.
 */
sllp_client_t *sllp_client_new (sllp_comm_func_t send_func,
                                sllp_comm_func_t recv_func);

/**
 * Deallocate a SLLP Client instance
 * 
 * @param client [input] A SLLP Client Library instance to be deallocated.
 * 
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client is a NULL pointer. </li>
 * </ul>
 */
enum sllp_err sllp_client_destroy (sllp_client_t *client);

/*
 * Initializes an instance of the SLLP Client Library. Initialization means
 * that information about the server will be queried (list of variables, list
 * of groups, list of curves) and stored in the instance.
 *
 * The instance MUST be initialized only once.
 *
 * The communications functions (send_func and recv_func, passed to the
 * sllp_client_new function) MUST be able to communicate before sllp_client_init
 * invocation.
 *
 * @param client [input] Handle to the instance to be initialized
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client is a NULL pointer or it was already
 *                               initialized</li>
 *   <li>SLLP_ERR_COMM: There was a communication error during the
 *                      initialization of the sllp instance</li>
 *   <li>SLLP_ERR_OUT_OF_MEMORY: Not enough memory to complete the
 *                               initialization of the sllp instance</li>
 * </ul>
 *
 */
enum sllp_err sllp_client_init(sllp_client_t *client);

/*
 * Returns the list of variables provided by a server in the list parameter.
 *
 * The sllp instance MUST be previously initialized. Otherwise an empty list
 * will be returned.
 *
 * @param client [input] A SLLP Client Library instance
 * @param list [output] Address of the variables list pointer
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: either client or list is a NULL pointer</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 *
 */
enum sllp_err sllp_get_vars_list (sllp_client_t *client,
                                  struct sllp_var_info_list **list);

/*
 * Returns the list of groups provided by a server in the list parameter.
 *
 * The sllp instance MUST be previously initialized. Otherwise an empty list
 * will be returned.
 *
 * @param client [input] A SLLP Client Library instance
 * @param list [output] Address of the groups list pointer
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: either client or list is a NULL pointer</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 *
 */
enum sllp_err sllp_get_groups_list (sllp_client_t *client,
                                    struct sllp_group_list **list);

/*
 * Returns the list of curves provided by a server in the list parameter.
 *
 * The sllp instance MUST be previously initialized. Otherwise an empty list
 * will be returned.
 *
 * @param sllp [input] A SLLP Client Library instance
 * @param list [output] Address of the curves list pointer
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: either sllp or list is a NULL pointer</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 *
 */
enum sllp_err sllp_get_curves_list (sllp_client_t *client,
                                    struct sllp_curve_info_list **list);

/*
 * Returns the list of Fcuntions, provided by a server, in the list parameter.
 *
 * The sllp instance MUST be previously initialized. Otherwise an empty list
 * will be returned.
 *
 * @param sllp [input] A SLLP Client Library instance
 * @param list [output] Address of the Functions list pointer
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: either sllp or list is a NULL pointer</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 *
 */
enum sllp_err sllp_get_funcs_list (sllp_client_t *client,
                                   struct sllp_func_info_list **list);

/*
 * Reads the value of a variable into a caller provided buffer.
 *
 * The values buffer MUST be able to hold, at least, var->size bytes.
 *
 * @param client [input] A SLLP Client Library instance
 * @param var [input] The variable to be read
 * @param value [output] Pointer to a buffer to contain the read values
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client, var or value is a NULL pointer</li>
 *   <li>SLLP_ERR_PARAM_INVALID: var is not a valid server variable</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_read_var (sllp_client_t *client, struct sllp_var_info *var,
                             uint8_t *value);

/*
 * Writes values to a variable from a caller provided buffer.
 *
 * The values buffer MUST contain, at least, var->size bytes.
 *
 * @param client [input] A SLLP Client Library instance
 * @param var [input] The variable to be written
 * @param value [input] Pointer to a buffer containing the values to be written
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client, var or value is a NULL pointer</li>
 *   <li>SLLP_ERR_PARAM_INVALID: var is not a valid server variable</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_write_var (sllp_client_t *client, struct sllp_var_info *var,
                              uint8_t *value);

/*
 * Reads the values of a group of variables into a caller provided buffer.
 *
 * The values buffer MUST be able to hold, at least, group->size bytes.
 *
 * @param client [input] A SLLP Client Library instance
 * @param var [input] The variable to be read
 * @param value [output] Pointer to a buffer to contain the read values
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client, grp or value is a NULL pointer</li>
 *   <li>SLLP_ERR_PARAM_INVALID: grp is not a valid server group</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_read_group (sllp_client_t *client, struct sllp_group *grp,
                               uint8_t *values);

/*
 * Writes values to variables in a group from a caller provided buffer.
 *
 * The values buffer MUST contain, at least, grp->size bytes.
 *
 * @param client [input] A SLLP Client Library instance
 * @param grp [input] The group to be written
 * @param value [input] Pointer to a buffer containing the values to be written
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client, grp or value is a NULL pointer</li>
 *   <li>SLLP_ERR_PARAM_INVALID: grp is not a valid server group</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_write_group (sllp_client_t *client, struct sllp_group *grp,
                                uint8_t *values);

/*
 * Perform a binary operation in a variable with the bits specified by the mask.
 *
 * The mask MUST contain, at least, var->size bytes.
 *
 * @param client [input] A SLLP Client Library instance
 * @param var [input] The variable to perform the operation
 * @param value [input] Pointer to a buffer containing the values to be written
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client, var or value is a NULL pointer</li>
 *   <li>SLLP_ERR_PARAM_INVALID: var is not a valid server variable</li>
 *   <li>SLLP_ERR_PARAM_INVALID: op isn't one of the supported operations</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_bin_op_var (sllp_client_t *client, enum sllp_bin_op op,
                               struct sllp_var_info *var, uint8_t *mask);

/*
 * Perform a binary operation in a group of variables with the bits
 * specified by the mask.
 *
 * The mask MUST contain, at least, grp->size bytes.
 *
 * @param client [input] A SLLP Client Library instance
 * @param var [input] The group to perform the operation
 * @param value [input] Pointer to a buffer containing the values to be written
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client, grp or value is a NULL pointer</li>
 *   <li>SLLP_ERR_PARAM_INVALID: grp is not a valid server group</li>
 *   <li>SLLP_ERR_PARAM_OUT_OF_RANGE: op isn't one of the supported
 *                                    operations</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_bin_op_group (sllp_client_t *client, enum sllp_bin_op op,
                                 struct sllp_group *grp, uint8_t *mask);

/*
 * Creates a group of variables from the specified variables list.
 *
 * This function updates the list of groups of a SLLP Client instance if
 * successful.
 *
 * The parameter created_group will be ignored if it is NULL.
 *
 * @param client [input] A SLLP Client Library instance
 * @param vars_list [input] NULL-terminated list of variables to be in the new
 *                          group
 * @param created_group [output] Variable to receive a pointer to the created
 *                               group
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: either client or vars_list is a NULL pointer.
 *   </li>
 *   <li>SLLP_ERR_PARAM_INVALID: vars_list contains an invalid variable</li>
 *   <li>SLLP_ERR_DUPLICATE: vars_list contains a duplicate entry</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 *
 */
enum sllp_err sllp_create_group (sllp_client_t *client,
                                 struct sllp_var_info **vars_list);

/*
 * Removes all custom created groups from a server.
 *
 * @param client [input] A SLLP Client Library instance
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client is a NULL pointer</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_remove_all_groups (sllp_client_t *client);

/*
 * Read a block of values from a specified curve.
 *
 * The data buffer MUST be able to hold SLLP_CURVE_BLOCK_SIZE bytes.
 *
 * @param client [input] A SLLP Client Library instance
 * @param curve [input] The curve to be read
 * @param offset [input] The block to be fetched
 * @param data [output] Buffer to hold the read data
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client, curve or data is a NULL pointer</li>
 *   <li>SLLP_ERR_OUT_OF_RANGE: offset is not less than curve->nblocks</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_request_curve_block (sllp_client_t *client,
                                        struct sllp_curve_info *curve,
                                        uint16_t offset, uint8_t *data);

/*
 * Writes values to a block of a curve.
 *
 *  The data buffer MUST be able to hold SLLP_CURVE_BLOCK_SIZE bytes.
 *
 * @param client [input] A SLLP Client Library instance
 * @param curve [input] The curve to be written
 * @param offset [input] The block to be written
 * @param data [input] Buffer containing the data to be written
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: client, curve or data is a NULL pointer</li>
 *   <li>SLLP_ERR_OUT_OF_RANGE: offset is not less than curve->nblocks</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_send_curve_block (sllp_client_t *client,
                                     struct sllp_curve_info *curve,
                                     uint16_t offset, uint8_t *data);

/*
 * Request a recalculation of the checksum of a server curve.
 *
 * The instance's list of curves is updated if the function is successful.
 *
 * @param client [input] A SLLP Client Library instance
 * @param curve [input] The curve to have its checksum recalculated
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: either client or curve is a NULL pointer</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_recalc_checksum (sllp_client_t *client,
                                    struct sllp_curve_info *curve);

/*
 * Request a function to be executed.
 *
 * @param client [input] A SLLP Client Library instance
 * @param func [input] The function to be executed
 * @param error [output] A pointer to an uint8_t that will hold the returned
 *                       error (if an error occurred) or zero otherwise.
 * @param input [input] An array of values to be passed as input. Necessary if
 *                      func->input_bytes > 0.
 * @param output [output] An array of uint8_t that will hold the output of the
 *                        function. Necessary if func->ouput_bytes > 0.
 *
 * @return SLLP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>SLLP_ERR_PARAM_INVALID: either client, func or error is a NULL pointer
 *   </li>
 *   <li>SLLP_ERR_PARAM_INVALID: input is NULL but func->input_size isn't 0</li>
 *   <li>SLLP_ERR_PARAM_INVALID: output is NULL but func->output_size isn't
 *                               0</li>
 *   <li>SLLP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum sllp_err sllp_func_execute (sllp_client_t *client,
                                 struct sllp_func_info *func, uint8_t *error,
                                 uint8_t *input, uint8_t *output);

#endif

