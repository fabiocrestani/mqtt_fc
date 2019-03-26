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

int main(int argc, char *argv[])
{
    char buffer[256] = {0x10, 0x10, 0x00, 0x04, 'M', 'Q', 'T', 'T',
						0x04, 0x02, 0x00, 0x3C, 0x00, 0x04, 0x44, 0x49, 
						0x47, 0x49};
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
	printf("[tcp] Writing %u bytes to socket\n", len);
	dump(buffer, len);

	tcp_send(buffer, len);

	tcp_receive(buffer, &len);

	printf("[mqtt] CONNACK response: \n");
    dump(buffer, len);
	dump_fixed_header(buffer[0], buffer[1]);

    return 0;
}
