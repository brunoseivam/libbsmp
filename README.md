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

Installing the library is very easy:

    mkdir build
    cd build
    ../configure --enable-silent-rules
    make -s
    sudo make install
    

