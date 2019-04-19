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
#include "fsm.h"

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	char mqtt_tcp_server_address[] = "iot.eclipse.org";
	uint32_t mqtt_tcp_server_port_number = 1883;
	char temp[512];

	printf("\n");

	if (!tcp_connect())
	{
		sprintf(temp, "[tcp] Error connecting to %s:%d", 
						mqtt_tcp_server_address, mqtt_tcp_server_port_number);
		logger_log(temp);
		return FALSE;
	}

	sprintf(temp, "[tcp] TCP connected to %s:%d", mqtt_tcp_server_address,
												 mqtt_tcp_server_port_number);
	logger_log(temp);

	///////////////////////////////////////////////////////////////////////////
	// Connect
	///////////////////////////////////////////////////////////////////////////
	logger_log("[mqtt] Sending CONNECT message");
	char mqtt_protocol_name[] = "MQTT";
	char mqtt_client_id[] = "fabio";

	mqtt_connect(mqtt_protocol_name, mqtt_client_id);


	///////////////////////////////////////////////////////////////////////////
	// Start main timer for FSM
	///////////////////////////////////////////////////////////////////////////
	/*Fsm mqtt_fsm;
	fsm_init(&mqtt_fsm);
	FsmState state_tcp_connect; 
	state_tcp_connect = fsm_state_build("tcp connect", *callback_tcp_connect);
	
	Timer timer_mqtt_fsm;
	timer_init(&timer_mqtt_fsm, 1000*1000, 3);
	timer_start(&timer_mqtt_fsm);

	while (1)
	{
		if (timer_reached(&timer_mqtt_fsm))
		{
			fsm_poll(&mqtt_fsm);
		}
		usleep(1000*500);
	}	*/

	///////////////////////////////////////////////////////////////////////////
	// Publish
	///////////////////////////////////////////////////////////////////////////
	logger_log("[mqtt] Sending PUBLISH message");
	char topic_to_publish[] = "abcd";
	//char message_to_publish[] = "hello world :)";
	char message_to_publish[] = "2";

	mqtt_publish(topic_to_publish, message_to_publish, E_QOS_PUBACK);

	///////////////////////////////////////////////////////////////////////////
	// Ping
	///////////////////////////////////////////////////////////////////////////
	logger_log("[mqtt] Sending PINGREQ message");
	//mqtt_ping_request();
	
	///////////////////////////////////////////////////////////////////////////
	// Subscribe
	///////////////////////////////////////////////////////////////////////////
	logger_log("[mqtt] Sending SUBSCRIBE");
	char topic_name[] = "abcd";	
	mqtt_subscribe(topic_name, 1);	

	///////////////////////////////////////////////////////////////////////////
	// Wait for remote PUBLISH messages
	///////////////////////////////////////////////////////////////////////////
	while (1)
	{
		PublishMessage received_message;		
		if (mqtt_poll_publish_messages(&received_message))
		{
			//log_publish_message(received_message);
			log_publish_message_payload(received_message);

			// TODO Send ACK if QoS asks for
			mqtt_send_response_to_publish_message(received_message);
		}

		sleep(1);	
	}




	// TODO implement mqtt disconnect
	//if (tcp_disconnect())
	//{
	//	logger_log("[tcp] TCP disconnected");
	//}
	//else
	//{
	//	logger_log("[tcp] Error on TCP disconnection");
	//}

    return 0;
}

