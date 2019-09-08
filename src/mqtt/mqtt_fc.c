/*
 * mqtt_fc.c
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

Mqtt lc_mqtt;

Timer timer_mqtt_fsm;

static uint16_t message_id_counter = 1;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

uint16_t mqtt_get_new_message_id(void)
{
	return message_id_counter++;
}

void mqtt_init(void)
{
	char default_server_address[256] = "iot.eclipse.org";
	uint32_t default_server_port = 1883;

	lc_mqtt.connected = FALSE;
	strcpy(lc_mqtt.server_address, default_server_address);
	lc_mqtt.server_port = default_server_port;

	lc_mqtt.state = E_MQTT_STATE_IDLE;
	lc_mqtt.substate = E_MQTT_SUBSTATE_SEND;
	lc_mqtt.retries = 0;
	lc_mqtt.ping_timeout = 0;
	lc_mqtt.pong_received = FALSE;
	lc_mqtt.is_all_topics_subscribed = FALSE;
	lc_mqtt.wait_for_topic_subscribe = FALSE;
	lc_mqtt.subscribe_topics_number = 0;
	lc_mqtt.subscribe_topics_subscribed = 0;
	lc_mqtt.received_publish_counter = 0;
}

void mqtt_start(Mqtt *mqtt)
{
	mqtt_fsm_set_state(mqtt, E_MQTT_STATE_TCP_CONNECT);
}

void mqtt_restart(Mqtt *mqtt)
{
	lc_mqtt.connected = FALSE;
	lc_mqtt.state = E_MQTT_STATE_IDLE;
	lc_mqtt.substate = E_MQTT_SUBSTATE_SEND;
	lc_mqtt.retries = 0;
	lc_mqtt.ping_timeout = 0;
	lc_mqtt.pong_received = FALSE;
	lc_mqtt.is_all_topics_subscribed = FALSE;
	lc_mqtt.wait_for_topic_subscribe = FALSE;
	lc_mqtt.subscribe_topics_subscribed = 0;
	lc_mqtt.received_publish_counter = 0;
	mqtt_fsm_set_state(mqtt, E_MQTT_STATE_TCP_CONNECT);
}

Mqtt * mqtt_get_instance(void)
{
	return &lc_mqtt;
}

void mqtt_reset_ping_timeout(Mqtt *mqtt)
{
	mqtt->ping_timeout = 0;
}

void mqtt_set_subscribe_topics(Mqtt *mqtt, 
	char topics[][MQTT_TOPIC_NAME_MAX_LEN], uint32_t num_topics)
{
	char temp[512];

	mqtt->subscribe_topics_number = 0;
	for (uint32_t i = 0; i < num_topics; i++)
	{
		if (i > MQTT_SUBSCRIBE_TOPIC_NUM_MAX)
		{
			return;
		}

		strcpy(mqtt->subscribe_topics[i], topics[i]);
		(mqtt->subscribe_topics_number)++;

		sprintf(temp, "Topic \"%s\" was added to list of subscribed topics", 
			mqtt->subscribe_topics[i]);
		logger_log_mqtt(temp);
	}

	if (mqtt->subscribe_topics_number != num_topics)
	{
		sprintf(temp, "WARNING: Not all topics could be added to the list (%d added)", 
			mqtt->subscribe_topics_number);
		logger_log_mqtt(temp);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Command handlers
///////////////////////////////////////////////////////////////////////////////

// When a TCP/IP socket connection is established from a client to a server, a 
// protocol level session must be created using a CONNECT flow.
uint8_t	mqtt_connect(char mqtt_protocol_name[], char mqtt_client_id[])
{
	ConnectMessage connect_message = 
			mqtt_build_connect_message(mqtt_protocol_name, mqtt_client_id);
	mqtt_send((void *) &connect_message);
	return TRUE;
}

// A PUBLISH message is sent by a client to a server for distribution to 
// interested subscribers. Each PUBLISH message is associated with a topic name
// (also known as the Subject or Channel). This is a hierarchical name space 
// that defines a taxonomy of information sources for which subscribers can 
// register an interest. A message that is published to a specific topic name 
// is delivered to connected subscribers for that topic. If a client subscribes 
// to one or more topics, any message published to those topics are sent by the
// server to the client as a PUBLISH message.
uint8_t mqtt_publish(char topic_to_publish[], char message_to_publish[],
						EQosLevel qos)
{
	PublishMessage publish_message;
	publish_message = mqtt_build_publish_message(topic_to_publish, 
						mqtt_get_new_message_id(), message_to_publish, 
						strlen(message_to_publish), qos);
	mqtt_send((void *) &publish_message);
	return TRUE;
}

// The PINGREQ message is an "are you alive?" message that is sent from a 
// connected client to the server
uint8_t mqtt_ping_request(void)
{
	PingReqMessage ping_message;
	ping_message = mqtt_build_ping_message();
	mqtt_send((void *) &ping_message);
	return TRUE;
}

// The SUBSCRIBE message allows a client to register an interest in one or more
// topic names with the server. Messages published to these topics are delivered
// from the server to the client as PUBLISH messages. The SUBSCRIBE message also
// specifies the QoS level at which the subscriber wants to receive published
// messages.
uint8_t mqtt_subscribe(char topic_to_subscribe[], uint8_t requested_qos)
{
	SubscribeMessage subscribe_message;
	subscribe_message = mqtt_build_subscribe_message(topic_to_subscribe,
							mqtt_get_new_message_id(), requested_qos);
	mqtt_send((void *) &subscribe_message);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Received Publish response handler
///////////////////////////////////////////////////////////////////////////////
uint8_t	mqtt_send_response_to_publish_message(PublishMessage publish_message)
{
	PubAckMessage puback_message;
	//PubRecMessage pubrec_message;

	switch (publish_message.header.qos)
	{
		case E_QOS_PUBACK: 
			puback_message = mqtt_build_puback_message(publish_message);
			return mqtt_send(&puback_message);

		//case E_QOS_PUBREC: return mqtt_send_pubrec(message);

		case E_QOS_NONE:
		default:
		return TRUE;
	}
}

// Returns the last PUBLISH received message
uint8_t mqtt_get_last_publish_received_message(Mqtt *mqtt, PublishMessage *message)
{
	if (mqtt->received_publish_counter > 0)
	{
		*message = mqtt->
			received_publish_message_queue[mqtt->received_publish_counter-1];
		(mqtt->received_publish_counter)--;
		return TRUE;
	}
	return FALSE;
}

