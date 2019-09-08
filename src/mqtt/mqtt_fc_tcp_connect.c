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

void mqtt_tcp_connect_set_server_address(Mqtt *mqtt, char * address)
{
	strcpy(mqtt->server_address, address);
}

void mqtt_tcp_connect_set_server_port(Mqtt *mqtt, uint32_t port)
{
	mqtt->server_port = port;
}

uint8_t mqtt_tcp_server_connect(Mqtt *mqtt)
{
	char temp[256];

	logger_log_tcp("Connecting to TCP server...");

	if (!tcp_connect(mqtt->server_address, mqtt->server_port))
	{
		return FALSE;
	}

	tcp_set_socket_non_blocking();

	sprintf(temp, "Connected to %s:%u", mqtt->server_address,
												 mqtt->server_port);
	logger_log_tcp(temp);
	return TRUE;
}

