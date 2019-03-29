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
#include "utils.h"

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

ConnectMessage mqtt_build_connect_message(char protocol_name[],
	char client_id[])
{
	ConnectMessage m;
	uint16_t keep_alive_timer_value = KEEP_ALIVE_TIMER_VALUE;
	uint16_t protocol_name_len = strlen(protocol_name);
	uint16_t client_id_len = strlen(client_id);
	uint8_t remaining_length = 0;

	protocol_name_len = (protocol_name_len <= CONNECT_PROTOCOL_NAME_MAX_LEN) ?
						 protocol_name_len : CONNECT_PROTOCOL_NAME_MAX_LEN;
	client_id_len = (client_id_len <= COONECT_CLIENT_ID_MAX_LEN) ? 
					client_id_len : COONECT_CLIENT_ID_MAX_LEN;

	m.header.message_type = CONNECT;
	m.header.dup = 0;
	m.header.qos = 0;
	m.header.retain = 0;

	m.protocol_name_len = protocol_name_len;
	strcpy(m.protocol_name, protocol_name);
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

void mqtt_pack_connect_message(ConnectMessage message, char *buffer)
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

	uint32_t client_id_len = 7;
	for (uint32_t i = 0; i < client_id_len; i++)
	{
		buffer[len++] = message.client_id[i];
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

	ConnectMessage connect_message = mqtt_build_connect_message("MQTT", "cafe");
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
