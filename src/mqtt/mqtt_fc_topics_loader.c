/*
 * mqtt_fc_topics_loader.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mqtt_fc.h"
#include "utils.h"

uint8_t mqtt_fc_load_topic_from_file(char * filename, 
	char topics[MQTT_SUBSCRIBE_TOPIC_NUM_MAX][MQTT_TOPIC_NAME_MAX_LEN], 
	uint32_t * number_of_entries)
{
	char topic_name[MQTT_TOPIC_NAME_MAX_LEN];
	FILE *f;
	f = fopen(filename, "r");

	if (f == NULL)
	{
		logger_log_mqtt(TYPE_ERROR, 
			"List of topics to subscribe was not found");
		return FALSE;
	}

	*number_of_entries = 0;

	while (fscanf(f, "%s\n", topic_name) != EOF)
	{
		strcpy(topics[*number_of_entries], topic_name);

		(*number_of_entries)++;

		if (*number_of_entries >= MQTT_SUBSCRIBE_TOPIC_NUM_MAX)
		{
			logger_log_mqtt(TYPE_ERROR, "Not all topics could be added. The\
list of topics is too large");
			break;
		}
	}

	fclose(f);

	char temp[1024];
	sprintf(temp, "%d topics to subscribe were found in list of topics",
		*number_of_entries);
	logger_log_mqtt(TYPE_INFO, temp);
	return TRUE;
}
