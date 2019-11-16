/*
 * mqtt_fc_publish_handler.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mqtt_fc_publish_handler.h"
#include "mqtt_fc.h"
#include "mqtt_fc_send_receive.h"
#include "mqtt_fc_pack.h"
#include "mqtt_fc_unpack.h"
#include "mqtt_fc_build.h"
#include "mqtt_fc_fsm.h"
#include "mqtt_fc_tcp_connect.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"

PublishMessage mqtt_publish_handler_build_message(char * topic, uint32_t len,
	uint8_t * data)
{
	PublishMessage m;

	(void) topic;
	(void) len;
	(void) data;

	return m;
}

uint8_t mqtt_publish_handler_add_message_to_queue(Mqtt *mqtt, 
	PublishMessage *message)
{
	if ((mqtt->publish_message_queue_index + 1) > 
			MQTT_OUTPUT_PUBLISH_QUEUE_SIZE)
	{
		mqtt->publish_message_queue[mqtt->publish_message_queue_index++] =
			*message;
		return TRUE;
	}
	return FALSE;
}

uint8_t mqtt_publish_handler_add_data_to_queue(char * topic, uint32_t len,
	uint8_t * data)
{
	PublishMessage message;
	message = mqtt_publish_handler_build_message(topic, len, data);
	
	printf("calling mqtt_add_publish_message_to_queue with: %s %d %s\n",
		topic, len, data);
	return FALSE;
}

