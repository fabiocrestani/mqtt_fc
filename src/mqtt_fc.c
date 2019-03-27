	/*
 * main.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "mqtt_fc.h"
#include "tcp.h"
#include "logger.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

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

ConnectMessage mqtt_build_connect_message()
{
	ConnectMessage m;
	uint8_t remaining_length = 0;

	m.header.message_type = CONNECT;
	m.header.dup = 0;
	m.header.qos = 0;
	m.header.retain = 0;

	char protocol_name[] = "MQTT";
	uint16_t protocol_name_len = strlen(protocol_name);
	m.length = protocol_name_len;
	strcpy(m.protocol_name, protocol_name);
	remaining_length += 2 + protocol_name_len;

	m.version = 4;
	m.flags = 0x02;
	m.keep_alive_timer = 0;
	remaining_length += 3;

	uint32_t payload_len = 0;
	m.payload[payload_len++] = 0x3C;
	m.payload[payload_len++] = 0x00;
	m.payload[payload_len++] = 0x04;
	m.payload[payload_len++] = 0x44;
	m.payload[payload_len++] = 0x49;
	m.payload[payload_len++] = 0x47;
	m.payload[payload_len++] = 0x49;

	remaining_length += payload_len;
	m.header.remaining_length = remaining_length;
	
	return m;
}

void mqtt_pack_connect_message(ConnectMessage message, char *buffer)
{
	uint32_t len = 0;

	buffer[len++] = message.header.byte1;
	buffer[len++] = message.header.byte2;
	buffer[len++] = message.length_msb;
	buffer[len++] = message.length_lsb;
	buffer[len++] = message.protocol_name[0];
	buffer[len++] = message.protocol_name[1];
	buffer[len++] = message.protocol_name[2];
	buffer[len++] = message.protocol_name[3];
	buffer[len++] = message.version;
	buffer[len++] = message.flags;
	buffer[len++] = message.keep_alive_timer;

	uint32_t payload_len = 7;
	for (uint32_t i = 0; i < payload_len; i++)
	{
		buffer[len++] = message.payload[i];
	}
}


int main(int argc, char *argv[])
{
    char buffer[1024];
	unsigned int len = 18;

	if (tcp_connect())
	{
		printf("[tcp] TCP connected to %s:%d\n", "iot.eclipse.org", 1883);
	}
	else
	{
		printf("[tcp] Error connecting to %s:%d\n", "iot.eclipse.org", 1883);
	}

	printf("[mqtt] Sending CONNECT message\n");

	ConnectMessage connect_message = mqtt_build_connect_message();
	mqtt_pack_connect_message(connect_message, buffer);

	dump_connect_message(connect_message);
	dump_fixed_header(connect_message.header);
	dump(buffer, len);

	tcp_send(buffer, len);
	tcp_receive(buffer, &len);

	printf("[mqtt] CONNACK response: \n");
    dump(buffer, len);

	FixedHeader header;
	header.byte1 = buffer[0];
	header.byte2 = buffer[1];
	dump_fixed_header(header);

	printf("\n");
	printf("[mqtt] Sending PUBLISH message\n");
	printf("\n");

	PublishMessage publish_message;
	publish_message.header = mqtt_build_fixed_header(PUBLISH, 0, 1, 0, 0);

	dump_fixed_header(publish_message.header);

	// TODO
	// mqtt_pack_connect_message(connect_message, buffer);
	// tcp_send(buffer, len);
	// tcp_receive(buffer, &len);

    return 0;
}
