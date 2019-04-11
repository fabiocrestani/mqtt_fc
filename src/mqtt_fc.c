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
#include "mqtt_fc_send_receive.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"

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

///////////////////////////////////////////////////////////////////////////////
// Build message
///////////////////////////////////////////////////////////////////////////////
FixedHeader mqtt_build_fixed_header(uint8_t message_type, uint8_t dup,
	uint8_t qos, uint8_t retain, uint8_t remaining_length)
{
	FixedHeader header;
	header.message_type = message_type;
	header.dup = dup;
	header.qos = qos;
	header.retain = retain;
	header.remaining_length = remaining_length;

	return header;
}

ConnectMessage mqtt_build_connect_message(char protocol_name[],
	char client_id[])
{
	ConnectMessage m;
	uint16_t keep_alive_timer_value = KEEP_ALIVE_TIMER_VALUE;
	uint16_t protocol_name_len = strlen(protocol_name);
	uint16_t client_id_len = strlen(client_id);
	uint8_t remaining_length = 0;

	protocol_name_len = LIMITER(protocol_name_len,
								CONNECT_PROTOCOL_NAME_MAX_LEN);
	client_id_len = LIMITER(client_id_len, CONNECT_CLIENT_ID_MAX_LEN);

	m.header.message_type = CONNECT;
	m.header.dup = 0;
	m.header.qos = 0;
	m.header.retain = 0;

	m.protocol_name_len = protocol_name_len;

	for (uint32_t i = 0; i < protocol_name_len; i++)
	{
		m.protocol_name[i] = protocol_name[i];
	}

	remaining_length = 2 + protocol_name_len;

	m.version = MQTT_VERSION;
	m.reserved_flag = 0;
	m.clean_session_flag = 1;
	m.will_flag = 0;
	m.will_qos_flag = 0;
	m.will_retain_flag = 0;
	m.password_flag = 0;
	m.user_name_flag = 0;
	m.keep_alive_timer_msb = GET_MSB(keep_alive_timer_value);
	m.keep_alive_timer_lsb = GET_LSB(keep_alive_timer_value);
	remaining_length += 4;

	m.client_id_len_msb = GET_MSB(client_id_len);
	m.client_id_len_lsb = GET_LSB(client_id_len);
	remaining_length += 2;

	for (uint16_t i = 0; i < client_id_len; i++)
	{
		m.client_id[i] = client_id[i];
	}

	remaining_length += client_id_len;
	m.header.remaining_length = remaining_length;
	
	return m;
}

PublishMessage mqtt_build_publish_message(char topic_name[], uint16_t id, 
										  char payload[], uint32_t payload_len)
{	
	PublishMessage m;

	uint32_t topic_name_len = strlen(topic_name);
	topic_name_len = LIMITER(topic_name_len, MQTT_TOPIC_NAME_MAX_LEN);

	m.topic_name_len = topic_name_len;
	
	for (uint32_t i = 0; i < topic_name_len; i++)
	{
		m.topic_name[i] = topic_name[i];
	}

	m.message_id = id;

	payload_len = LIMITER(payload_len, PUBLISH_PAYLOAD_MAX_LEN);

	for (uint32_t i = 0; i < payload_len; i++)
	{
		m.payload[i] = payload[i];
	}

	m.header = mqtt_build_fixed_header(PUBLISH, 0, 1, 0, 
										topic_name_len + 4 + payload_len);

	return m;
}

SubscribeMessage mqtt_build_subscribe_message(char topic_name[], uint16_t id,
												uint8_t requested_qos)
{
	SubscribeMessage m;

	uint32_t topic_name_len = strlen(topic_name);
	topic_name_len = LIMITER(topic_name_len, MQTT_TOPIC_NAME_MAX_LEN);

	m.topic_name_len = topic_name_len;
	
	for (uint32_t i = 0; i < topic_name_len; i++)
	{
		m.topic_name[i] = topic_name[i];
	}

	m.message_id = id;
	m.requested_qos = requested_qos;
	m.header = mqtt_build_fixed_header(SUBSCRIBE, 0, 1, 0, topic_name_len + 5);

	return m;
}

PingReqMessage mqtt_build_ping_message()
{
	PingReqMessage m;
	m.header = mqtt_build_fixed_header(PINGREQ, 0, 0, 0, 0);
	return m;
}

///////////////////////////////////////////////////////////////////////////////
// Pack message to a buffer
///////////////////////////////////////////////////////////////////////////////
uint32_t mqtt_pack_connect_message(ConnectMessage message, char *buffer)
{
	uint32_t len = 0;
	uint16_t remaining_len = message.protocol_name_len;

	buffer[len++] = message.header.byte1;
	buffer[len++] = message.header.byte2;
	buffer[len++] = message.protocol_name_len_msb;
	buffer[len++] = message.protocol_name_len_lsb;

	for (uint32_t i = 0; i < remaining_len; i++)
	{
		buffer[len++] = message.protocol_name[i];
	}

	buffer[len++] = message.version;
	buffer[len++] = message.flags;
	buffer[len++] = message.keep_alive_timer_msb;
	buffer[len++] = message.keep_alive_timer_lsb;
	buffer[len++] = message.client_id_len_msb;
	buffer[len++] = message.client_id_len_lsb;

	uint32_t client_id_len = LIMITER(message.client_id_len,
									CONNECT_CLIENT_ID_MAX_LEN);

	for (uint32_t i = 0; i < client_id_len; i++)
	{
		buffer[len++] = message.client_id[i];
	}

	return len;
}

uint32_t mqtt_pack_publish_message(PublishMessage message, char *buffer)
{
	uint32_t len = 0;

	buffer[len++] = message.header.byte1;
	buffer[len++] = message.header.byte2;

	uint32_t topic_name_len = message.topic_name_len;
	topic_name_len = LIMITER(topic_name_len, MQTT_TOPIC_NAME_MAX_LEN);

	buffer[len++] = message.topic_name_len_msb;
	buffer[len++] = message.topic_name_len_lsb;

	for (uint32_t i = 0; i < topic_name_len; i++)
	{
		buffer[len++] = message.topic_name[i];
	}

	buffer[len++] = message.message_id_msb;
	buffer[len++] = message.message_id_lsb;

	uint32_t payload_len = message.header.remaining_length - len + 2;

	for (uint32_t i = 0; i < payload_len; i++)
	{
		buffer[len++] = message.payload[i];
	}

	return len;
}

uint32_t mqtt_pack_puback_message(PubAckMessage message, char *buffer)
{
	uint32_t len = 0;

	buffer[len++] = message.header.byte1;
	buffer[len++] = message.header.byte2;
	buffer[len++] = message.message_id_msb;
	buffer[len++] = message.message_id_lsb;

	return len;
}

uint32_t mqtt_pack_subscribe_message(SubscribeMessage message, char *buffer)
{
	uint32_t len = 0;

	buffer[len++] = message.header.byte1;
	buffer[len++] = message.header.byte2;

	buffer[len++] = message.message_id_msb;
	buffer[len++] = message.message_id_lsb;

	uint32_t topic_name_len = message.topic_name_len;
	topic_name_len = LIMITER(topic_name_len, MQTT_TOPIC_NAME_MAX_LEN);

	buffer[len++] = message.topic_name_len_msb;
	buffer[len++] = message.topic_name_len_lsb;

	for (uint32_t i = 0; i < topic_name_len; i++)
	{
		buffer[len++] = message.topic_name[i];
	}

	buffer[len++] = message.requested_qos;

	return len;
}

uint32_t mqtt_pack_suback_message(SubAckMessage message, char *buffer)
{
	uint32_t len = 0;

	buffer[len++] = message.header.byte1;
	buffer[len++] = message.header.byte2;
	buffer[len++] = message.message_id_msb;
	buffer[len++] = message.message_id_lsb;
	buffer[len++] = message.granted_qos;

	return len;
}

uint32_t mqtt_pack_pingreq_message(PingReqMessage message, char *buffer)
{
	uint32_t len = 0;

	buffer[len++] = message.header.byte1;
	buffer[len++] = message.header.byte2;

	return len;
}

///////////////////////////////////////////////////////////////////////////////
// Unpack message from a buffer
///////////////////////////////////////////////////////////////////////////////
uint8_t mqtt_unpack_fixed_header(char buffer[], uint32_t len, 
								 FixedHeader *header)
{
	if (len < 2)
	{
		return FALSE;
	}

	header->byte1 = buffer[0];
	header->byte2 = buffer[1];
	
	return TRUE;
}
uint8_t mqtt_unpack_connack_message(char buffer[], uint32_t len,
								    ConnackMessage *connack_message)
{
	if (len != CONNACK_MESSAGE_SIZE)
	{
		return FALSE;
	}

	connack_message->header.byte1 = buffer[0];
	connack_message->header.byte2 = buffer[1];
	connack_message->message[0] = buffer[2];
	connack_message->message[1] = buffer[3];

	return TRUE;
}

uint8_t mqtt_unpack_puback_message(char buffer[], uint32_t len,
								   PubAckMessage *puback_message)
{
	if (len != PUBACK_MESSAGE_SIZE)
	{
		return FALSE;
	}

	puback_message->header.byte1 = buffer[0];
	puback_message->header.byte2 = buffer[1];
	puback_message->message_id_msb = buffer[2];
	puback_message->message_id_lsb = buffer[3];

	return TRUE;
}

uint8_t mqtt_unpack_pingresp_message(char buffer[], uint32_t len, 
										PingRespMessage *pingresp_message)
{
	if (len != PINGRESP_MESSAGE_SIZE)
	{
		return FALSE;
	}

	pingresp_message->header.byte1 = buffer[0];
	pingresp_message->header.byte2 = buffer[1];

	return TRUE;
}

uint8_t mqtt_unpack_suback_message(char buffer[], uint32_t len, 
									SubAckMessage *suback_message)
{
	if (len < SUBACK_MIN_MESSAGE_SIZE)
	{
		return FALSE;
	}

	suback_message->header.byte1 = buffer[0];
	suback_message->header.byte2 = buffer[1];
	suback_message->message_id_msb = buffer[2];
	suback_message->message_id_lsb = buffer[3];
	suback_message->granted_qos = buffer[4];

	return TRUE;
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
	mqtt_receive_response();
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
uint8_t mqtt_publish(char topic_to_publish[], char message_to_publish[])
{
	PublishMessage publish_message;
	publish_message = mqtt_build_publish_message(topic_to_publish, 
						mqtt_get_new_message_id(), message_to_publish, 
						strlen(message_to_publish));
	mqtt_send((void *) &publish_message);
	mqtt_receive_response();
	return TRUE;
}

// The PINGREQ message is an "are you alive?" message that is sent from a 
// connected client to the server
uint8_t mqtt_ping_request(void)
{
	PingReqMessage ping_message;
	ping_message = mqtt_build_ping_message();
	mqtt_send((void *) &ping_message);
	mqtt_receive_response();
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
	mqtt_receive_response();
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Response handlers
///////////////////////////////////////////////////////////////////////////////

// The CONNACK message is the message sent by the server in response to a 
// CONNECT request from a client.
uint8_t mqtt_handle_received_connack(char *buffer, uint32_t len)
{
	logger_log("[mqtt] CONNECT response received: CONNACK");
	ConnackMessage connack_message;
	if (mqtt_unpack_connack_message(buffer, len, &connack_message))
	{
		log_connack_message(connack_message);
		return TRUE;
	}
	else 
	{
		logger_log("[mqtt] Error parsing CONNACK message");
		return FALSE;
	}
}

// A PUBACK message is the response to a PUBLISH message with QoS level 1. 
// A PUBACK message is sent by a server in response to a PUBLISH message from a 
// publishing client, and by a subscriber in response to a PUBLISH message from
// the server.
uint8_t mqtt_handle_received_puback(char *buffer, uint32_t len)
{
	logger_log("[mqtt] PUBLISH response received: PUBACK");
	PubAckMessage puback_message;
	if (mqtt_unpack_puback_message(buffer, len, &puback_message))
	{
		log_puback_message(puback_message);
		return TRUE;
	}
	else 
	{
		logger_log("[mqtt] Error parsing PUBACK message");
		return FALSE;
	}
}

// A PINGRESP message is the response sent by a server to a PINGREQ message and
// means "yes I am alive".
uint8_t mqtt_handle_received_pingresp(char *buffer, uint32_t len)
{
	logger_log("[mqtt] PINGREQ response received: PINGRESP");
	PingRespMessage message;
	if (mqtt_unpack_pingresp_message(buffer, len, &message))
	{
		log_pingresp_message(message);
		return TRUE;
	}
	else 
	{
		logger_log("[mqtt] Error parsing PINGRESP message");
		return FALSE;
	}
}

// A SUBACK message is sent by the server to the client to confirm receipt of a
// SUBSCRIBE message. A SUBACK message contains a list of granted QoS levels. 
// The order of granted QoS levels in the SUBACK message matches the order of 
// the topic names in the corresponding SUBSCRIBE message.
uint8_t mqtt_handle_received_suback(char *buffer, uint32_t len)
{
	logger_log("[mqtt] SUBSCRIBE response received: SUBACK");
	SubAckMessage message;
	if (mqtt_unpack_suback_message(buffer, len, &message))
	{
		log_suback_message(message);
		return TRUE;
	}
	else 
	{
		logger_log("[mqtt] Error parsing PINGRESP message");
		return FALSE;
	}
}

