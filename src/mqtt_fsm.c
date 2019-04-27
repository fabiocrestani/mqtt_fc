/*
 * mqtt_fsm.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mqtt_fc.h"
#include "mqtt_fc_send_receive.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"
#include "timer.h"

void mqtt_fsm_poll(void)
{
	char temp[256];

	///////////////////////////////////////////////////////////////////////////
	// Connect
	///////////////////////////////////////////////////////////////////////////
	logger_log("[mqtt] Sending CONNECT message");
	char mqtt_protocol_name[] = "MQTT";
	char mqtt_client_id[] = "fabio";
	mqtt_connect(mqtt_protocol_name, mqtt_client_id);

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
	char topic_name[] = "abcd";
	sprintf(temp, "[mqtt] Sending SUBSCRIBE to topic \"%s\"", topic_name);
	logger_log(temp);
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

}

