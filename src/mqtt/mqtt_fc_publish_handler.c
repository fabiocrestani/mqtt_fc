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
#include "mqtt_fc_qos.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"
#include "circular_message_buffer.h"

static uint32_t id = 0;

uint8_t mqtt_publish_handler_add_message_to_queue(Mqtt *mqtt, 
	PublishMessage *message)
{
	char temp[512];

	if (message_buffer_has_room_for(mqtt->circular_message_buffer_tx, 1))
	{
		message_buffer_put(mqtt->circular_message_buffer_tx, *message);

		sprintf(temp, "Queueing Publish with topic: %s", message->topic_name);
		logger_log_mqtt(TYPE_INFO, temp);
		return TRUE;
	}
	else
	{
		logger_log_mqtt(TYPE_ERROR,
			"Error queueing Publish message. Buffer is full!");
		return FALSE;
	}
}

uint8_t mqtt_publish_handler_add_data_to_queue(Mqtt *mqtt, char * topic, 
	uint32_t len, char * data, EQosLevel qos)
{
	PublishMessage message;

	if (!topic)
	{
		return FALSE;
	}

	message = mqtt_build_publish_message(topic, id, data, len, qos);

	if (qos > 0)
	{
		id++;
	}
	
	mqtt_publish_handler_add_message_to_queue(mqtt, &message);

	return TRUE;
}



