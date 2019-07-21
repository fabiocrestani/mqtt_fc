/*
 * mqtt_fc_unpack.c
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
#include "mqtt_fc_unpack.h"
#include "mqtt_fc_send_receive.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"

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

uint8_t mqtt_unpack_publish_message(char buffer[], uint32_t len, 
									PublishMessage *received_message)
{
	if (len < RECEIVED_PUBLISH_MIN_MESSAGE_SIZE)
	{
		return FALSE;
	}

	uint16_t topic_name_len = 0;
	uint16_t i = 0;
	uint32_t k = 0;
	int32_t payload_len = 0;

	received_message->header.byte1 = buffer[k++];
	received_message->header.byte2 = buffer[k++];

	received_message->topic_name_len_msb = buffer[k++];
	received_message->topic_name_len_lsb = buffer[k++];

	topic_name_len = received_message->topic_name_len;
	topic_name_len = LIMITER(topic_name_len, MQTT_TOPIC_NAME_MAX_LEN);

	for (i = 0; i < topic_name_len; i++)
	{
		received_message->topic_name[i] = buffer[k++];
	}

	received_message->topic_name[i] = 0;

	if (received_message->header.qos != 0)
	{
		received_message->message_id_msb = buffer[k++];
		received_message->message_id_lsb = buffer[k++];
	}

	payload_len = len - k;
	received_message->payload_len = payload_len;

	for (i = 0; i < payload_len; i++)
	{
		received_message->payload[i] = buffer[k++];
	}
	received_message->payload[i] = 0;

	return TRUE;
}
