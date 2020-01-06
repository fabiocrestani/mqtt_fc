/*
 * mqtt_fc_topics_loader.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_FC_TOPICS_LOADER_H__
#define __MQTT_FC_TOPICS_LOADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "utils.h"

uint8_t mqtt_fc_load_topic_from_file(char * filename, 
	char topics[MQTT_SUBSCRIBE_TOPIC_NUM_MAX][MQTT_TOPIC_NAME_MAX_LEN], 
	uint32_t * number_of_entries);

#endif //__MQTT_FC_TOPICS_LOADER_H__
