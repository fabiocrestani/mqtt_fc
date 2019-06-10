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

static EnumMqttState state = E_MQTT_STATE_IDLE;
static EnumMqttSubState substate = E_MQTT_SUBSTATE_SEND;

char * mqtt_fsm_translate_state(EnumMqttState state);

void mqtt_state_idle(void);
void mqtt_state_tcp_connect(void);
void mqtt_state_connect(void);

void mqtt_fsm_set_state(EnumMqttState new_state)
{
	state = new_state;
	substate = E_MQTT_SUBSTATE_SEND; 
}

void mqtt_fsm_set_error_state(EnumMqttState origin)
{
	char temp[512];
	sprintf(temp , "FATAL ERROR! Error in MQTT state %s (%d)", 
		mqtt_fsm_translate_state(origin), origin);
	logger_log(temp);
	exit(1);
}

char * mqtt_fsm_translate_state(EnumMqttState state)
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

void mqtt_fsm_poll(void)
{
	char temp[512];

	if (!timer_reached(&timer_mqtt_fsm))
	{
		return;
	}

	switch (state)
	{
		case E_MQTT_STATE_IDLE: mqtt_state_idle(); break;
		case E_MQTT_STATE_TCP_CONNECT: mqtt_state_tcp_connect(); break;
		case E_MQTT_STATE_CONNECT: mqtt_state_connect(); break;

		case E_MQTT_STATE_ERROR:
		case E_MQTT_STATE_SUBSCRIBE:
		case E_MQTT_STATE_POOL_LOCAL_DATA:
		case E_MQTT_STATE_POOL_REMOTE_DATA:
		case E_MQTT_STATE_PING:
		default:
			sprintf(temp, "Invalid state: %d", state);
			logger_log(temp);
	}
}

void mqtt_state_idle(void)
{
	if (substate == E_MQTT_SUBSTATE_SEND)
	{
		logger_log("MQTT is waiting to start");
		substate = E_MQTT_SUBSTATE_WAIT;
	} 
	else if (substate == E_MQTT_SUBSTATE_WAIT)
	{
	}	
}

///////////////////////////////////////////////////////////////////////////
// TCP Connect
///////////////////////////////////////////////////////////////////////////
void mqtt_state_tcp_connect(void)
{
	if (substate == E_MQTT_SUBSTATE_SEND)
	{
		if (mqtt_tcp_server_connect())
		{
			mqtt_fsm_set_state(E_MQTT_STATE_CONNECT);
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
void mqtt_state_connect(void)
{
	if (substate == E_MQTT_SUBSTATE_SEND)
	{
		logger_log("[mqtt] Sending CONNECT message");
		char mqtt_protocol_name[] = "MQTT";
		char mqtt_client_id[] = "fabio";
		mqtt_connect(mqtt_protocol_name, mqtt_client_id);

		substate = E_MQTT_SUBSTATE_WAIT;
	} 
	else if (substate == E_MQTT_SUBSTATE_WAIT)
	{
		logger_log("Waiting response from connect");
	}	
}


	



