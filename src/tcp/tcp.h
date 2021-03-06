/*
 * tcp.h
 * Author: Fabio Crestani
 */

#ifndef __TCP_H__
#define __TCP_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "circular_buffer.h"

#define TCP_DEFAULT_CONNECT_TIMEOUT_S (5)

//#define TCP_VERBOSE

uint8_t tcp_connect(char server_address[], uint32_t server_port);
uint8_t tcp_disconnect();
uint8_t tcp_send(const char buffer[], const uint32_t len);
uint8_t tcp_receive(char buffer[], uint32_t *len);
uint8_t tcp_is_connected(void);

void tcp_set_circular_buffer(CircularBuffer *ptr_buffer);
void tcp_set_socket_non_blocking(void);

void tcp_poll(void);


#endif // __TCP_H__
