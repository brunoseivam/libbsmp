#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

int server_init(void);
int server_process_message(uint8_t *recv_data, unsigned int recv_len,
                           uint8_t *send_data, unsigned int *send_len);

#endif
