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

typedef struct Mqtt_ Mqtt;

typedef enum EMqttState_ {
	E_MQTT_STATE_IDLE,
	E_MQTT_STATE_TCP_CONNECT,
	E_MQTT_STATE_CONNECT,
	E_MQTT_STATE_CONNECTED,
	E_MQTT_STATE_ERROR,
	E_MQTT_STATE_SUBSCRIBE,
	E_MQTT_STATE_POOL_LOCAL_DATA,
	E_MQTT_STATE_POOL_REMOTE_DATA,
	E_MQTT_STATE_PING,
} EMqttState;

typedef enum EMqttSubState_ {
	E_MQTT_SUBSTATE_SEND,
	E_MQTT_SUBSTATE_WAIT
} EMqttSubstate;

void mqtt_fsm_set_state(Mqtt *mqtt, EMqttState new_state);
void mqtt_fsm_poll(Mqtt *mqtt);

#endif // __MQTT_FC_FSM_H__
