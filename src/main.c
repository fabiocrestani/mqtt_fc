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
#include "sensor_dummy.h"

CircularBuffer mqtt_rx_buffer;
CircularBuffer mqtt_tx_buffer;

int main(int argc, char *argv[])
{
	char topics_to_subscribe[3][MQTT_TOPIC_NAME_MAX_LEN] = 
		{{"topic_1"}, {"topic_2"}, {"topic_3"}};

	printf("\n\
*********************************************************************\n\
*                                                                   *\n\
* MQTT FC                                                           *\n\
* Author: Fabio Crestani                                            *\n\
* Version: 0.1                                                      *\n\
*                                                                   *\n\
*********************************************************************\n\n");
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

	timer_init(&timer_mqtt_fsm, TIMER_PERIOD_1_MS * 500, 1);
	timer_start(&timer_mqtt_fsm);

	tcp_set_circular_buffer(&mqtt_rx_buffer);
	tcp_set_socket_non_blocking();

	mqtt_set_circular_buffer_rx(mqtt, &mqtt_rx_buffer);
	mqtt_set_circular_buffer_tx(mqtt, &mqtt_tx_buffer);
	mqtt_set_subscribe_topics(mqtt, topics_to_subscribe, 3);
	mqtt_start(mqtt);

	timer_init(&timer_sensor_dummy, TIMER_PERIOD_1_S * 5, 1);
	timer_start(&timer_sensor_dummy);

	printf("\n");
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

