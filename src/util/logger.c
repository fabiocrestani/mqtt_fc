/*
 * logger.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include "mqtt_fc.h"
#include "logger.h"
#include "utils.h"

void logger_print_separator(void);

const char * translate_qos(uint8_t qos);
const char * translate_true_false(uint8_t input);

void dump(char *data, uint32_t len)
{
#if LOG_DUMP == TRUE
	char temp[9];
	uint32_t temp_idx = 0;
	uint32_t byte_counter = 0;

	printf("Dumping %u bytes:\n", len);
	printf("    ");

	for (uint32_t i = 0; i < 8; i++)
	{
		printf("    %u", i);
    }

	printf("\n%03u: ", byte_counter);

	for (uint32_t i = 0; i < len; i++)
	{
		printf("0x%02x ", data[i] & 0xff);

		temp[temp_idx++] = data[i];
	
		if (((i > 0) && (((i+1) % 8) == 0)))
		{
			printf("\t");
			for (uint32_t j = 0; j < temp_idx; j++)
			{
				if (((temp[j] > 33) && (temp[j] < 127)) || (temp[j] == ' '))
				{
					printf("%c", temp[j]);
				}
				else
				{
					printf(".");
				}
			}
			byte_counter = byte_counter + 8;
			printf("\n%03u: ", byte_counter);
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
		if (((temp[j] > 33) && (temp[j] < 127)) || (temp[j] == ' '))
		{
			printf("%c", temp[j]);
		}
		else
		{
			printf(".");
		}
	}
	printf("\n");
#else
	(void) data;
	(void) len;
#endif
}

void logger_print_timestamp(void)
{
	char buffer[256];
	time_t t;
	time(&t);
	struct tm *tm = localtime(&t);
	struct timeval tv;

	gettimeofday(&tv, NULL);
	uint32_t us = tv.tv_usec;

	sprintf(buffer, "[%02d.%02d.%02d %02d:%02d:%02d:%06u]", 
			tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, 
			tm->tm_min, tm->tm_sec, us);

	printf("%s", buffer);
}

void logger_log(char buffer[])
{
	logger_print_timestamp();
	printf(" %s\n", buffer);
}

void dump_parsed_fixed_header(FixedHeader header)
{
	printf("Fixed header:\n");
	printf("byte1: 0x%02x {type: 0x%x (%s) DUP: 0x%x (%s) QoS: 0x%x (%s)\n\
RETAIN: 0x%x (%s)}\nbyte2: 0x%02x {remaining length: %d}\n",
		header.byte1, header.message_type, 
		translate_message_type(header.message_type), header.dup,
		translate_true_false(header.dup), header.qos, translate_qos(header.qos), 
		header.retain, translate_true_false(header.retain), header.byte2,
		header.remaining_length);
}

void dump_parsed_connect_message(ConnectMessage message)
{
	printf("CONNECT message:\n");
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
	printf("CONNACK message: ");
	printf("{Reserved: %d Return code: (%d) %s}\n",
		connack_message.byte1, connack_message.return_code, 
		translate_connack_return_code(connack_message.return_code));
}

void dump_parsed_publish_message(PublishMessage publish_message)
{
	printf("PUBLISH message: ");
	if (publish_message.header.qos == 0)
	{
		printf("{Topic name: (%d) %s Message Identifier: none}\n",
			publish_message.topic_name_len, publish_message.topic_name);
	}
	else
	{
		printf("{Topic name: (%d) %s Message Identifier: %d}\n",
			publish_message.topic_name_len, publish_message.topic_name, 
			publish_message.message_id);
	}
}

void dump_parsed_subscribe_message(SubscribeMessage subscribe_message)
{
	printf("SUBSCRIBE message: ");
	printf("{Topic name: (%d) %s Message Identifier: %d Requested QoS: %d}\n",
			subscribe_message.topic_name_len, subscribe_message.topic_name, 
			subscribe_message.message_id, subscribe_message.requested_qos);
}

void dump_parsed_puback_message(PubAckMessage puback_message)
{
	printf("PUBACK message: ");
	printf("{Message ID: %d}\n", puback_message.message_id);
}

void dump_parsed_pingreq_message(PingReqMessage pingreq_message)
{
	printf("PINGREQ message: ");
	printf("{Message Type: %s}\n", 
		translate_message_type(pingreq_message.header.message_type));
}

void dump_parsed_pingresp_message(PingRespMessage pingresp_message)
{
	printf("PINGRESP message: ");
	printf("{Message Type: %s}\n", 
		translate_message_type(pingresp_message.header.message_type));
}

void dump_parsed_suback_message(SubAckMessage suback_message)
{
	printf("SUBACK message: ");
	printf("{Message Type: %s Message ID: %d Granted QoS: %d}\n", 
		translate_message_type(suback_message.header.message_type),
		suback_message.message_id, suback_message.granted_qos);
}

void dump_connect_message(ConnectMessage message)
{
#if LOG_DUMP_CONNECT == TRUE
	char buffer[1024];
	uint32_t len = 0;
	len = mqtt_pack_connect_message(message, buffer);
	dump(buffer, len);
#else
	(void) message;
#endif
}

void dump_publish_message(PublishMessage message)
{
#if LOG_DUMP_PUBLISH == TRUE
	char buffer[1024];
	uint32_t len = 0;
	len = mqtt_pack_publish_message(message, buffer);
	dump(buffer, len);
#else
	(void) message;
#endif
}

void dump_puback_message(PubAckMessage message)
{
#if LOG_DUMP_PUBACK == TRUE
	char buffer[1024];
	uint32_t len = 0;
	len = mqtt_pack_puback_message(message, buffer);
	dump(buffer, len);
#else
	(void) message;
#endif
}

void dump_subscribe_message(SubscribeMessage message)
{
#if LOG_DUMP_SUBSCRIBE == TRUE
	char buffer[1024];
	uint32_t len = 0;
	len = mqtt_pack_subscribe_message(message, buffer);
	dump(buffer, len);
#else
	(void) message;
#endif
}

void dump_suback_message(SubAckMessage message)
{
#if LOG_DUMP_SUBACK == TRUE
	char buffer[1024];
	uint32_t len = 0;
	len = mqtt_pack_suback_message(message, buffer);
	dump(buffer, len);
#else
	(void) message;
#endif
}

void dump_pingreq_message(PingReqMessage message)
{
#if LOG_DUMP_PINGREQ == TRUE
	char buffer[64];
	uint32_t len = 0;
	len = mqtt_pack_pingreq_message(message, buffer);
	dump(buffer, len);
#else
	(void) message;
#endif
}

void log_message(void * message)
{
	FixedHeader *header = (FixedHeader *) message;
	uint8_t type = header->message_type;

	ConnectMessage *connect_message = 0;
	ConnackMessage *connack_message = 0;
	PublishMessage *publish_message = 0;
	PingReqMessage *pingreq_message = 0;
	PingRespMessage *pingresp_message = 0;
	SubscribeMessage *subscribe_message = 0;
	PubAckMessage *puback_message = 0;
	SubAckMessage *suback_message = 0;

	if ((type == 0) || (type > MESSAGE_TYPE_COUNT))
	{
		return;
	}
	
	switch (type)
	{
		case CONNECT:
			#if LOG_CONNECT == TRUE
				connect_message = (ConnectMessage *) message;
				logger_print_separator();
				dump_connect_message(*connect_message);
				dump_parsed_fixed_header(connect_message->header);
				dump_parsed_connect_message(*connect_message);
				logger_print_separator();
				printf("\n");
			#else
				(void) connect_message;
			#endif
		return;

		case PUBLISH:
			#if LOG_PUBLISH == TRUE
				publish_message = (PublishMessage *) message;
				logger_print_separator();
				dump_parsed_fixed_header(publish_message->header);
				dump_parsed_publish_message(*publish_message);
				dump_publish_message(*publish_message);
				logger_print_separator();
				printf("\n");
				log_publish_message_payload(publish_message);
			#else
				(void) publish_message;
			#endif
		return;

		case PINGREQ:
			#if LOG_PINGREQ == TRUE
				pingreq_message = (PingReqMessage *) message;
				logger_print_separator();
				dump_parsed_fixed_header(pingreq_message->header);
				dump_parsed_pingreq_message(*pingreq_message);
				logger_print_separator();
				printf("\n");
			#else
				(void) pingreq_message;
			#endif
		return;

		case SUBSCRIBE:
			#if LOG_SUBSCRIBE == TRUE
				subscribe_message = (SubscribeMessage *) message;
				logger_print_separator();
				dump_parsed_fixed_header(subscribe_message->header);
				dump_parsed_subscribe_message(*subscribe_message);
				dump_subscribe_message(*subscribe_message);
				logger_print_separator();
				printf("\n");
			#else
				(void) subscribe_message;
			#endif
		return;

		case PUBACK:
			#if LOG_PUBACK == TRUE
				puback_message = (PubAckMessage *) message;
				logger_print_separator();
				dump_parsed_fixed_header(puback_message->header);
				dump_parsed_puback_message(*puback_message);
				dump_puback_message(*puback_message);
				logger_print_separator();
				printf("\n");
			#else
				(void) puback_message;
			#endif
		return;

		case CONNACK:
			#if LOG_CONNACK == TRUE
				connack_message = (ConnackMessage *) message;
				logger_print_separator();
				dump_parsed_fixed_header(connack_message->header);
				dump_parsed_connack_message(*connack_message);
				logger_print_separator();
				printf("\n");
			#else
				(void) connack_message;
			#endif
		return;

		case SUBACK:
			#if LOG_SUBACK == TRUE
				suback_message = (SubAckMessage *) message;
				logger_print_separator();
				dump_parsed_fixed_header(suback_message->header);
				dump_parsed_suback_message(*suback_message);
				dump_suback_message(*suback_message);
				logger_print_separator();
				printf("\n");
			#else
				(void) suback_message;
			#endif
		return;

		case PINGRESP:
			#if LOG_PINGRESP == TRUE
				pingresp_message = (PingRespMessage *) message;
				logger_print_separator();
				dump_parsed_fixed_header(pingresp_message->header);
				dump_parsed_pingresp_message(*pingresp_message);
				logger_print_separator();
				printf("\n");
			#else
				(void) pingresp_message;
			#endif

		case PUBREC:
		case PUBREL:
		case PUBCOMP:
		case UNSUBSCRIBE:
		case UNSUBACK:
		default:
		return;
	}
}

void log_publish_message_payload(PublishMessage *publish_message)
{
#if LOG_PUBLISH_PAYLOAD == TRUE
	char temp[128];

	publish_message->payload[publish_message->payload_len] = 0;

	sprintf(temp, "[mqtt] Publish message payload received: (%u) %s", 
		publish_message->payload_len, publish_message->payload);
	logger_log(temp);
#else
	(void) publish_message;
#endif
}

void logger_print_separator(void)
{
	printf("----------------------------------------\
----------------------------------------\n");
}

const char * translate_message_type(EMessageType message_type)
{
	switch (message_type)
	{
		case CONNECT: return "CONNECT";
		case CONNACK: return "CONNACK";
		case PUBLISH: return "PUBLISH";
		case PUBACK: return "PUBACK";
		case PUBREC: return "PUBREC";
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

const char * translate_qos_level(EQosLevel qos_level)
{
	switch (qos_level)
	{
		case E_QOS_NONE: return "None";
		case E_QOS_PUBACK: return "PUBACK";
		case E_QOS_PUBREC: return "PUBREC";
		default: return "?";
	}
}

const char * translate_qos(uint8_t qos)
{
	switch (qos)
	{
		case 0: return "At most once";
		case 1: return "At least once";
		case 2: return "Exactly once";
		default: return "?";
	}
}

const char * translate_true_false(uint8_t input)
{
	switch (input)
	{
		case FALSE: return "false";
		case TRUE: return "true";
		default: return "?";
	}
}
