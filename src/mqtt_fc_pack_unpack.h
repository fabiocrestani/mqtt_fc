/*
 * mqtt_fc_pack_unpack.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_PACK_UNPACK_H__
#define __MQTT_PACK_UNPACK_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mqtt_fc.h"

// Message packers
uint32_t mqtt_pack_publish_message(PublishMessage message, char *buffer);
uint32_t mqtt_pack_connect_message(ConnectMessage message, char *buffer);
uint32_t mqtt_pack_puback_message(PubAckMessage message, char *buffer);
uint32_t mqtt_pack_subscribe_message(SubscribeMessage message, char *buffer);
uint32_t mqtt_pack_suback_message(SubAckMessage message, char *buffer);
uint32_t mqtt_pack_pingreq_message(PingReqMessage message, char *buffer);
uint32_t mqtt_pack_pingresp_message(PingRespMessage message, char *buffer);

// Message unpackers
uint8_t mqtt_unpack_fixed_header(char buffer[], uint32_t len, 
									FixedHeader *header);
uint8_t mqtt_unpack_connack_message(char buffer[], uint32_t len,
								    ConnackMessage *connack_message);
uint8_t mqtt_unpack_puback_message(char buffer[], uint32_t len,
								   PubAckMessage *puback_message);
uint8_t mqtt_unpack_pingresp_message(char buffer[], uint32_t len, 
										PingRespMessage *pingresp_message);
uint8_t mqtt_unpack_suback_message(char buffer[], uint32_t len, 
									SubAckMessage *suback_message);
uint8_t mqtt_unpack_publish_message(char buffer[], uint32_t len, 
									PublishMessage *received_message);

#endif // __MQTT_PACK_UNPACK_H__
