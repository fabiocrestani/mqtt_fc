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

PublishMessage mqtt_publish_handler_build_message(/*data*/);
uint8_t mqtt_publish_handler_add_message_to_queue(Mqtt *mqtt, 
	PublishMessage *message);
uint8_t mqtt_add_publish_message_to_queue(PublishMessage message);

#endif // __MQTT_FC_PUBLISH_HANDLER_H__
