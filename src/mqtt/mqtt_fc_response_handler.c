/*
 * mqtt_fc_response_handler.c
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
#include "mqtt_fc_pack.h"
#include "mqtt_fc_unpack.h"
#include "mqtt_fc_build.h"
#include "mqtt_fc_send_receive.h"
#include "mqtt_fc_fsm.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"

void mqtt_queue_received_publish(Mqtt *mqtt, PublishMessage *message);

///////////////////////////////////////////////////////////////////////////////
// Response handlers
///////////////////////////////////////////////////////////////////////////////

// The CONNACK message is the message sent by the server in response to a 
// CONNECT request from a client.
uint8_t mqtt_handle_received_connack(char *buffer, uint32_t len)
{
	Mqtt *mqtt = mqtt_get_instance();

	logger_log_mqtt("CONNECT response received: CONNACK");
	ConnackMessage connack_message;
	if (mqtt_unpack_connack_message(buffer, len, &connack_message))
	{
		log_message(&connack_message);
		mqtt->connected = TRUE;
		return TRUE;
	}
	else 
	{
		logger_log_mqtt("Error parsing CONNACK message");
		return FALSE;
	}
}

// A PUBACK message is the response to a PUBLISH message with QoS level 1. 
// A PUBACK message is sent by a server in response to a PUBLISH message from a 
// publishing client, and by a subscriber in response to a PUBLISH message from
// the server.
uint8_t mqtt_handle_received_puback(char *buffer, uint32_t len)
{
	logger_log_mqtt("PUBLISH response received: PUBACK");
	PubAckMessage puback_message;
	if (mqtt_unpack_puback_message(buffer, len, &puback_message))
	{
		log_message(&puback_message);
		return TRUE;
	}
	else 
	{
		logger_log_mqtt("Error parsing PUBACK message");
		return FALSE;
	}
}

// A PINGRESP message is the response sent by a server to a PINGREQ message and
// means "yes I am alive".
uint8_t mqtt_handle_received_pingresp(char *buffer, uint32_t len)
{
	Mqtt *mqtt = mqtt_get_instance();

	logger_log_mqtt("PINGREQ response received: PINGRESP");
	PingRespMessage message;
	if (mqtt_unpack_pingresp_message(buffer, len, &message))
	{
		log_message(&message);
		mqtt->pong_received = TRUE;
		return TRUE;
	}
	else 
	{
		logger_log_mqtt("Error parsing PINGRESP message");
		return FALSE;
	}
}

// A SUBACK message is sent by the server to the client to confirm receipt of a
// SUBSCRIBE message. A SUBACK message contains a list of granted QoS levels. 
// The order of granted QoS levels in the SUBACK message matches the order of 
// the topic names in the corresponding SUBSCRIBE message.
uint8_t mqtt_handle_received_suback(char *buffer, uint32_t len)
{
	Mqtt *mqtt = mqtt_get_instance();

	logger_log_mqtt("SUBSCRIBE response received: SUBACK");
	SubAckMessage message;
	if (mqtt_unpack_suback_message(buffer, len, &message))
	{
		mqtt->wait_for_topic_subscribe = FALSE;
		log_message(&message);
		return TRUE;
	}
	else 
	{
		logger_log_mqtt("Error parsing PINGRESP message");
		return FALSE;
	}
}

// A PUBLISH message is sent by a client to a server for distribution to 
// interested subscribers. Each PUBLISH message is associated with a topic name
// (also known as the Subject or Channel). This is a hierarchical name space
// that defines a taxonomy of information sources for which subscribers can 
// register an interest. A message that is published to a specific topic name is
// delivered to connected subscribers for that topic. If a client subscribes to 
// one or more topics, any message published to those topics are sent by the
// server to the client as a PUBLISH message.
uint8_t mqtt_handle_received_publish(char *buffer, uint32_t len)
{
	Mqtt *mqtt = mqtt_get_instance();

	logger_log_mqtt("PUBLISH message received");
	PublishMessage message;
	if (mqtt_unpack_publish_message(buffer, len, &message))
	{
		mqtt_queue_received_publish(mqtt, &message);
		return TRUE;
	}
	else 
	{
		logger_log_mqtt("Error parsing PUBLISH message");
		return FALSE;
	}
}

// Adds a message to the queue of received publish messages
void mqtt_queue_received_publish(Mqtt *mqtt, PublishMessage *message)
{
	if (mqtt->received_publish_counter >= MQTT_RECEIVED_PUBLISH_QUEUE_SIZE)
	{
		logger_log_mqtt("Error! Queue of received publish messages is full!");
		return;
	}

	log_message(message);

	mqtt->received_publish_message_queue[mqtt->received_publish_counter] = 
		*message;
	(mqtt->received_publish_counter)++;
}

