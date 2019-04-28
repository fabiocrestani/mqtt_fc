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

char mqtt_tcp_server_address[256] = "iot.eclipse.org";
uint32_t mqtt_tcp_server_port_number = 1883;
char temp[512];

static uint8_t tcp_server_connect(void)
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

	sprintf(temp, "[tcp] Connected to %s:%d", mqtt_tcp_server_address,
												 mqtt_tcp_server_port_number);
	logger_log(temp);
	return TRUE;
}

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

	tcp_server_connect();

	///////////////////////////////////////////////////////////////////////////
	// Connect
	///////////////////////////////////////////////////////////////////////////
	logger_log("[mqtt] Sending CONNECT message");
	char mqtt_protocol_name[] = "MQTT";
	char mqtt_client_id[] = "fabio";
	mqtt_connect(mqtt_protocol_name, mqtt_client_id);
	mqtt_receive_response();

	///////////////////////////////////////////////////////////////////////////
	// Publish
	///////////////////////////////////////////////////////////////////////////
	logger_log("[mqtt] Sending PUBLISH message");
	char topic_to_publish[] = "abcd";
	//char message_to_publish[] = "hello world :)";
	char message_to_publish[] = "2";
	mqtt_publish(topic_to_publish, message_to_publish, E_QOS_PUBACK);
	mqtt_receive_response();

	///////////////////////////////////////////////////////////////////////////
	// Ping
	///////////////////////////////////////////////////////////////////////////
	logger_log("[mqtt] Sending PINGREQ message");
	//mqtt_ping_request();
	//mqtt_receive_response();
	
	///////////////////////////////////////////////////////////////////////////
	// Subscribe
	///////////////////////////////////////////////////////////////////////////
	char topic_name[] = "abcd";
	sprintf(temp, "[mqtt] Sending SUBSCRIBE to topic \"%s\"", topic_name);
	logger_log(temp);
	mqtt_subscribe(topic_name, 1);
	mqtt_receive_response();

	tcp_set_socket_non_blocking();

	///////////////////////////////////////////////////////////////////////////
	// Wait for remote PUBLISH messages
	///////////////////////////////////////////////////////////////////////////
	int progress = 0;
	while (1)
	{
		// Poll MQTT for messages
		PublishMessage received_message;		
		if (mqtt_poll_publish_messages(&received_message))
		{
			for (int i = 0; i++ < progress; printf("\b"));
			for (int i = 0; i++ < progress; printf(" "));
			for (int i = 0; i++ < progress; printf("\b"));
			progress = 0;

			//log_publish_message(received_message);
			log_publish_message_payload(received_message);

			// TODO Send ACK if QoS asks for
			mqtt_send_response_to_publish_message(received_message);
		}

		printf(".");
		progress++;
		if (progress >= 30)
		{
			for (int i = 0; i++ < progress; printf("\b"));
			for (int i = 0; i++ < progress; printf(" "));
			for (int i = 0; i++ < progress; printf("\b"));
			progress = 0;
		}

		fflush(stdout);	

		usleep(100*1000);	
	}

    return 0;
}

