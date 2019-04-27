/*
 * tcp_fsm.c
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
#include "tcp.h"
#include "logger.h"
#include "utils.h"
#include "timer.h"
#include "tcp_fsm.h"

static ENUM_TcpState e_tcp_state;

char mqtt_tcp_server_address[256];
uint32_t mqtt_tcp_server_port_number;

static uint8_t st_connect(void);
static void st_connected(void);

void tcp_fsm_init(char server_address[], uint32_t server_port)
{
	strcpy(mqtt_tcp_server_address, server_address);
	mqtt_tcp_server_port_number = server_port;

	e_tcp_state = TCP_IDLE;
}

void tcp_fsm_poll(void)
{
	switch (e_tcp_state)
	{
		case TCP_IDLE:
		break;
		
		case TCP_CONNECT:
			st_connect();
		break;

		case TCP_CONNECTED:
			st_connected();
		break;

		default:
			printf("unknown state in tcp_fsm_poll: %d\n", e_tcp_state);
			break;
	}	
}

void tcp_fsm_set_idle(void)
{
	e_tcp_state = TCP_IDLE;
}

void tcp_fsm_set_connect(void)
{
	e_tcp_state = TCP_CONNECT;
}

void tcp_fsm_set_connected(void)
{
	e_tcp_state = TCP_CONNECTED;
}

uint8_t tcp_fsm_get_connected(void)
{
	return (e_tcp_state == TCP_CONNECTED);
}

static uint8_t st_connect(void)
{
	char temp[256];

	logger_log("Connecting to TCP server...");

	if (!tcp_connect(mqtt_tcp_server_address, mqtt_tcp_server_port_number))
	{
		sprintf(temp, "[tcp] Error connecting to %s:%d", 
						mqtt_tcp_server_address, mqtt_tcp_server_port_number);
		logger_log(temp);

		tcp_fsm_set_idle();
		return FALSE;
	}

	sprintf(temp, "[tcp] Connected to %s:%d", mqtt_tcp_server_address,
												 mqtt_tcp_server_port_number);
	logger_log(temp);

	tcp_fsm_set_connected();
	return TRUE;
}

static void st_connected(void)
{

	
}


