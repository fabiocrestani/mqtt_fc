/*
 * mqtt_fc_build.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_FC_BUILD_H__
#define __MQTT_FC_BUILD_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

FixedHeader mqtt_build_fixed_header(uint8_t message_type, uint8_t dup,
	uint8_t qos, uint8_t retain, uint8_t remaining_length);

ConnectMessage mqtt_build_connect_message(char protocol_name[],
	char client_id[]);

PublishMessage mqtt_build_publish_message(char topic_name[], uint16_t id, 
	char payload[], uint32_t payload_len, EQosLevel qos);

SubscribeMessage mqtt_build_subscribe_message(char topic_name[], uint16_t id,
												uint8_t requested_qos);

PingReqMessage mqtt_build_ping_message();

PubAckMessage mqtt_build_puback_message(PublishMessage reference_message);

#endif // __MQTT_FC_BUILD_H__
