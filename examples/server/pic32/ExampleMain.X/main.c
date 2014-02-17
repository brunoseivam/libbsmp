// Internal Oscillator
#pragma config FNOSC    = FRCPLL        // Oscillator Selection
#pragma config POSCMOD  = EC            // Primary Oscillator

// 80 Mhz
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (PIC32 Starter Kit: use divide by 2 only)
#pragma config FPLLMUL  = MUL_20        // PLL Multiplier
#pragma config FPLLODIV = DIV_1         // PLL Output Divider
#pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor

#pragma config FSRSSEL  = PRIORITY_6    // Which Priority Level gets the SRS functionality

#pragma config FWDTEN   = OFF           // Watchdog Timer
#pragma config WDTPS    = PS1           // Watchdog Timer Postscale
#pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
#pragma config OSCIOFNC = ON            // CLKO Enable
#pragma config IESO     = OFF           // Internal/External Switch-over
#pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable
#pragma config CP       = OFF           // Code Protect
#pragma config BWP      = OFF           // Boot Flash Write Protect
#pragma config PWP      = OFF           // Program Flash Write Protect
#pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
#pragma config DEBUG    = OFF           // Debugger Disabled for Starter Kit

#include "server.h"
#include <peripheral/ports.h>
#include <peripheral/uart.h>
#include <peripheral/timer.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define SYS_FREQ                80000000L   // 80 MHz

#define UART_BAUD               1000000L    // UART @ 1Mbps
#define UART_TIMER_PRESCALER    T2_PS_1_8
#define UART_TIMER_FREQ         (SYS_FREQ/8) // Divided by PRESCALER
#define UART_WAIT_TICKS         ((20*UART_TIMER_FREQ)/UART_BAUD) // Wait 20 bits

// UART Driver Enable
#define UART_DE                 LATFbits.LATF12 // Driver enable bit of the UART2

#define SERIAL_HEADER           1   // Destination
#define SERIAL_CSUM             1

#define SERIAL_ADDRESS          1   // My Address

#define SERIAL_BUF_SIZE         (SERIAL_HEADER+16384+SERIAL_CSUM)

void write_PORTE (uint8_t value)
{
    TRISE &= 0xFF00; // Output
    LATE = value;
}

uint8_t read_PORTE (void)
{
    TRISE &= 0x00FF;
    return (LATE & 0xFF);
}

void hook(enum bsmp_operation op, struct bsmp_var **list)
{
    switch(op)
    {
    case BSMP_OP_READ:
        list[0]->data[0] = read_PORTE();
        break;
    case BSMP_OP_WRITE:
        write_PORTE(list[0]->data[0]);
        break;
    }
}

struct serial_buffer
{
    uint8_t data[SERIAL_BUF_SIZE];
    uint16_t index;
    uint8_t csum;
};

struct serial_buffer recv_buffer = {.index = 0};
struct serial_buffer send_buffer = {.index = 0};

struct bsmp_raw_packet recv_packet =
                             { .data = recv_buffer.data + SERIAL_HEADER };
struct bsmp_raw_packet send_packet =
                             { .data = send_buffer.data + SERIAL_HEADER };

bsmp_server_t *bsmp = NULL;

void __ISR(_UART_2_VECTOR, IPL6SRS) serial_byte_received (void)
{
    if(UART2GetErrors())
    {
        UART2ClearAllErrors();
        mU2EClearIntFlag();
        return;
    }

    T2CONCLR = _T2CON_ON_MASK;  // Turn TIMER2 off

    while (UARTReceivedDataIsAvailable(UART2) && recv_buffer.index < SERIAL_BUF_SIZE)
    {
        recv_buffer.data[recv_buffer.index++] = UARTGetDataByte(UART2);
        recv_buffer.csum += recv_buffer.data[recv_buffer.index++];
    }

    WriteTimer2(0);             // Reset TIMER2
    T2CONSET = _T2CON_ON_MASK;  // Turn TIMER2 on

    mU2RXClearIntFlag();        // ACK interrupt

}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) serial_packet_received (void)
{
    unsigned int_status = INTDisableInterrupts();
    T2CONCLR = _T2CON_ON_MASK;  // Turn TIMER2 off

    uint8_t dest   = recv_buffer.data[0];
    uint8_t source = recv_buffer.data[1];

    // Received less than HEADER + CSUM bytes
    if(recv_buffer.index < (SERIAL_HEADER + SERIAL_CSUM))
        goto exit;

    // Checksum is not zero
    if(recv_buffer.csum)
        goto exit;

    // Packet is not for me
    if(dest != SERIAL_ADDRESS)
        goto exit;

    recv_packet.len = recv_buffer.index - SERIAL_HEADER - SERIAL_CSUM;

    // Library will process the packet
    bsmp_process_packet(bsmp, &recv_packet, &send_packet);

    // Prepare answer
    send_buffer.data[0] = source;
    send_buffer.data[1] = dest;
    send_buffer.csum    = 0;

    // Send packet
    unsigned int i;
    UART_DE = 1;
    for(i = 0; i < send_packet.len + SERIAL_HEADER; ++i)
    {
        while(!UARTTransmitterIsReady(UART2));
        send_buffer.csum -= send_buffer.data[i];
        UARTSendDataByte(UART2, send_buffer.data[i]);
    }
    while(!UARTTransmitterIsReady(UART2));
    UARTSendDataByte(UART2, send_buffer.csum);
    while(!UARTTransmissionHasCompleted(UART2));
    UART_DE = 0;

exit:
    recv_buffer.index = 0;
    recv_buffer.csum  = 0;
    send_buffer.index = 0;
    send_buffer.csum  = 0;

    INTClearFlag(INT_T2);
    INTRestoreInterrupts(int_status);
}

int main(void)
{
    // Initialize communications library
    bsmp = bsmp_server_new();
    bsmp_register_hook(bsmp, hook);

    // Register PORTE as a writable var
    uint8_t porte_data[1];
    struct bsmp_var porte_var;
    porte_var.info.size = 1;             // 1 byte
    porte_var.info.writable = true;      // Writable var
    porte_var.data = porte_data;         // Data associated with PORTE variable
    bsmp_register_variable(bsmp, &porte_var);   // Register variable in library

    // Initialize serial port
    UARTConfigure(UART2, UART_ENABLE_HIGH_SPEED);
    UARTSetDataRate(UART2, SYS_FREQ, UART_BAUD);
    UARTSetLineControl(UART2, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE |
                              UART_STOP_BITS_1);
    UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

    ConfigIntUART2(UART_RX_INT_EN | UART_TX_INT_DIS | UART_ERR_INT_EN |
                   UART_INT_PR6 | UART_INT_SUB_PR0);

    OpenTimer2(T2_OFF | UART_TIMER_PRESCALER | T2_SOURCE_INT, UART_WAIT_TICKS);

    ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_5 | T2_INT_SUB_PRIOR_0);
    
    // Enable interrupts
    INTEnableInterrupts();
    INTEnableSystemMultiVectoredInt();

    // Do nothing
    for(;;){}
    return (EXIT_SUCCESS);
}




