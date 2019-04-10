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

uint8_t tcp_connect();
uint8_t tcp_disconnect();
uint8_t tcp_send(char buffer[], uint32_t len);
uint8_t tcp_receive(char buffer[], uint32_t *len);

#endif // __TCP_H__
