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
#include "mqtt_fc_publish_handler.h"
#include "tcp.h"
#include "timer.h"
#include "logger.h"
#include "utils.h"
#include "circular_buffer.h"
#include "circular_message_buffer.h"
#include "sensor_dummy.h"

CircularBuffer mqtt_rx_buffer;
CircularMessageBuffer mqtt_tx_message_buffer;

int main(int argc, char *argv[])
{
	char topics_to_subscribe[3][MQTT_TOPIC_NAME_MAX_LEN] = 
		{{"topic_1"}, {"topic_2"}, {"topic_3"}};

	printf("\n\
*********************************************************************\n\
*                                                                   *\n\
* MQTT FC                                                           *\n\
* Author: Fabio Crestani                                            *\n\
* Version: 1.0                                                      *\n\
*                                                                   *\n\
*********************************************************************\n\n");
	mqtt_init();
	Mqtt *mqtt = mqtt_get_instance();
	
	// Command line arguments parsing
	if (argc > 1)
	{
		mqtt_tcp_connect_set_server_address(mqtt, argv[1]);
	}

	if (argc > 2)
	{
		mqtt_tcp_connect_set_server_port(mqtt, atoi(argv[2]));
	}

	// Timer init
	timer_init(&timer_mqtt_fsm, TIMER_PERIOD_1_MS * 500, 1);
	timer_start(&timer_mqtt_fsm);

	// TCP init
	tcp_set_circular_buffer(&mqtt_rx_buffer);
	tcp_set_socket_non_blocking();

	// Mqtt init
	buffer_init(&mqtt_rx_buffer);
	message_buffer_init(&mqtt_tx_message_buffer);
	mqtt_set_circular_buffer_rx(mqtt, &mqtt_rx_buffer);
	mqtt_set_circular_message_buffer_tx(mqtt, &mqtt_tx_message_buffer);
	mqtt_set_subscribe_topics(mqtt, topics_to_subscribe, 3);
	mqtt_start(mqtt);

	// Dummy sensor init
	timer_init(&timer_sensor_dummy, TIMER_PERIOD_1_S * 2, 1);
	timer_start(&timer_sensor_dummy);
	dummy_sensor_set_mqtt_reference(mqtt);
	dummy_sensor_set_mqtt_callback(mqtt_publish_handler_add_data_to_queue);

	while (1)
	{
		// Polls TCP for input/output of data
		tcp_poll();

		// Polls MQTT main FSM
		mqtt_poll(mqtt);

		// Dummy sensor produces data and puts in mqtt_tx_buffer
		dummy_sensor_poll();
	
		usleep(1000*1000);
	}

    return 0;
}

