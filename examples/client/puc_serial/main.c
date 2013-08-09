#include <sllp_client.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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
    uint8_t packet[17000];
    uint32_t packet_size = *count + 3;
    uint8_t *csum = &packet[packet_size - 1];

    *csum = 0;

    packet[0] = address;
    packet[1] = 0;

    *csum -= address;

    unsigned int i;
    for(i = 0; i < *count; ++i)
    {
        packet[i + 2] = data[i];
        *csum -= data[i];
    }

    /*printf("puc_send [ ");

    if(*count < 32)
    {
        unsigned int i;
        for(i = 0; i < packet_size; ++i)
            printf("%02X ", packet[i]);
        printf("]\n");
    }
    else
        printf("%d bytes ]\n", packet_size);*/


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
    uint8_t packet[17000] = {0};
    uint32_t packet_size;

    if(read_all(serial, packet, 4))
        return EXIT_FAILURE;

    unsigned int remaining;

    if(packet[3] == 255)
        remaining = 16386;
    else
        remaining = packet[3];

    if(read_all(serial, packet + 4, remaining + 1))
        return EXIT_FAILURE;

    packet_size = 4 + remaining + 1;

    /*printf("puc_recv [ ");

    if(packet_size < 32)
    {
        unsigned int i;
        for(i = 0; i < packet_size; ++i)
            printf("%02X ", packet[i]);
        printf("]\n");
    }
    else
        printf("%d bytes ]\n", packet_size);*/

    *count = packet_size - 3;
    memcpy(data, packet + 2, *count);

    return EXIT_SUCCESS;
}

void print_vars(struct sllp_vars_list *vars)
{
    unsigned int i;
    for(i = 0; i < vars->count; ++i)
    {
        struct sllp_var_info *var = &vars->list[i];
        printf("VAR id=%d size=%d writable=%d\n", var->id, var->size, var->writable);
    }
}

void print_groups(struct sllp_groups_list *groups)
{
    unsigned int i;
    for(i = 0; i < groups->count; ++i)
    {
        unsigned int j;
        struct sllp_group *grp = &groups->list[i];
        printf("GROUP id=%d writable=%d size=%d ", grp->id, grp->writable, grp->vars.count);
        printf("vars=[ ");
        for(j = 0; j < grp->vars.count; ++j)
            printf("%d ", grp->vars.list[j]->id);
        printf("]\n");
    }
}

void print_curves(struct sllp_curves_list *curves)
{
    unsigned int i;
    for(i = 0; i < curves->count; ++i)
    {
        struct sllp_curve_info *curve = &curves->list[i];
        printf("CURVE id=%d blocks=%d writable=%d ", curve->id, curve->nblocks, curve->writable);
        printf("csum=");

        unsigned int j;
        for(j = 0; j < sizeof(curve->checksum); ++j)
            printf("%02X", curve->checksum[j]);
        printf("\n");
    }
}

void test_analog(sllp_client_t *client, struct sllp_var_info *ad,
                 struct sllp_var_info *da)
{
    uint8_t read_back[3] = {0};
    uint8_t adjust[3] = {0, 0, 0};

    puts("Test analog board");

    enum sllp_err err;
    unsigned int i;
    int written;
    uint32_t value;
    double dvalue;

    for(i = 0; i < (1 << 12); ++i)
    {
        written = 0;

        if(da)
        {
            value = i << 6;
            adjust[2] = value & 0xFF;
            adjust[1] = (value >> 8) & 0xFF;
            adjust[0] = (value >> 16) & 0xFF;

            dvalue = (((double)value)/(1 << 18))*20.0;
            if(dvalue > 10.0)
                dvalue = dvalue - 20.0;

            written += printf("Write: %3.3f ", dvalue);
            if((err = sllp_write_var(client, da, adjust)))
            {
                fprintf(stderr, "sllp_write_var: %s\n", sllp_error_str(err));
                return;
            }
        }

        if(ad)
        {
            if((err = sllp_read_var(client, ad, read_back)))
            {
                fprintf(stderr, "sllp_read_var: %s\n", sllp_error_str(err));
                return;
            }

            value = (read_back[0] << 16) + (read_back[1] << 8) + read_back[2];
            dvalue = (((double)value)/(1 << 18))*20.0 - 10.0;
            written += printf("Read: %3.3f   ", dvalue);
        }

        while(written--)
            printf("\r");
        fflush(stdout);
    }
    puts("Done                        ");
}

void test_digital(sllp_client_t *client, struct sllp_var_info *din,
                  struct sllp_var_info *dout)
{
    puts("Test digital board");

    enum sllp_err err;
    uint8_t read_back[1], adjust[1];

    if(dout)
    {
        adjust[0] = 0x55;
        printf("Write %02X ", adjust[0]);


        if((err = sllp_write_var(client, dout, adjust)))
        {
            fprintf(stderr, "sllp_write_var: %s\n", sllp_error_str(err));
            return;
        }
    }

    if(din)
    {
        if((err = sllp_read_var(client, din, read_back)))
        {
            fprintf(stderr, "sllp_read_var: %s\n", sllp_error_str(err));
            return;
        }

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
    sllp_client_t *sllp = sllp_client_new(puc_send, puc_recv);

    if(!sllp)
    {
        fprintf(stderr, "Error allocating SLLP instance\n");
        goto exit_close;
    }

    // Initialize the client instance (communication must be already working)
    enum sllp_err err;
    if((err = sllp_client_init(sllp)))
    {
        fprintf(stderr, "sllp_client_init: %s\n", sllp_error_str(err));
        goto exit_destroy;
    }

    // Get the variables list
    struct sllp_vars_list *vars;
    if((err = sllp_get_vars_list(sllp, &vars)))
    {
        fprintf(stderr, "sllp_get_vars_list: %s\n", sllp_error_str(err));
        goto exit_destroy;
    }

    // Get the groups list
    struct sllp_groups_list *groups;
    if((err = sllp_get_groups_list(sllp, &groups)))
    {
        fprintf(stderr, "sllp_get_groups_list: %s\n", sllp_error_str(err));
        goto exit_destroy;
    }

    // Get the curves list
    struct sllp_curves_list *curves;
    if((err = sllp_get_curves_list(sllp, &curves)))
    {
        fprintf(stderr, "sllp_get_curves_list: %s\n", sllp_error_str(err));
        goto exit_destroy;
    }

    // Print the lists
    puts("\nVariables:\n");
    print_vars(vars);

    puts("\nGroups:\n");
    print_groups(groups);

    puts("\nCurves:\n");
    print_curves(curves);

    // Search the lists for the first of each type of variable
    struct sllp_var_info *sync_config, *sync_enable;
    struct sllp_var_info *first_ad, *first_da;
    struct sllp_var_info *first_digin, *first_digout;

    sync_config = &vars->list[0];
    sync_enable = &vars->list[1];
    first_ad = first_da = first_digin = first_digout = NULL;

    unsigned int i;
    for(i = 2; i < vars->count; ++i)
    {
        struct sllp_var_info *v = &vars->list[i];
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
    printf("SYNC_CONFIG id=%d\n", sync_config->id);
    printf("SYNC_ENABLE id=%d\n", sync_enable->id);

    if(first_ad)     printf("FIRST AD     id=%d\n", first_ad->id);
    if(first_da)     printf("FIRST DA     id=%d\n", first_da->id);
    if(first_digin)  printf("FIRST DIGIN  id=%d\n", first_digin->id);
    if(first_digout) printf("FIRST DIGOUT id=%d\n", first_digout->id);

    if(first_ad || first_da)
        test_analog(sllp, first_ad, first_da);

    if(first_digin || first_digout)
        test_digital(sllp, first_digin, first_digout);

exit_destroy:
    sllp_client_destroy(sllp);
    puts("SLLP deallocated");
exit_close:
    close(serial);
    puts("Serial port closed");
    return 0;

}
