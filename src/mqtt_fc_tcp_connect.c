/*
 * mqtt_fc_tcp_connect.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "mqtt_fc.h"
#include "mqtt_fc_send_receive.h"
#include "mqtt_fc_tcp_connect.h"
#include "tcp.h"
#include "timer.h"
#include "logger.h"
#include "utils.h"
#include "circular_buffer.h"

char mqtt_tcp_server_address[256] = "iot.eclipse.org";
uint32_t mqtt_tcp_server_port_number = 1883;
char temp[512];

void mqtt_tcp_connect_set_server_address(char * address)
{
	strcpy(mqtt_tcp_server_address, address);
}

void mqtt_tcp_connect_set_server_port(uint32_t port)
{
	mqtt_tcp_server_port_number = port;
}

uint8_t mqtt_tcp_server_connect(void)
{
	char temp[256];

	logger_log("Connecting to TCP server...");

	if (!tcp_connect(mqtt_tcp_server_address, mqtt_tcp_server_port_number))
	{
		sprintf(temp, "[tcp] Error connecting to %s:%d", 
						mqtt_tcp_server_address, mqtt_tcp_server_port_number);
		logger_log(temp);
		return FALSE;
	}

	tcp_set_socket_non_blocking();

	sprintf(temp, "[tcp] Connected to %s:%d", mqtt_tcp_server_address,
												 mqtt_tcp_server_port_number);
	logger_log(temp);
	return TRUE;
}

