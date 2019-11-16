/*
 * mqtt_fc_build.c
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
#include "mqtt_fc_build.h"
#include "mqtt_fc_pack.h"
#include "mqtt_fc_unpack.h"
#include "mqtt_fc_send_receive.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"

FixedHeader mqtt_build_fixed_header(uint8_t message_type, uint8_t dup,
	uint8_t qos, uint8_t retain, uint8_t remaining_length)
{
	FixedHeader header;
	header.message_type = message_type;
	header.dup = dup;
	header.qos = qos;
	header.retain = retain;
	header.remaining_length = remaining_length;

	if ((header.qos == 0) && (header.remaining_length >= 2))
	{
		header.remaining_length = remaining_length - 2;
	}

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
		char payload[], uint32_t payload_len, EQosLevel qos)
{	
	PublishMessage m;

	uint32_t topic_name_len = strlen(topic_name);
	topic_name_len = LIMITER(topic_name_len, MQTT_TOPIC_NAME_MAX_LEN);

	m.topic_name_len = topic_name_len;
	
	for (uint32_t i = 0; i < topic_name_len; i++)
	{
		m.topic_name[i] = topic_name[i];
	}
	
	m.topic_name[topic_name_len] = 0;

	m.message_id = id;

	payload_len = LIMITER(payload_len, PUBLISH_PAYLOAD_MAX_LEN);

	for (uint32_t i = 0; i < payload_len; i++)
	{
		m.payload[i] = payload[i];
	}

	m.header = mqtt_build_fixed_header(PUBLISH, 0, qos, 0, 
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

PubAckMessage mqtt_build_puback_message(PublishMessage reference_message)
{
	PubAckMessage m;

	m.header = mqtt_build_fixed_header(PUBACK, 0, 0, 0, 2);
	m.message_id = reference_message.message_id;

	return m;
}
