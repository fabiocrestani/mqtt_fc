/*
 * mqtt_fc_unpack.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_UNPACK_H__
#define __MQTT_UNPACK_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mqtt_fc.h"

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

#endif // __MQTT_UNPACK_H__
