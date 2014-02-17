#include <bsmp/client.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SERIAL_HDR_SIZE         1
#define SERIAL_CSUM_SIZE        1
#define SERIAL_PKT_WRAP_SIZE    SERIAL_HDR_SIZE + SERIAL_CSUM_SIZE
#define PACKET_SIZE             BSMP_MAX_MESSAGE + SERIAL_PKT_WRAP_SIZE
#define PACKET_HEADER           SERIAL_HDR_SIZE + BSMP_HEADER_SIZE

#ifdef DEBUG
void print_packet (char* pre, uint8_t *data, uint32_t size)
{
    printf("%s: [", pre);

    if(size < 32)
    {
        unsigned int i;
        for(i = 0; i < size; ++i)
            printf("%02X ", data[i]);
        printf("]\n");
    }
    else
        printf("%d bytes ]\n", size);
}
#else
#define print_packet(pre,data,size)
#endif

#define TRY(err, func) \
do { if((err = func)) { fprintf(stderr, #func": %s\n", bsmp_error_str(err));\
                        return; }}while(0)

char *port;
int baud, address, serial;

int read_all(int fd, uint8_t *data, uint32_t count)
{
    unsigned int read_bytes = 0;
    unsigned int remaining = count;

    while(remaining)
    {
        int ret = read(fd, data + read_bytes, remaining);

        if(ret < 0)
        {
            perror("read");
            return EXIT_FAILURE;
        }

        remaining -= ret;
        read_bytes += ret;
    }

    return EXIT_SUCCESS;
}

int puc_send(uint8_t *data, uint32_t *count)
{
    uint8_t  packet[PACKET_SIZE];
    uint32_t packet_size = *count + SERIAL_PKT_WRAP_SIZE;
    uint8_t  *csum = &packet[packet_size - 1];

    *csum = 0;
    packet[0] = address;
    *csum -= address;

    unsigned int i;
    for(i = 0; i < *count; ++i)
    {
        packet[i + 1] = data[i];
        *csum -= data[i];
    }

    print_packet("SEND", packet, packet_size);

    int ret = write(serial, packet, packet_size);

    if(ret != packet_size)
    {
        if(ret < 0)
            perror("write");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int puc_recv(uint8_t *data, uint32_t *count)
{
    uint8_t packet[PACKET_SIZE] = {0};
    uint32_t packet_size;

    if(read_all(serial, packet, PACKET_HEADER))
        return EXIT_FAILURE;

    unsigned int remaining;

    remaining = (packet[2] << 8) + packet[3];

    if(read_all(serial, packet + PACKET_HEADER, remaining + SERIAL_CSUM_SIZE))
        return EXIT_FAILURE;

    packet_size = PACKET_HEADER + remaining + SERIAL_CSUM_SIZE;

    print_packet("RECV", packet, packet_size);

    *count = packet_size - SERIAL_PKT_WRAP_SIZE;
    memcpy(data, packet + SERIAL_HDR_SIZE, *count);

    return EXIT_SUCCESS;
}

void print_vars(struct bsmp_var_info_list *vars)
{
    unsigned int i;
    for(i = 0; i < vars->count; ++i)
    {
        struct bsmp_var_info *var = &vars->list[i];
        printf("VAR id=%d size=%d writable=%d\n", var->id, var->size, var->writable);
    }
}

void print_groups(struct bsmp_group_list *groups)
{
    unsigned int i;
    for(i = 0; i < groups->count; ++i)
    {
        unsigned int j;
        struct bsmp_group *grp = &groups->list[i];
        printf("GROUP id=%d writable=%d size=%d ", grp->id, grp->writable, grp->vars.count);
        printf("vars=[ ");
        for(j = 0; j < grp->vars.count; ++j)
            printf("%d ", grp->vars.list[j]->id);
        printf("]\n");
    }
}

void print_curves(struct bsmp_curve_info_list *curves)
{
    unsigned int i;
    for(i = 0; i < curves->count; ++i)
    {
        struct bsmp_curve_info *curve = &curves->list[i];
        printf("CURVE id=%d blocks=%d writable=%d ", curve->id, curve->nblocks, curve->writable);
        printf("csum=");

        unsigned int j;
        for(j = 0; j < sizeof(curve->checksum); ++j)
            printf("%02X", curve->checksum[j]);
        printf("\n");
    }
}

void test_analog(bsmp_client_t *client, struct bsmp_var_info *ad,
                 struct bsmp_var_info *da)
{
    uint8_t read_back[3] = {0};
    uint8_t adjust[3] = {0, 0, 0};

    puts("Test analog board");

    enum bsmp_err err;
    unsigned int i;
    int written;
    uint32_t value;
    double dvalue;

    for(i = 0; i < (1 << 12); ++i)
    {
        written = 0;

        value = i << 6;
        adjust[2] = value & 0xFF;
        adjust[1] = (value >> 8) & 0xFF;
        adjust[0] = (value >> 16) & 0xFF;

        dvalue = (((double)value)/(1 << 18))*20.0 - 10.0;

        written += printf("Write: %3.3f ", dvalue);

        TRY(err, bsmp_write_read_vars(client,da,adjust,ad,read_back));

        value = (read_back[0] << 16) + (read_back[1] << 8) + read_back[2];
        dvalue = (((double)value)/(1 << 18))*20.0 - 10.0;
        written += printf("Read: %3.3f   ", dvalue);

        while(written--)
            printf("\r");
        fflush(stdout);
    }
    puts("Done                        ");
}

void test_digital(bsmp_client_t *client, struct bsmp_var_info *din,
                  struct bsmp_var_info *dout)
{
    puts("Test digital board");

    enum bsmp_err err;
    uint8_t read_back[1], adjust[1];

    if(dout)
    {
        adjust[0] = 0x55;
        printf("Write %02X ", adjust[0]);
        TRY(err, bsmp_write_var(client, dout, adjust));
    }

    if(din)
    {
        TRY(err, bsmp_read_var(client, din, read_back));
        printf("Read %02X\n", read_back[0]);
    }

    if(dout)
    {
        adjust[0] = 0xF0;
        printf("AND %02X ", adjust[0]);
        TRY(err, bsmp_bin_op_var(client, BIN_OP_AND, dout, adjust));
    }

    if(din)
    {
        TRY(err, bsmp_read_var(client, din, read_back));
        printf("Read %02X\n", read_back[0]);
    }

    if(dout)
    {
        adjust[0] = 0xFF;
        printf("TOGGLE %02X ", adjust[0]);
        TRY(err, bsmp_bin_op_var(client, BIN_OP_OR, dout, adjust));
    }

    if(din)
    {
        TRY(err, bsmp_read_var(client, din, read_back));
        printf("Read %02X\n", read_back[0]);
    }

}

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        fprintf(stderr, "Usage: %s port baud address\n", argv[0]);
        return 0;
    }

    // Get parameters from command line
    port = argv[1];
    baud = atoi(argv[2]);
    address = atoi(argv[3]);

    // Open serial port to communicate with the PUC
    if((serial = open(port, O_RDWR | O_NOCTTY)) < 0)
    {
        perror("open");
        return 0;
    }

    printf("PUC[%d]  %s @ %d bps\n", address, port, baud);

    // Create a new client instance
    bsmp_client_t *bsmp = bsmp_client_new(puc_send, puc_recv);

    if(!bsmp)
    {
        fprintf(stderr, "Error allocating BSMP instance\n");
        goto exit_close;
    }

    // Initialize the client instance (communication must be already working)
    enum bsmp_err err;
    if((err = bsmp_client_init(bsmp)))
    {
        fprintf(stderr, "bsmp_client_init: %s\n", bsmp_error_str(err));
        goto exit_destroy;
    }

    // Get the variables list
    struct bsmp_var_info_list *vars;
    if((err = bsmp_get_vars_list(bsmp, &vars)))
    {
        fprintf(stderr, "bsmp_get_vars_list: %s\n", bsmp_error_str(err));
        goto exit_destroy;
    }

    puts("\nVariables:\n");
    print_vars(vars);

    // Get the groups list
    struct bsmp_group_list *groups;
    if((err = bsmp_get_groups_list(bsmp, &groups)))
    {
        fprintf(stderr, "bsmp_get_groups_list: %s\n", bsmp_error_str(err));
        goto exit_destroy;
    }

    puts("\nGroups:\n");
    print_groups(groups);

    // Get the curves list
    struct bsmp_curve_info_list *curves;
    if((err = bsmp_get_curves_list(bsmp, &curves)))
    {
        fprintf(stderr, "bsmp_get_curves_list: %s\n", bsmp_error_str(err));
        goto exit_destroy;
    }

    puts("\nCurves:\n");
    print_curves(curves);

    /*BASE_VAR_DETECTED_BOARDS,
        BASE_VAR_PM_VAR,
        BASE_VAR_PM_PERIOD,
        BASE_VAR_PM_INDEX,
        BASE_VAR_PM_RUNNING,
        BASE_VAR_SYNC_VAR,
        BASE_VAR_SYNC_CURVE,
        BASE_VAR_SYNC_PROGRESS,*/

    // Search the list for the first of each type of variable
    struct bsmp_var_info *detected_boards   = &vars->list[0];
    struct bsmp_var_info *pm_var            = &vars->list[1];
    struct bsmp_var_info *pm_period         = &vars->list[2];
    struct bsmp_var_info *pm_index          = &vars->list[3];
    struct bsmp_var_info *pm_running        = &vars->list[4];
    struct bsmp_var_info *sync_var          = &vars->list[5];
    struct bsmp_var_info *sync_curve        = &vars->list[6];
    struct bsmp_var_info *sync_progress     = &vars->list[7];

    struct bsmp_var_info *first_ad      = NULL, *first_da       = NULL;
    struct bsmp_var_info *first_digin   = NULL, *first_digout   = NULL;

    unsigned int i;
    for(i = 8; i < vars->count; ++i)
    {
        struct bsmp_var_info *v = &vars->list[i];
        if(!first_ad && v->size == 3 && !v->writable)
            first_ad = v;
        else if(!first_da && v->size == 3 && v->writable)
            first_da = v;
        else if(!first_digin && v->size == 1 && !v->writable)
            first_digin = v;
        else if(!first_digout && v->size == 1 && v->writable)
            first_digout = v;

        if(first_ad && first_da && first_digin && first_digout)
            break;
    }

    // Print them out

    puts("\nImportant variables:\n");
    uint8_t det[4];
    bsmp_read_var(bsmp, detected_boards, det);
    printf("Detected boards: %2X %2X %2X %2X\n", det[0], det[1], det[2], det[3]);

    if(first_ad)     printf("FIRST AD     id=%d\n", first_ad->id);
    if(first_da)     printf("FIRST DA     id=%d\n", first_da->id);
    if(first_digin)  printf("FIRST DIGIN  id=%d\n", first_digin->id);
    if(first_digout) printf("FIRST DIGOUT id=%d\n", first_digout->id);

    if(first_ad && first_da)
        test_analog(bsmp, first_ad, first_da);

    if(first_digin && first_digout)
        test_digital(bsmp, first_digin, first_digout);

exit_destroy:
    bsmp_client_destroy(bsmp);
    puts("BSMP deallocated");
exit_close:
    close(serial);
    puts("Serial port closed");
    return 0;

}
