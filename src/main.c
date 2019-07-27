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
#include "mqtt_fc_tcp_connect.h"
#include "tcp.h"
#include "timer.h"
#include "logger.h"
#include "utils.h"
#include "circular_buffer.h"

char temp[512];

CircularBuffer mqtt_rx_buffer;
CircularBuffer mqtt_tx_buffer;

int main(int argc, char *argv[])
{
	printf("\nmqtt_fc\n");
	mqtt_init();
	Mqtt *mqtt = mqtt_get_instance();
	
	if (argc > 1)
	{
		mqtt_tcp_connect_set_server_address(mqtt, argv[1]);
	}

	if (argc > 2)
	{
		mqtt_tcp_connect_set_server_port(mqtt, atoi(argv[2]));
	}

	timer_init(&timer_mqtt_fsm, TIMER_PERIOD_1_MS*1000, 1);
	timer_start(&timer_mqtt_fsm);

	tcp_set_circular_buffer(&mqtt_rx_buffer);
	tcp_set_socket_non_blocking();

	mqtt_set_circular_buffer_rx(mqtt, &mqtt_rx_buffer);
	mqtt_set_circular_buffer_tx(mqtt, &mqtt_tx_buffer);
	mqtt_start(mqtt);

	while (1)
	{
		tcp_poll();

		mqtt_poll(mqtt);

		// Dummy sensor produces data and puts in mqtt tx queue
		//dummy_sensor_poll();
	
		//usleep(1000*1000);
	}


/*	mqtt_receive_response();


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
*/

    return 0;
}

