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

	ConnectMessage *m;

	if ((type > 0) && (type < MESSAGE_TYPE_COUNT))
	{
		printf("type ok %d\n", type);

		switch (type)
		{
			case CONNECT:
				m = (ConnectMessage *) message;
				len = mqtt_pack_connect_message(*m, buffer);
				log_connect_message(*m);
				tcp_send(buffer, len);
			break;

			case CONNACK:
			case PUBLISH:
			case PUBACK:
			case PBUREC:
			case PUBREL:
			case PUBCOMP:
			case SUBSCRIBE:
			case SUBACK:
			case UNSUBSCRIBE:
			case UNSUBACK:
			case PINGREQ:
			case PINGRESP:
			default:
			printf("Cannot send message type (%d) %s\n",
						type, translate_message_type(type));
			return FALSE;
		}

		return TRUE;
	}

	printf("[mqtt_send] Error: Invalid message type (%d)\n", type);
	return FALSE;
}
