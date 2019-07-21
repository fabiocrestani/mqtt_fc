/*
 * mqtt_fc_pack.c
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
#include "mqtt_fc_send_receive.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"

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
	int32_t payload_len = 0;

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

	if (message.header.qos != 0)
	{
		buffer[len++] = message.message_id_msb;
		buffer[len++] = message.message_id_lsb;
	}

	payload_len = message.header.remaining_length - len + 2;

	for (int32_t i = 0; i < payload_len; i++)
	{
		buffer[len++] = message.payload[i];
	}


//printf("len %d\n", len);
//printf("dump em pack publish 4:\n");
//dump(buffer, len);


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

