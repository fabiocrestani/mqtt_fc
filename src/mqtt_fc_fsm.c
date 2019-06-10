/*
 * mqtt_fc_fsm.c
 * Author: Fabio Crestani
 */

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


char * mqtt_fsm_translate_state(EMqttState state);

void mqtt_state_idle(Mqtt *mqtt);
void mqtt_state_tcp_connect(Mqtt *mqtt);
void mqtt_state_connect(Mqtt *mqtt);

void mqtt_fsm_set_state(Mqtt *mqtt, EMqttState new_state)
{
	mqtt->state = new_state;
	mqtt->substate = E_MQTT_SUBSTATE_SEND; 
}

void mqtt_fsm_set_error_state(EMqttState origin)
{
	char temp[512];
	sprintf(temp , "FATAL ERROR! Error in MQTT state %s (%d)", 
		mqtt_fsm_translate_state(origin), origin);
	logger_log(temp);
	exit(1);
}

char * mqtt_fsm_translate_state(EMqttState state)
{
	switch (state)
	{
		case E_MQTT_STATE_IDLE: return "E_MQTT_STATE_IDLE";
		case E_MQTT_STATE_TCP_CONNECT: return "E_MQTT_STATE_TCP_CONNECT";
		case E_MQTT_STATE_CONNECT: return "E_MQTT_STATE_CONNECT";
		case E_MQTT_STATE_ERROR: return "E_MQTT_STATE_ERROR";
		case E_MQTT_STATE_SUBSCRIBE: return "E_MQTT_STATE_SUBSCRIBE";
		case E_MQTT_STATE_POOL_LOCAL_DATA: return "E_MQTT_STATE_POOL_LOCAL_DATA";
		case E_MQTT_STATE_POOL_REMOTE_DATA: return "E_MQTT_STATE_POOL_REMOTE_DATA";
		case E_MQTT_STATE_PING: return "E_MQTT_STATE_PING";
		default: return "?";
	}
}

char * mqtt_fsm_translate_substate(EMqttSubstate substate)
{
	switch (substate)
	{
		case E_MQTT_SUBSTATE_SEND: return "E_MQTT_SUBSTATE_SEND";
		case E_MQTT_SUBSTATE_WAIT: return "E_MQTT_SUBSTATE_WAIT";
		default: return "?";
	}
}

void mqtt_fsm_poll(Mqtt *mqtt)
{
	char temp[512];

	if (!timer_reached(&timer_mqtt_fsm))
	{
		return;
	}

#ifdef LOG_MQTT_FSM_POLL
	sprintf(temp, "State %s (%d), Substate: %s (%d)", 
		mqtt_fsm_translate_state(mqtt->state), mqtt->state,
		mqtt_fsm_translate_substate(mqtt->substate), mqtt->substate);
	logger_log(temp);
#endif

	switch (mqtt->state)
	{
		case E_MQTT_STATE_IDLE: mqtt_state_idle(mqtt); break;
		case E_MQTT_STATE_TCP_CONNECT: mqtt_state_tcp_connect(mqtt); break;
		case E_MQTT_STATE_CONNECT: mqtt_state_connect(mqtt); break;

		case E_MQTT_STATE_ERROR:
		case E_MQTT_STATE_SUBSCRIBE:
		case E_MQTT_STATE_POOL_LOCAL_DATA:
		case E_MQTT_STATE_POOL_REMOTE_DATA:
		case E_MQTT_STATE_PING:
		default:
			sprintf(temp, "Invalid state: %d", mqtt->state);
			logger_log(temp);
	}
}

///////////////////////////////////////////////////////////////////////////
// Idle
///////////////////////////////////////////////////////////////////////////
void mqtt_state_idle(Mqtt *mqtt)
{
	if (mqtt->substate == E_MQTT_SUBSTATE_SEND)
	{
		logger_log("MQTT is waiting to start");
		mqtt->substate = E_MQTT_SUBSTATE_WAIT;
	} 
	else if (mqtt->substate == E_MQTT_SUBSTATE_WAIT)
	{
	}	
}

///////////////////////////////////////////////////////////////////////////
// TCP Connect
///////////////////////////////////////////////////////////////////////////
void mqtt_state_tcp_connect(Mqtt *mqtt)
{
	if (mqtt->substate == E_MQTT_SUBSTATE_SEND)
	{
		if (mqtt_tcp_server_connect(mqtt))
		{
			mqtt_fsm_set_state(mqtt, E_MQTT_STATE_CONNECT);
		}
		else
		{
			mqtt_fsm_set_error_state(E_MQTT_STATE_TCP_CONNECT);
		}
	}
}

///////////////////////////////////////////////////////////////////////////
// Connect
///////////////////////////////////////////////////////////////////////////
void mqtt_state_connect(Mqtt *mqtt)
{
	if (mqtt->substate == E_MQTT_SUBSTATE_SEND)
	{
		logger_log("[mqtt] Sending CONNECT message");
		char mqtt_protocol_name[] = "MQTT";
		char mqtt_client_id[] = "fabio";
		mqtt_connect(mqtt_protocol_name, mqtt_client_id);

		mqtt->substate = E_MQTT_SUBSTATE_WAIT;
	} 
	else if (mqtt->substate == E_MQTT_SUBSTATE_WAIT)
	{
		if (mqtt->connected)
		{
			logger_log("Connected!");
		}
		else
		{
			logger_log("Waiting response from connect");
		}
	}	
}


	



