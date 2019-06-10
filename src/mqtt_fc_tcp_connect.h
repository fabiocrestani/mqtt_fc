/*
 * mqtt_fc_tcp_connect.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_TCP_CONNECT_H__
#define __MQTT_TCP_CONNECT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

void mqtt_tcp_connect_set_server_address(char * address);
void mqtt_tcp_connect_set_server_port(uint32_t port);

uint8_t mqtt_tcp_server_connect(void);

#endif // __MQTT_TCP_CONNECT_H__
