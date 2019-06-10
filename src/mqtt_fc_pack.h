/*
 * mqtt_fc_pack.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_PACK_H__
#define __MQTT_PACK_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mqtt_fc.h"

uint32_t mqtt_pack_publish_message(PublishMessage message, char *buffer);
uint32_t mqtt_pack_connect_message(ConnectMessage message, char *buffer);
uint32_t mqtt_pack_puback_message(PubAckMessage message, char *buffer);
uint32_t mqtt_pack_subscribe_message(SubscribeMessage message, char *buffer);
uint32_t mqtt_pack_suback_message(SubAckMessage message, char *buffer);
uint32_t mqtt_pack_pingreq_message(PingReqMessage message, char *buffer);
uint32_t mqtt_pack_pingresp_message(PingRespMessage message, char *buffer);

#endif // __MQTT_PACK_H__
