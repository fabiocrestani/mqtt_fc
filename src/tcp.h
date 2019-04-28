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

uint8_t tcp_connect(char server_address[], uint32_t server_port);
uint8_t tcp_disconnect();
uint8_t tcp_send(char buffer[], uint32_t len);
uint8_t tcp_receive(char buffer[], uint32_t *len);
void tcp_set_socket_non_blocking(void);

#endif // __TCP_H__
