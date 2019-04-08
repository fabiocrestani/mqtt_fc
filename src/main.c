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

#include "mqtt_fc.h"
#include "mqtt_fc_send_receive.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"
#include "timer.h"

int main(int argc, char *argv[])
{
	char mqtt_tcp_server_address[] = "iot.eclipse.org";
	uint32_t mqtt_tcp_server_port_number = 1883;

	printf("\n");

	if (!tcp_connect())
	{
		printf("[tcp] Error connecting to %s:%d\n", mqtt_tcp_server_address,
												mqtt_tcp_server_port_number);
		return FALSE;
		
	}

	printf("[tcp] TCP connected to %s:%d\n", mqtt_tcp_server_address,
												 mqtt_tcp_server_port_number);
	///////////////////////////////////////////////////////////////////////////
	// Connect
	///////////////////////////////////////////////////////////////////////////
	printf("\n[mqtt] Sending CONNECT message\n");
	char mqtt_protocol_name[] = "MQTT";
	char mqtt_client_id[] = "fabio";

	mqtt_connect(mqtt_protocol_name, mqtt_client_id);


	///////////////////////////////////////////////////////////////////////////
	// Start main timer for FSM
	///////////////////////////////////////////////////////////////////////////
	Timer timer_mqtt_fsm;
	timer_init(&timer_mqtt_fsm, 1000*1000, 3);
	timer_start(&timer_mqtt_fsm);

	while (1)
	{
		if (timer_reached(&timer_mqtt_fsm))
		{
			printf("timer_reached\n");
		}
		usleep(1000*500);
	}	

	///////////////////////////////////////////////////////////////////////////
	// Publish
	///////////////////////////////////////////////////////////////////////////
	printf("[mqtt] Sending PUBLISH message\n");
	char topic_to_publish[] = "abc";
	char message_to_publish[] = "hello world :)";
	uint16_t message_id = 10;

	mqtt_publish(topic_to_publish, message_to_publish, message_id);
	
    return 0;
}



