/*
 * mqtt_fc_send_receive.c
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

uint8_t mqtt_send(void * message)
{
	FixedHeader *header = (FixedHeader *) message;
	uint8_t type = header->message_type;
	uint32_t len = 0;
	char buffer[1024];

	ConnectMessage *connect_message;
	PublishMessage *publish_message;
	PingReqMessage *pingreq_message;
	SubscribeMessage *subscribe_message;

	if ((type > 0) && (type < MESSAGE_TYPE_COUNT))
	{
		switch (type)
		{
			case CONNECT:
				connect_message = (ConnectMessage *) message;
				len = mqtt_pack_connect_message(*connect_message, buffer);
				log_connect_message(*connect_message);
				tcp_send(buffer, len);
			return TRUE;

			
			case PUBLISH:
				publish_message = (PublishMessage *) message;
				len = mqtt_pack_publish_message(*publish_message, buffer);
				log_publish_message(*publish_message);
				tcp_send(buffer, len);
			return TRUE;

			case PINGREQ:
				pingreq_message = (PingReqMessage *) message;
				len = mqtt_pack_pingreq_message(*pingreq_message, 
														buffer);
				log_pingreq_message(*pingreq_message);
				tcp_send(buffer, len);
			return TRUE;

			case SUBSCRIBE:
				subscribe_message = (SubscribeMessage *) message;
				len = mqtt_pack_subscribe_message(*subscribe_message, buffer);
				log_subscribe_message(*subscribe_message);
				tcp_send(buffer, len);
			return TRUE;
		
			case CONNACK:
			case PUBACK:
			case PBUREC:
			case PUBREL:
			case PUBCOMP:
			case SUBACK:
			case UNSUBSCRIBE:
			case UNSUBACK:
			case PINGRESP:
			default:
				printf("Cannot send message type (%d) %s\n",
						type, translate_message_type(type));
			return FALSE;
		}
	}

	printf("[mqtt_send] Error: Invalid message type (%d)\n", type);
	return FALSE;
}

uint8_t mqtt_receive_response(void)
{
	char buffer[1024];
	unsigned int len = 18;

	tcp_receive(buffer, &len);
	mqtt_handle_received_message(buffer, len);

	return TRUE;
}

uint8_t mqtt_handle_received_message(char *buffer, uint32_t len)
{
	FixedHeader header;
	if (mqtt_unpack_fixed_header(buffer, len, &header))
	{
		uint8_t type = header.message_type;
		switch (type)
		{
			case CONNACK:
				return mqtt_handle_received_connack(buffer, len);

			case PUBACK:
				return mqtt_handle_received_puback(buffer, len);

			case PINGRESP:
				return mqtt_handle_received_pingresp(buffer, len);
				
			case CONNECT:
			case PUBLISH:
			case PBUREC:
			case PUBREL:
			case PUBCOMP:
			case SUBSCRIBE:
			case SUBACK:
			case UNSUBSCRIBE:
			case UNSUBACK:
			case PINGREQ:
			default:
			printf("Cannot handle message type (%d) %s\n",
						type, translate_message_type(type));
			return FALSE;
		
		}
	
	}
	else
	{
		return FALSE;
	}
}











