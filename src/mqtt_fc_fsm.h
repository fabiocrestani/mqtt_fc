/*
 * mqtt_fc_fsm.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_FC_FSM_H__
#define __MQTT_FC_FSM_H__

#include "mqtt_fc.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"

typedef enum EnumMqttState_ {
	E_MQTT_STATE_IDLE,
	E_MQTT_STATE_TCP_CONNECT,
	E_MQTT_STATE_CONNECT,
	E_MQTT_STATE_ERROR,
	E_MQTT_STATE_SUBSCRIBE,
	E_MQTT_STATE_POOL_LOCAL_DATA,
	E_MQTT_STATE_POOL_REMOTE_DATA,
	E_MQTT_STATE_PING,
} EnumMqttState;

typedef enum EnumMqttSubState_ {
	E_MQTT_SUBSTATE_SEND,
	E_MQTT_SUBSTATE_WAIT
} EnumMqttSubState;

void mqtt_fsm_set_state(EnumMqttState new_state);
void mqtt_fsm_poll(void);

#endif // __MQTT_FC_FSM_H__
