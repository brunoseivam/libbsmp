/*
 * Sirius Low Level Protocol Library - libsllp
 * Server example
 *
 *
 * This is a fully documented libsllp server example. This is a good place to
 * see the API in action.
 */

/* Some boilerplate */
#include "server.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define S "SERVER: "

#define TRY(name, func)\
    do {\
        enum sllp_err err = func;\
        if(err) {\
            fprintf(stderr, S name": %s\n", sllp_error_str(err));\
            return -1;\
        }\
    }while(0)

/*
 * The very first thing to do is to include the server library definitions. This
 * is done with this directive. Nothing else SLLP-related is needed.
 */
#include <sllp_server.h>


/*
 * It is possible to instantiate many servers. This example instantiates only
 * one. It is stored on a 'global' variable, restricted to this file (hence
 * static). Note that the sllp_server_t type is opaque. You should never try to
 * manipulate its internal fields.
 *
 * An instance must be passed to almost every Server API call.
 */

static sllp_server_t *server = NULL;


/*
* Once we have an instance, we can populate it with Entities. The SLLP defines
* four kinds of entities: Variables, Groups, Curves and Functions. Groups are
* handled automatically by the library, and currently cannot be created by the
* server. The only mandatory Entities, the three standard Groups, are created
* and handled automatically by the library.
*/

/*
 * We can start declaring some Variables. Let's create four:
 *    - a name to identify this server, with at most 64 chars;
 *    - two fake 16-bit A/D converter variables
 *    - one fake digital output variable
 *
 * The memory for each Variable must be handled by the user. Here they are
 * declared separately for each Variable. They must be arrays of type uint8_t
 * (essentially, arrays of bytes).
 *
 * To later identify those Variables, we add a identifier to the 'user' field of
 * every Variable. It can be nothing or whatever desired, as it is a pointer to
 * void (void*).
 *
 */

static uint8_t name_memory[64] = "MIGHTY SERVER"; // Memory block for 'name'
static struct sllp_var name = {
   .data = name_memory,                 // Point to the memory block
   .info.size = sizeof(name_memory),    // Store the size of the memory block
   .info.writable = false,              // Can the client write?
   .user = (void*) "NAME"               // An internal identifier
};

static uint8_t ad_memory[2][2];
static struct sllp_var ad[2] =
{
    [0] = {
        .data = ad_memory[0],
        .info.size = sizeof(ad_memory[0]),
        .info.writable = false,
        .user = (void*) "AD1"
    },

    [1] = {
        .data = ad_memory[1],
        .info.size = sizeof(ad_memory[1]),
        .info.writable = false,
        .user = (void*) "AD2"
    },
};

static uint8_t digital_output_memory[1];
static struct sllp_var digital_output =
{
    .data = digital_output_memory,
    .info.size = sizeof(digital_output_memory),
    .info.writable = true,
    .user = (void*) "DIGITAL"
};

/*
 * Ok, now that we have a bunch of Variables, let's declare some Curves. A Curve
 * is like a very very big Variable. It is a collection of blocks. Each Curve
 * can have at most 65536 blocks of data. Each block has 16384 bytes. Therefore,
 * a single Curve can hold up to 1GB of data. If you need more than that, you
 * can use up to 128 Curves (128 GB of data), which should be enough for
 * everyone.
 *
 * (If you really need to represent more than 128 Curves, you can create more
 * server instances. However, keep in mind that this protocol was not designed
 * to do big data transfers)
 *
 * The user has to define functions to manipulate blocks of a Curve (read and
 * write). The user then has to store pointers to those functions in the Curve
 * structure. If C was a nicer language, we could use lambdas and keep the code
 * clean and beautiful. It isn't, however, so we make do with what we have.
 *
 * We will create two Curves: one little, with 2 blocks (2*16384 bytes) and one
 * big, with 256 blocks (256*16384 bytes).
 */

/*
 * First, let's declare the memory blocks to be used by these Curves.
 */
static uint8_t little_curve_memory[2*SLLP_CURVE_BLOCK_SIZE];
static uint8_t big_curve_memory[256*SLLP_CURVE_BLOCK_SIZE];

/*
 * Every Curve must have two auxiliary functions: read_block and write_block.
 * Here we define those two functions to operate on both our curves
 */
static void curve_read_block (struct sllp_curve *curve, uint16_t block,
                              uint8_t *data)
{
    /* Let's check which curve we have so we can point to the right block. */

    /* Note: the use of strcmp here is unsafe, but bear in mind that this server
     * is just an example. A safer way to do it would to be to use strncmp or
     * not to use strings altogether as identifiers (as they are slow, too).
     */

    /* Note: the library will NOT request access to Curve blocks beyond the
     * specified limits. If you are paranoid or do not trust the library, you
     * can check the block limits yourself.
     */

    uint8_t *block_data;
    if(!strcmp((char*)curve->user, "MY PRETTY LITTLE CURVE"))
        block_data = &little_curve_memory[block*SLLP_CURVE_BLOCK_SIZE];
    else if(!strcmp((char*)curve->user, "MY AWESOME BIG CURVE"))
        block_data = &big_curve_memory[block*SLLP_CURVE_BLOCK_SIZE];
    else
    {
        fprintf(stderr,S"That's weird. I've got an unexpected Curve to read\n");
        return;
    }

    /* Now we need to copy the block requested into the 'data' pointer. */
    memcpy(data, block_data, SLLP_CURVE_BLOCK_SIZE);
}

static void curve_write_block (struct sllp_curve *curve, uint16_t block,
                               uint8_t *data)
{
    /*
     * Same logic used in curve_read_block. Note that this function will only
     * be called for the little Curve, because the big Curve is read-only.
     */
    uint8_t *block_data;
    if(!strcmp((char*)curve->user, "MY PRETTY LITTLE CURVE"))
        block_data = &little_curve_memory[block*SLLP_CURVE_BLOCK_SIZE];
    else
    {
        fprintf(stderr,S"This is not the Curve I'm looking for.\n");
        return;
    }

    /* Now we need to copy the 'data' pointer into the requested block. */
    memcpy(block_data, data, SLLP_CURVE_BLOCK_SIZE);
}

/* Let's declare those Curves already! */
static struct sllp_curve little_curve = {
    .info.nblocks = 2,      // 2 blocks
    .info.writable = true,  // The client can write on this Curve.
    .read_block = curve_read_block,
    .write_block = curve_write_block,
    .user = (void*) "MY PRETTY LITTLE CURVE"
};

static struct sllp_curve big_curve = {
    .info.nblocks = 256,                // 256 blocks
    .info.writable = false,             // Read-only
    .read_block = curve_read_block,
    .user = (void*) "MY AWESOME BIG CURVE"
};

/* It's nice to have those Curves. What about some Functions? I mean, SLLP's
 * Functions. You can think of them as RPC, Remote Procedure Calls. They can
 * do whatever you like. Let's define three of them: one that "starts the
 * conversion" of our fake A/D converters; one that write random values to a
 * specific block of the big Curve; and one that prints some awesome quotes.
 *
 * Functions must return 0 if they are successful or a value greater than 0 (up
 * to 255) if they failed. An error code is particular to a Function.
 */

/* This is a fake convert function. We will just put some random values in the
 * A/D Variables. Bear in mind that in real applications YOU would be
 * responsible to avoid race conditions to all the Entities.
 */
static uint8_t ad_convert(uint8_t *input, uint8_t *output)
{
    printf(S"Starting conversion of the A/D converters...\n");
    ad[0].data[0] = rand() % 256;
    ad[0].data[1] = rand() % 256;

    ad[1].data[0] = rand() % 256;
    ad[1].data[1] = rand() % 256;

    return 0; // Success!!
}

static struct sllp_func ad_convert_func = {
    .func_p = ad_convert,
    .info.input_size = 0,       // Nothing is read from the input parameter
    .info.output_size = 0,      // Nothing is written to the output parameter
};

/* This one will randomize the values of one block of the big Curve. This might
 * take some time to complete.
 */
static uint8_t rand_block(uint8_t *input, uint8_t *output)
{
    uint16_t block = (input[0] << 8) | input[1];

    if(block > big_curve.info.nblocks)
        return 1;                       // 1 means INVALID BLOCK

    unsigned int i;
    for(i = 0; i < SLLP_CURVE_BLOCK_SIZE; ++i)
        big_curve_memory[block*SLLP_CURVE_BLOCK_SIZE + i] = rand() % 256;

    return 0;                           // 0 is SUCCESS!!!
}

static struct sllp_func rand_block_func = {
    .func_p = rand_block,
    .info.input_size = 2,       // It takes 2 bytes to identify the block
    .info.output_size = 0,      // Nothing is written to the output parameter
};

/*
 * Last but not least, we will define our awesome quote printer!
 */
static uint8_t quote (uint8_t *input, uint8_t *output)
{
    static unsigned int quote_index = 0;

    char *quotes[6] = {
        "Never gonna give you up",
        "Never gonna let you down",
        "Never gonna run around and desert you"
        "Never gonna make you cry",
        "Never gonna say goodbye",
        "Never gonna tell a lie and hurt you"
    };

    printf(S"%s\n", quotes[quote_index]);
    quote_index = (quote_index + 1) % 6;

    return 0;   // SUCCESS!!!
}

static struct sllp_func quote_func = {
    .func_p = quote,
    .info.input_size = 0,
    .info.output_size = 0,
};

/*
 * ... and one more thing: sometimes you do not want the library to take care of
 * everything. If you are one of those control freaks, I've got something for
 * you: a hook. Not Captain Hook's hook (that would be silly), but a hook
 * function that will be called right BEFORE a READ command is performed or
 * right AFTER a WRITE command is performed. Our hook here won't do much, just
 * print what is happening.
 */
static void hook (enum sllp_operation op, struct sllp_var **list)
{
    fprintf(stdout, S"Request to %s the Variables: ",
            op == SLLP_OP_READ ? "READ from" : "WRITE to");

    while(*list)
    {
        fprintf(stdout, "%s[id=%d] ", (char*) (*list)->user, (*list)->info.id);
        ++list;
    }

    fprintf(stdout, "\n");
}

/*
 * OKAY! Everything is declared. Let's create the server.
 */
int server_init (void)
{
    /*
     * Initialize our Random Number Generator.
     */
    srand(time(NULL));

    /*
     * This call malloc's a new server instance. If it returns NULL, the
     * allocation failed. Probably there's not enough memory.
     */
    server = sllp_server_new();

    if(!server)
    {
        fprintf(stderr, S"Couldn't allocate a SLLP Server instance\n");
        return -1;
    }

    /*
     * Register the hook
     */
    TRY("reg_hook", sllp_register_hook(server, hook));

    /*
     * Register all Variables
     */
    TRY("reg_var", sllp_register_variable(server, &name));              // ID 0
    TRY("reg_var", sllp_register_variable(server, &ad[0]));             // ID 1
    TRY("reg_var", sllp_register_variable(server, &ad[1]));             // ID 2
    TRY("reg_var", sllp_register_variable(server, &digital_output));    // ID 3

    /*
     * Register all Curves
     */
    TRY("reg_curve", sllp_register_curve(server, &little_curve));       // ID 0
    TRY("reg_curve", sllp_register_curve(server, &big_curve));          // ID 1

    /*
     * Register all Functions
     */
    TRY("reg_func", sllp_register_function(server, &ad_convert_func));  // ID 0
    TRY("reg_func", sllp_register_function(server, &rand_block_func));  // ID 1
    TRY("reg_func", sllp_register_function(server, &quote_func));       // ID 2

    /*
     * Great! Now our server is up and ready to receive some commands.
     * This will be done in the function server_process_message.
     */
    fprintf(stdout, S"Initialized!\n");
    return 0;
}

/*
 * A useful server isn't isolated from the world. Instead it must able to
 * receive messages, do something about them and then return a nice answer
 * saying that everything is OK.
 *
 * We will abstract the communication away because this is a simple example.
 * Here the communication will be consisted of receiving some bytes, arranging
 * them nicely and then returning other bunch of bytes. Those bytes can be
 * transmitted via any media: Ethernet, TCP/IP, Serial lines, telephone lines,
 * smoke signals... As long as you convert those pesky bytes to a struct
 * sllp_raw_packet and pass it to the server, it will work.
 */
int server_process_message(uint8_t *recv_data, unsigned int recv_len,
                           uint8_t *send_data, unsigned int *send_len)
{
    if(!recv_data || !send_data || !send_len)
        return -1;

    struct sllp_raw_packet recv_packet, send_packet;

    recv_packet.data = recv_data;
    recv_packet.len = recv_len;
    send_packet.data = send_data;

    /*
     * The next function will read directly from recv_packet.data and write the
     * result directly to send_packet.data
     */
    if(sllp_process_packet(server, &recv_packet, &send_packet))
        return -1;

    *send_len = send_packet.len;

    return 0;
}
