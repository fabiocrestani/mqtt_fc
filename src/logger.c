/*
 * logger.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mqtt_fc.h"
#include "utils.h"

const char * translate_message_type(MessageType message_type);
const char * translate_connack_return_code(EConnakReturnCode return_code);

void dump(char *data, uint32_t len)
{
	char temp[9];
	uint32_t temp_idx = 0;
	uint32_t byte_counter = 0;

	printf("Dumping %d bytes:\n", len);
	printf("    ");

	for (uint32_t i = 0; i < 8; i++)
	{
		printf("    %d", i);
    }

	printf("\n%03d: ", byte_counter);

	for (uint32_t i = 0; i < len; i++)
	{
		printf("0x%02x ", data[i]);

		temp[temp_idx++] = data[i];
	
		if (((i > 0) && (((i+1) % 8) == 0)))
		{
			printf("\t");
			for (uint32_t j = 0; j < temp_idx; j++)
			{
				if ((temp[j] > 33) && (temp[j] < 127))
				{
					printf("%c", temp[j]);
				}
				else
				{
					printf(".");
				}
			}
			byte_counter = byte_counter + 8;
			printf("\n%03d: ", byte_counter);
			temp_idx = 0;
		}
	}
	
	for (uint32_t j = temp_idx; j < 8; j++)
	{
		printf(".... ");
	}

	printf("\t");
	for (uint32_t j = 0; j < temp_idx; j++)
	{
		if ((temp[j] > 33) && (temp[j] < 127))
		{
			printf("%c", temp[j]);
		}
		else
		{
			printf(".");
		}
	}
	printf("\n\n");
}

void dump_parsed_fixed_header(FixedHeader header)
{
	printf("Fixed header:\n");
	printf("byte1: 0x%02x {type: 0x%x (%s) DUP: 0x%x QoS: 0x%x RETAIN: 0x%x}\n\
byte2: 0x%02x {remaining length: %d}\n",
		header.byte1, header.message_type, 
		translate_message_type(header.message_type), header.dup, 
		header.qos, header.retain, header.byte2, header.remaining_length);
}

void dump_parsed_connect_message(ConnectMessage message)
{
	printf("Connect message:\n");
	printf("Protocol name (%d): %s Version: %d User name flag: %d \
Password flag: %d\nWill retain flag: %d Will QoS flag: %d Will flag: %d\
\nClean Session flag: %d Reserved flag: %d Keep alive timer: %d\n\
Client ID (%d): %s\n",
		message.protocol_name_len, message.protocol_name, message.version,
		message.user_name_flag, message.password_flag, message.will_retain_flag,
		message.will_qos_flag, message.will_flag, message.clean_session_flag,
		message.reserved_flag, message.keep_alive_timer, message.client_id_len,
		message.client_id);
}

void dump_parsed_connack_message(ConnackMessage connack_message)
{
	printf("Connack message:\n");
	printf("Reserved: %d Return code: (%d) %s\n",
		connack_message.byte1, connack_message.return_code, 
		translate_connack_return_code(connack_message.return_code));
}

void dump_connect_message(ConnectMessage message)
{
	char buffer[1024];
	uint32_t len = 0;
	uint16_t protocol_name_len = message.protocol_name_len;
	uint16_t client_id_len = message.client_id_len;

	buffer[len++] = message.header.byte1;
	buffer[len++] = message.header.byte2;
	buffer[len++] = message.protocol_name_len_msb;
	buffer[len++] = message.protocol_name_len_lsb;

	for (uint16_t i = 0; i < protocol_name_len; i++)
	{
		buffer[len++] = message.protocol_name[i];
	}

	buffer[len++] = message.version;
	buffer[len++] = message.flags;
	buffer[len++] = message.keep_alive_timer_msb;
	buffer[len++] = message.keep_alive_timer_lsb;
	buffer[len++] = message.client_id_len_msb;
	buffer[len++] = message.client_id_len_lsb;

	for (uint32_t i = 0; i < client_id_len; i++)
	{
		buffer[len++] = message.client_id[i];
	}

	dump(buffer, len);
}

const char * translate_message_type(MessageType message_type)
{
	switch (message_type)
	{
		case CONNECT: return "CONNECT";
		case CONNACK: return "CONNACK";
		case PUBLISH: return "PUBLISH";
		case PUBACK: return "PUBACK";
		case PBUREC: return "PBUREC";
		case PUBREL: return "PUBREL";
		case PUBCOMP: return "PUBCOMP";
		case SUBSCRIBE: return "SUBSCRIBE";
		case SUBACK: return "SUBACK";
		case UNSUBSCRIBE: return "UNSUBSCRIBE";
		case UNSUBACK: return "UNSUBACK";
		case PINGREQ: return "PINGREQ";
		case PINGRESP: return "PINGRESP";
		case DISCONNECT: return "DISCONNECT";
		default: return "?";
	}
}

const char * translate_connack_return_code(EConnakReturnCode return_code)
{
	switch (return_code)
	{
		case E_CONNACK_CONNECTION_ACCEPTED:
			return "Connection Accepted";
		case E_CONNACK_UNACCEPTABLE_PROTOCOL_VERSION:
			return "Connection Refused: unacceptable protocol version";
		case E_CONNACK_IDENTIFIER_REJECTED:
			return "Connection Refused: identifier rejected";
		case E_CONNACK_SERVER_UNAVAILABLE:
			return "Connection Refused: server unavailable";
		case E_CONNACK_BAD_USER_NAME_OR_PASSWORD:
			return "Connection Refused: bad user name or password";
		case E_CONNACK_NOT_AUTHORIZED:
			return "Connection Refused: not authorized";
		default: return "?";
	}
}



