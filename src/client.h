#ifndef BSMP_CLIENT_H
#define BSMP_CLIENT_H

#include "bsmp.h"

// Types

// Communication function (send or receive data). Must return 0 if successful
// and anything but 0 otherwise.
typedef int (*bsmp_comm_func_t) (uint8_t* data, uint32_t *count);

// BSMP Client instance
struct bsmp_client
{
    bsmp_comm_func_t            send, recv;
    struct bsmp_version         server_version;
    struct bsmp_var_info_list   vars;
    struct bsmp_group_list      groups;
    struct bsmp_curve_info_list curves;
    struct bsmp_func_info_list  funcs;
};

// Handle to a client instance
typedef struct bsmp_client bsmp_client_t;

/*
 * Initializes an instance of the BSMP Client Library. Initialization means
 * that information about the server will be queried (list of variables, list
 * of groups, list of curves) and stored in the instance.
 *
 * The instance MUST be initialized only once.
 *
 * The communications functions (send_func and recv_func, passed to the
 * bsmp_client_new function) MUST be able to communicate before bsmp_client_init
 * invocation.
 *
 * @param client [input] Handle to the instance to be initialized
 * @param send_func [input] Function used to send a message
 * @param recv_func [input] Function used to receive a message
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client is a NULL pointer or it was already
 *                               initialized</li>
 *   <li>BSMP_ERR_COMM: There was a communication error during the
 *                      initialization of the bsmp instance</li>
 *   <li>BSMP_ERR_OUT_OF_MEMORY: Not enough memory to complete the
 *                               initialization of the bsmp instance</li>
 * </ul>
 *
 */
enum bsmp_err bsmp_client_init(bsmp_client_t *client,
                               bsmp_comm_func_t send_func,
                               bsmp_comm_func_t recv_func);

/*
 * Returns a pointer to a struct describing the server version of the protocol.
 *
 * The bsmp instance MUST be previously initialized. Otherwise NULL will be
 * returned.
 *
 * @param client [input] A BSMP Client Library instance
 *
 * @return A pointer to a struct bsmp_version or NULL in case of error.
 */
struct bsmp_version *bsmp_get_version(bsmp_client_t *client);

/*
 * Returns the list of variables provided by a server in the list parameter.
 *
 * The bsmp instance MUST be previously initialized. Otherwise an empty list
 * will be returned.
 *
 * @param client [input] A BSMP Client Library instance
 * @param list [output] Address of the variables list pointer
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: either client or list is a NULL pointer</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 *
 */
enum bsmp_err bsmp_get_vars_list (bsmp_client_t *client,
                                  struct bsmp_var_info_list **list);

/*
 * Returns the list of groups provided by the server in the list parameter.
 *
 * The bsmp instance MUST be previously initialized. Otherwise an empty list
 * will be returned.
 *
 * @param client [input] A BSMP Client Library instance
 * @param list [output] Address of the groups list pointer
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: either client or list is a NULL pointer</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 *
 */
enum bsmp_err bsmp_get_groups_list (bsmp_client_t *client,
                                    struct bsmp_group_list **list);

/*
 * Returns the list of curves provided by a server in the list parameter.
 *
 * The bsmp instance MUST be previously initialized. Otherwise an empty list
 * will be returned.
 *
 * @param bsmp [input] A BSMP Client Library instance
 * @param list [output] Address of the curves list pointer
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: either bsmp or list is a NULL pointer</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 *
 */
enum bsmp_err bsmp_get_curves_list (bsmp_client_t *client,
                                    struct bsmp_curve_info_list **list);

/*
 * Returns the list of Functions, provided by a server, in the list parameter.
 *
 * The bsmp instance MUST be previously initialized. Otherwise an empty list
 * will be returned.
 *
 * @param bsmp [input] A BSMP Client Library instance
 * @param list [output] Address of the Functions list pointer
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: either bsmp or list is a NULL pointer</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 *
 */
enum bsmp_err bsmp_get_funcs_list (bsmp_client_t *client,
                                   struct bsmp_func_info_list **list);

/*
 * Reads the value of a variable into a caller provided buffer.
 *
 * The values buffer MUST be able to hold var->size bytes.
 *
 * @param client [input] A BSMP Client Library instance
 * @param var [input] The variable to be read
 * @param value [output] Pointer to a buffer to contain the read values
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client, var or value is a NULL pointer</li>
 *   <li>BSMP_ERR_PARAM_INVALID: var is not a valid server variable</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_read_var (bsmp_client_t *client, struct bsmp_var_info *var,
                             uint8_t *value);

/*
 * Writes values to a variable from a caller provided buffer.
 *
 * The values buffer MUST contain var->size bytes.
 *
 * @param client [input] A BSMP Client Library instance
 * @param var [input] The variable to be written
 * @param value [input] Pointer to a buffer containing the values to be written
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client, var or value is a NULL pointer</li>
 *   <li>BSMP_ERR_PARAM_INVALID: var is not a valid server variable</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_write_var (bsmp_client_t *client, struct bsmp_var_info *var,
                              uint8_t *value);

/*
 * Writes values to a variable from a caller provided buffer and reads a
 * variable to a caller provided buffer in just one protocol command.
 *
 * The values buffer MUST contain var->size bytes for each var.
 *
 * @param client [input] A BSMP Client Library instance
 * @param write_var [input] The variable to be written
 * @param write_value [input] Pointer to a buffer containing the values to be
 *                            written
 * @param read_var [input] The variable to be read
 * @param read_value [output] Pointer to a buffer to contain the read values
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: at least one parameter was a NULL pointer</li>
 *   <li>BSMP_ERR_PARAM_INVALID: write_var or read_var is not a valid server
 *                               variable</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_write_read_vars (bsmp_client_t *client,
                                    struct bsmp_var_info *write_var,
                                    uint8_t *write_value,
                                    struct bsmp_var_info *read_var,
                                    uint8_t *read_value);

/*
 * Reads the values of a group of variables into a caller provided buffer.
 *
 * The values buffer MUST be able to hold group->size bytes.
 *
 * @param client [input] A BSMP Client Library instance
 * @param var [input] The variable to be read
 * @param value [output] Pointer to a buffer to contain the read values
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client, grp or value is a NULL pointer</li>
 *   <li>BSMP_ERR_PARAM_INVALID: grp is not a valid server group</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_read_group (bsmp_client_t *client, struct bsmp_group *grp,
                               uint8_t *values);

/*
 * Writes values to variables in a group from a caller provided buffer.
 *
 * The values buffer MUST contain grp->size bytes.
 *
 * @param client [input] A BSMP Client Library instance
 * @param grp [input] The group to be written
 * @param value [input] Pointer to a buffer containing the values to be written
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client, grp or value is a NULL pointer</li>
 *   <li>BSMP_ERR_PARAM_INVALID: grp is not a valid server group</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_write_group (bsmp_client_t *client, struct bsmp_group *grp,
                                uint8_t *values);

/*
 * Perform a binary operation in a variable with the bits specified by the mask.
 *
 * The mask MUST contain var->size bytes.
 *
 * @param client [input] A BSMP Client Library instance
 * @param var [input] The variable to perform the operation
 * @param value [input] Pointer to a buffer containing the values to be written
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client, var or value is a NULL pointer</li>
 *   <li>BSMP_ERR_PARAM_INVALID: var is not a valid server variable</li>
 *   <li>BSMP_ERR_PARAM_INVALID: op isn't one of the supported operations</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_bin_op_var (bsmp_client_t *client, enum bsmp_bin_op op,
                               struct bsmp_var_info *var, uint8_t *mask);

/*
 * Perform a binary operation in a group of variables with the bits
 * specified by the mask.
 *
 * The mask MUST contain grp->size bytes.
 *
 * @param client [input] A BSMP Client Library instance
 * @param var [input] The group to perform the operation
 * @param value [input] Pointer to a buffer containing the values to be written
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client, grp or value is a NULL pointer</li>
 *   <li>BSMP_ERR_PARAM_INVALID: grp is not a valid server group</li>
 *   <li>BSMP_ERR_PARAM_OUT_OF_RANGE: op isn't one of the supported
 *                                    operations</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_bin_op_group (bsmp_client_t *client, enum bsmp_bin_op op,
                                 struct bsmp_group *grp, uint8_t *mask);

/*
 * Creates a group of variables from the specified variables list.
 *
 * This function updates the list of groups of a BSMP Client instance if
 * successful.
 *
 * @param client [input] A BSMP Client Library instance
 * @param vars_list [input] NULL-terminated list of variables to be in the new
 *                          group
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: either client or vars_list is a NULL pointer.
 *   </li>
 *   <li>BSMP_ERR_PARAM_INVALID: vars_list contains an invalid variable</li>
 *   <li>BSMP_ERR_DUPLICATE: vars_list contains a duplicate entry</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 *
 */
enum bsmp_err bsmp_create_group (bsmp_client_t *client,
                                 struct bsmp_var_info **vars_list);

/*
 * Removes all custom created groups from a server.
 *
 * @param client [input] A BSMP Client Library instance
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client is a NULL pointer</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_remove_all_groups (bsmp_client_t *client);

/*
 * Read a block of values from a specified curve.
 *
 * The data buffer MUST be able to hold up to curve->block_size bytes.
 *
 * @param client [input] A BSMP Client Library instance
 * @param curve [input] The curve to be read
 * @param offset [input] The block to be fetched
 * @param data [output] Buffer to hold the read data
 * @param len [output] Pointer to a variable to hold the number of bytes written
 *                     to the buffer
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client, curve, data or len is a NULL
 *                               pointer</li>
 *   <li>BSMP_ERR_OUT_OF_RANGE: offset is not less than curve->nblocks</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_request_curve_block (bsmp_client_t *client,
                                        struct bsmp_curve_info *curve,
                                        uint16_t offset, uint8_t *data,
                                        uint16_t *len);

/*
 * Read all blocks of from a specified curve.
 *
 * The functions stops when all the blocks have been read or when a block read
 * returned less than curve->block_size bytes.
 *
 * The data buffer MUST be able to hold up to curve->nblocks*curve->block_size
 * bytes.
 *
 * @param client [input] A BSMP Client Library instance
 * @param curve [input] The curve to be read
 * @param data [output] Buffer to hold the read data
 * @param len [output] Pointer to a variable to hold the number of bytes written
 *                     to the buffer
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client, curve, data or len is a NULL
 *                               pointer</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_read_curve (bsmp_client_t *client,
                               struct bsmp_curve_info *curve, uint8_t *data,
                               uint32_t *len);

/*
 * Write values to a block of a curve.
 *
 * @param client [input] A BSMP Client Library instance
 * @param curve [input] The curve to be written
 * @param offset [input] The block to be written
 * @param data [input] Buffer containing the data to be written
 * @param len [input] number of bytes from the buffer to be sent
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client, curve or data is a NULL pointer</li>
 *   <li>BSMP_ERR_OUT_OF_RANGE: offset is not less than curve->nblocks</li>
 *   <li>BSMP_ERR_OUT_OF_RANGE: len is greater than curve->block_size</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_send_curve_block (bsmp_client_t *client,
                                     struct bsmp_curve_info *curve,
                                     uint16_t offset, uint8_t *data,
                                     uint16_t len);

/*
 * Write sequentially to the blocks of a Curve.
 *
 * This function writes, at most, curve->nblocks*curve->block_size to the curve.
 *
 * @param client [input] A BSMP Client Library instance
 * @param curve [input] The curve to be written to
 * @param data [input] Buffer with data to be written
 * @param len [input] Number of bytes to be written to the curve
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: client, curve or data is a NULL pointer</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_write_curve (bsmp_client_t *client,
                                struct bsmp_curve_info *curve, uint8_t *data,
                                uint32_t len);

/*
 * Request a recalculation of the checksum of a server curve.
 *
 * The instance's list of curves is updated if the function is successful.
 *
 * @param client [input] A BSMP Client Library instance
 * @param curve [input] The curve to have its checksum recalculated
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: either client or curve is a NULL pointer</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_recalc_checksum (bsmp_client_t *client,
                                    struct bsmp_curve_info *curve);

/*
 * Request a function to be executed.
 *
 * @param client [input] A BSMP Client Library instance
 * @param func [input] The function to be executed
 * @param error [output] A pointer to an uint8_t that will hold the returned
 *                       error (if an error occurred) or zero otherwise.
 * @param input [input] An array of values to be passed as input. Necessary if
 *                      func->input_bytes > 0.
 * @param output [output] An array of uint8_t that will hold the output of the
 *                        function. Necessary if func->ouput_bytes > 0.
 *
 * @return BSMP_SUCCESS or one of the following errors:
 * <ul>
 *   <li>BSMP_ERR_PARAM_INVALID: either client, func or error is a NULL pointer
 *   </li>
 *   <li>BSMP_ERR_PARAM_INVALID: input is NULL but func->input_size isn't 0</li>
 *   <li>BSMP_ERR_PARAM_INVALID: output is NULL but func->output_size isn't
 *                               0</li>
 *   <li>BSMP_ERR_COMM: There was a failure either sending or receiving a
 *                      message</li>
 * </ul>
 */
enum bsmp_err bsmp_func_execute (bsmp_client_t *client,
                                 struct bsmp_func_info *func, uint8_t *error,
                                 uint8_t *input, uint8_t *output);

#endif

