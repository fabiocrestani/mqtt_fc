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
#include "mqtt_fc_response_handler.h"
#include "mqtt_fc_pack.h"
#include "mqtt_fc_unpack.h"
#include "mqtt_fc_build.h"
#include "mqtt_fc_fsm.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"
#include "circular_buffer.h"
#include "circular_message_buffer.h"

void mqtt_set_circular_buffer_rx(Mqtt *mqtt, CircularBuffer *ptr_buffer)
{
	mqtt->circular_buffer_rx = ptr_buffer;
}

void mqtt_set_circular_message_buffer_tx(Mqtt *mqtt, 
	CircularMessageBuffer *ptr_buffer)
{
	mqtt->circular_message_buffer_tx = ptr_buffer;
}

uint8_t mqtt_send(void * message)
{
	FixedHeader *header = (FixedHeader *) message;
	uint8_t type = header->message_type;
	uint32_t len = 0;
	char buffer[1024];
	uint8_t ret = 0;

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
				ret = tcp_send(buffer, len);
			return ret;

			
			case PUBLISH:
				publish_message = (PublishMessage *) message;
				len = mqtt_pack_publish_message(*publish_message, buffer);
				log_message((void *) publish_message);
				ret = tcp_send(buffer, len);
			return ret;

			case PINGREQ:
				pingreq_message = (PingReqMessage *) message;
				len = mqtt_pack_pingreq_message(*pingreq_message, buffer);
				log_message((void *) pingreq_message);
				ret = tcp_send(buffer, len);
			return ret;

			case SUBSCRIBE:
				subscribe_message = (SubscribeMessage *) message;
				len = mqtt_pack_subscribe_message(*subscribe_message, buffer);
				log_message((void *) subscribe_message);
				ret = tcp_send(buffer, len);
			return ret;

			case PUBACK:
				puback_message = (PubAckMessage *) message;
				len = mqtt_pack_puback_message(*puback_message, buffer);
				//log_message((void *) puback_message);
				ret = tcp_send(buffer, len);
			return ret;

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

			case PUBLISH:
				return mqtt_handle_received_publish(buffer, len);

			case CONNECT:
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

void mqtt_rx_poll(Mqtt *mqtt)
{
	char buffer[256];
	uint32_t len = 0;
		
	if (buffer_is_empty(mqtt->circular_buffer_rx))
	{
		return;
	}

	len = buffer_pop_array(mqtt->circular_buffer_rx, buffer);

	if (len > 0)
	{
		mqtt_handle_received_message(buffer, len);
	}
}

void mqtt_poll(Mqtt *mqtt)
{
	mqtt_rx_poll(mqtt);
	mqtt_fsm_poll(mqtt);
}

