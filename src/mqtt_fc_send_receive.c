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
#include "mqtt_fc_pack.h"
#include "mqtt_fc_unpack.h"
#include "mqtt_fc_build.h"
#include "mqtt_fc_fsm.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"

CircularBuffer *circular_buffer;

void mqtt_set_circular_buffer(CircularBuffer *ptr_buffer)
{
	circular_buffer = ptr_buffer;
}

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
	PubAckMessage *puback_message;

	if ((type > 0) && (type < MESSAGE_TYPE_COUNT))
	{
		switch (type)
		{
			case CONNECT:
				connect_message = (ConnectMessage *) message;
				len = mqtt_pack_connect_message(*connect_message, buffer);
				log_message((void *) connect_message);
				tcp_send(buffer, len);
			return TRUE;

			
			case PUBLISH:
				publish_message = (PublishMessage *) message;
				len = mqtt_pack_publish_message(*publish_message, buffer);
				log_message((void *) publish_message);
				tcp_send(buffer, len);
			return TRUE;

			case PINGREQ:
				pingreq_message = (PingReqMessage *) message;
				len = mqtt_pack_pingreq_message(*pingreq_message, buffer);
				log_message((void *) pingreq_message);
				tcp_send(buffer, len);
			return TRUE;

			case SUBSCRIBE:
				subscribe_message = (SubscribeMessage *) message;
				len = mqtt_pack_subscribe_message(*subscribe_message, buffer);
				log_message((void *) subscribe_message);
				tcp_send(buffer, len);
			return TRUE;

			case PUBACK:
				puback_message = (PubAckMessage *) message;
				len = mqtt_pack_puback_message(*puback_message, buffer);
				//log_message((void *) puback_message);
				tcp_send(buffer, len);
			return TRUE;

			case CONNACK:
			case PUBREC:
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
				
			case SUBACK:
				return mqtt_handle_received_suback(buffer, len);

			case CONNECT:
			case PUBLISH:
			case PUBREC:
			case PUBREL:
			case PUBCOMP:
			case SUBSCRIBE:
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

void mqtt_rx_poll(void)
{
	char buffer[256];
	uint32_t len = 0;
		
	if (buffer_is_empty(circular_buffer))
	{
		return;
	}

	len = buffer_pop_array(circular_buffer, buffer);

	if (len > 0)
	{
		mqtt_handle_received_message(buffer, len);
	}

}

void mqtt_poll(void)
{
	mqtt_rx_poll();
	mqtt_fsm_poll();
}

