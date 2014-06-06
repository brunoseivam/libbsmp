libbsmp
=======

Basic Small Messages Protocol Library
-------------------------------------

The BSMP - Basic Small Messages Protocol - is a stateless, synchronous and lightweight protocol. It was designed to be used in serial communication networks of small embedded devices which contain a device with the role of a master.

This protocol manipulates 4 simple things, which are called Entities:

1. Variables
2. Groups
3. Curves
4. Functions

**Variables** can be either writable or read-only and have a value of up to 128 bytes.

A **Group** contains a bunch of Variables that can be read from or written to with only one command.

A **Curve** can be seen as a very large Variable, with up to 65536 blocks of 65520 bytes each.

Finally, a **Function** is a very simple way to perform a Remote Procedure Call (RPC).

Installing
----------

Installing the library is very easy:

    make
    sudo make install

Examples
--------

A full-featured and commented example is available in the `examples/full_example` folder.

Order of calls for a server
---------------------------

When building a server, this would be the order of the calls to be made. They are not very detailed. They are documented in the `server.h` header.

    #include <bsmp/server.h>       // Header with all the necessary functions
    bsmp_server_t srv;             // Server instance
    bsmp_server_init(&srv);        // Initialize the instance

    // Create a function to intervene before reads and after writes, if
    // necessary
    void hook(enum bsmp_operation op, struct bsmp_var **list)
    {
        ...
    }

    bsmp_register_hook(&server,hook);    // Register the hook with the server

    struct bsmp_var v = {...};     // Create a variable
    bsmp_register_var(&bsmp, &v);  // Register a variable with the server

    // Same with bsmp_curve and bsmp_function

    // Once everything is registered, just
    for(;;)
    {
        // Receive a message into a bsmp_raw_packet (recv_pkt here)
        bsmp_process_packet(&srv, &recv_pkt, &send_pkt)
        // Send back the answer in a bsmp_raw_packet (send_pkt in this case)
    }

Order of calls for a client
---------------------------

    #include <bsmp/client.h>        // Header with all the necessary functions
    bsmp_client_t cli;              // Client instance
    bsmp_client_init(&cli, send_func, recv_func);    // initialize instance

    // Get all entities in the server
    struct bsmp_var_info_list *vars;
    bsmp_get_vars_list(&cli, &vars);

    struct bsmp_group_list *groups;
    bsmp_get_groups_list(&cli, &groups);

    struct bsmp_curve_info_list *curves;
    bsmp_get_curves_list(&cli, &curves);

    struct bsmp_func_info_list *funcs;
    bsmp_get_funcs_list(&cli, &funcs);

    // Manipulate entities

    // Read first var
    struct bsmp_var_info *first_var = &vars->list[0];
    uint8_t first_var_value[first-var->size];
    bsmp_read_var(&cli, first_var, first_var_value);

    // Write first var
    first_var_value[0] = 0x42;
    bsmp_write_var(&cli, first_var, first_var_value);

    // Consult the header to see how to manipulate other entities (groups, curves,
    // and functions.

