/*
 * main.c
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

#include "tcp_fsm.h"
#include "mqtt_fc.h"
#include "mqtt_fc_send_receive.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"
#include "timer.h"

char mqtt_tcp_server_address[256] = "iot.eclipse.org";
uint32_t mqtt_tcp_server_port_number = 1883;
char temp[512];

int main(int argc, char *argv[])
{
	printf("\n");
	
	if (argc > 1)
	{
		strcpy(mqtt_tcp_server_address, argv[1]);
	}

	if (argc > 2)
	{
		mqtt_tcp_server_port_number = atoi(argv[2]);
	}

	Timer timer_mqtt_fsm;
	timer_init(&timer_mqtt_fsm, 1000, 3);
	timer_start(&timer_mqtt_fsm);

	tcp_fsm_init(mqtt_tcp_server_address, mqtt_tcp_server_port_number);
	tcp_fsm_set_connect();

	while (1)
	{
		if (timer_reached(&timer_mqtt_fsm))
		{
			tcp_fsm_poll();
			mqtt_fsm_poll();
		}
		usleep(1000*500);
	}

	return FALSE;


    return 0;
}

