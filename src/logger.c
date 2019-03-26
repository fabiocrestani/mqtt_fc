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

const char * translate_message_type(uint8_t message_type);

void dump_fixed_header(uint8_t byte1, uint8_t byte2)
{
	FixedHeader header;
	header.byte1 = byte1;
	header.byte2 = byte2;

	printf("Fixed header:\n");
	printf("byte1: 0x%02x\n    message_type: 0x%x (%s)\n    \
DUP flag: 0x%x\n    QoS flags: 0x%x\n    RETAIN: 0x%x\n\
byte2: 0x%02x\n    remaining length: %d\n",
		header.byte1, header.message_type, 
		translate_message_type(header.message_type), header.dup_flag, 
		header.qos_flags, header.retain, header.byte2, header.remaining_length);
}

const char * translate_message_type(uint8_t message_type)
{
	switch (message_type)
	{
		case 0x1: return "CONNECT";
		case 0x2: return "CONNACK";
		case 0x3: return "PUBLISH";
		case 0x4: return "PUBACK";
		case 0x5: return "PBUREC";
		case 0x6: return "PUBREL";
		case 0x7: return "PUBCOMP";
		case 0x8: return "SUBSCRIBE";
		case 0x9: return "SUBACK";
		case 0xA: return "UNSUBSCRIBE";
		case 0xB: return "UNSUBACK";
		case 0xC: return "PINGREQ";
		case 0xD: return "PINGRESP";
		case 0xE: return "DISCONNECT";
		default: return "?";
	}
}

void dump(char *data, uint32_t len)
{
	char temp[9];
	uint32_t temp_idx = 0;
	uint32_t byte_counter = 0;

	printf("\n    ");

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
