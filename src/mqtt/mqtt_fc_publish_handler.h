/*
 * mqtt_fc_publish_handler.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_FC_PUBLISH_HANDLER_H__
#define __MQTT_FC_PUBLISH_HANDLER_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

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

uint8_t mqtt_publish_handler_add_data_to_queue(Mqtt * mqtt, char * topic, 
	uint32_t len, char * data, EQosLevel qos);

#endif // __MQTT_FC_PUBLISH_HANDLER_H__
