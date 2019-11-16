/*
 * mqtt_fc_fsm.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

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
#include "date.h"
#include "circular_message_buffer.h"

char * mqtt_fsm_translate_state(EMqttState state);

void mqtt_state_idle(Mqtt *mqtt);
void mqtt_state_tcp_connect(Mqtt *mqtt);
void mqtt_state_connect(Mqtt *mqtt);
void mqtt_state_connected(Mqtt *mqtt);
void mqtt_state_ping(Mqtt *mqtt);
void mqtt_state_subscribe(Mqtt *mqtt);

void mqtt_fsm_set_state(Mqtt *mqtt, EMqttState new_state)
{
	mqtt->state = new_state;
	mqtt->substate = E_MQTT_SUBSTATE_SEND;
	mqtt->retries = 0;
}

void mqtt_fsm_set_error_state(Mqtt *mqtt, EMqttState origin)
{
	char temp[512];
	sprintf(temp , "FATAL ERROR! Error in MQTT state %s (%d)", 
		mqtt_fsm_translate_state(origin), origin);
	logger_log(temp);

	logger_log("Reseting application in 10 seconds");
	sleep(10);
	mqtt_restart(mqtt);
}

char * mqtt_fsm_translate_state(EMqttState state)
{
	switch (state)
	{
		case E_MQTT_STATE_IDLE: return "E_MQTT_STATE_IDLE";
		case E_MQTT_STATE_TCP_CONNECT: return "E_MQTT_STATE_TCP_CONNECT";
		case E_MQTT_STATE_CONNECT: return "E_MQTT_STATE_CONNECT";
		case E_MQTT_STATE_CONNECTED: return "E_MQTT_STATE_CONNECTED";
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
	sprintf(temp, "State: %s (%d), Substate: %s (%d)", 
		mqtt_fsm_translate_state(mqtt->state), mqtt->state,
		mqtt_fsm_translate_substate(mqtt->substate), mqtt->substate);
	logger_log(temp);
#endif

	switch (mqtt->state)
	{
		case E_MQTT_STATE_IDLE: mqtt_state_idle(mqtt); break;
		case E_MQTT_STATE_TCP_CONNECT: mqtt_state_tcp_connect(mqtt); break;
		case E_MQTT_STATE_CONNECT: mqtt_state_connect(mqtt); break;
		case E_MQTT_STATE_CONNECTED: mqtt_state_connected(mqtt); break;
		case E_MQTT_STATE_PING: mqtt_state_ping(mqtt); break;
		case E_MQTT_STATE_SUBSCRIBE: mqtt_state_subscribe(mqtt); break;

		case E_MQTT_STATE_ERROR:
		case E_MQTT_STATE_POOL_LOCAL_DATA:
		case E_MQTT_STATE_POOL_REMOTE_DATA:
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
		mqtt->retries = 0;
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
			(mqtt->retries)++;
			char temp[512];
			sprintf(temp, "[tcp] Connection error. Retry %d/%d",
				mqtt->retries, TCP_CONNECT_MAX_RETRIES);		
			logger_log(temp);
			if (mqtt->retries >= TCP_CONNECT_MAX_RETRIES)
			{
				logger_log("[tcp] Max retries exceeded.");
				mqtt_fsm_set_error_state(mqtt, E_MQTT_STATE_TCP_CONNECT);
			}
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
		logger_log_mqtt(TYPE_OUTPUT, "Sending CONNECT message");
		char mqtt_protocol_name[] = "MQTT";
		char mqtt_client_id[] = "fabio";
		mqtt_connect(mqtt_protocol_name, mqtt_client_id);

		mqtt->substate = E_MQTT_SUBSTATE_WAIT;
	} 
	else if (mqtt->substate == E_MQTT_SUBSTATE_WAIT)
	{
		if (mqtt->connected)
		{
			logger_log_mqtt(TYPE_INFO, "Connected");
			mqtt_fsm_set_state(mqtt, E_MQTT_STATE_CONNECTED);

			// Publish hello world message
			logger_log_mqtt(TYPE_OUTPUT, "Sending Hello World PUBLISH message");
			char topic_to_publish[] = "topic_1";
			char message_to_publish[512];
			sprintf(message_to_publish, "Hello World in %s (%s)",
				topic_to_publish, get_current_date_time());
			mqtt_publish(topic_to_publish, message_to_publish, E_QOS_PUBACK);
		}
		else
		{
			// TODO add retries counter
			logger_log_mqtt(TYPE_INFO, "Waiting response from connect");
		}
	}	
}

///////////////////////////////////////////////////////////////////////////
// Connected
///////////////////////////////////////////////////////////////////////////
void mqtt_state_connected(Mqtt *mqtt)
{
	char temp[512];

	if (mqtt->substate == E_MQTT_SUBSTATE_SEND)
	{
		// When first connected, subscribe to desired topics
		if (mqtt->is_all_topics_subscribed == FALSE)
		{
			mqtt_fsm_set_state(mqtt, E_MQTT_STATE_SUBSCRIBE);
			return;
		}

		// After subscribing, wait for incoming data
		mqtt->substate = E_MQTT_SUBSTATE_WAIT;
	}

	if (mqtt->substate == E_MQTT_SUBSTATE_WAIT)
	{
		// Check if there is any Publish message incoming
		// If yes, process the data and reset the ping timeout
		if (mqtt->received_publish_counter > 0)
		{
			PublishMessage message;
			if (mqtt_get_last_publish_received_message(mqtt, &message))
			{
				sprintf(temp, "PUBLISH payload is (%d): %s",
					message.payload_len, message.payload);
				logger_log_mqtt(TYPE_INPUT, temp);
				mqtt_reset_ping_timeout(mqtt);
				return;
			}
		}

		// Check if there is any data in the output buffer
		// If yes, send the data and reset the ping timeout
		if (!message_buffer_is_empty(mqtt->circular_message_buffer_tx))
		{
			PublishMessage message = 
				message_buffer_pop(mqtt->circular_message_buffer_tx);
			uint8_t ret = mqtt_send((void *) &message);

			if (ret)
			{
				sprintf(temp, "Sending Publish message to topic %s", 
					message.topic_name);
				logger_log_mqtt(TYPE_OUTPUT, temp);
				mqtt->ping_timeout = 0;
			}
			else
			{
				sprintf(temp, "Error sending Publish message to topic %s", 
					message.topic_name);
				logger_log_mqtt(TYPE_ERROR, temp);
			}
		}

		// Sends ping if there is inactivity
		if ((mqtt->ping_timeout)++ >= MQTT_MAX_PING_TIMEOUT)
		{
			mqtt_fsm_set_state(mqtt, E_MQTT_STATE_PING);
			mqtt->ping_timeout = 0;
		}
	}
}
			
///////////////////////////////////////////////////////////////////////////
// Ping
///////////////////////////////////////////////////////////////////////////
void mqtt_state_ping(Mqtt *mqtt)
{
	if (mqtt->substate == E_MQTT_SUBSTATE_SEND)
	{
		// Sends ping request
		mqtt_ping_request();
		mqtt->pong_received = FALSE;
		mqtt->substate = E_MQTT_SUBSTATE_WAIT;
	} 
	else if (mqtt->substate == E_MQTT_SUBSTATE_WAIT)
	{
		// Waits for a response
		if (mqtt->pong_received == TRUE)
		{
			char temp[512];
			struct timeval tv;
			gettimeofday(&tv, NULL);
			uint32_t ping_stop_us = tv.tv_usec;
			uint32_t ping_elapsed_us = ping_stop_us - mqtt->ping_start_us;

			sprintf(temp, "PINGRESP received after %d us", ping_elapsed_us);
			logger_log_mqtt(TYPE_INPUT, temp);
			mqtt_fsm_set_state(mqtt, E_MQTT_STATE_CONNECTED);
		}
		else
		{
			logger_log_mqtt(TYPE_INFO, "Waiting response from PING");
			(mqtt->retries)++;
			if (mqtt->retries >= MQTT_PING_RESPONSE_MAX_RETRIES)
			{
				char temp[512];
				sprintf(temp, "Ping max retries exceeded (%d).",
					mqtt->retries);
				logger_log_mqtt(TYPE_ERROR, temp);
				mqtt_fsm_set_state(mqtt, E_MQTT_STATE_CONNECTED);
			}
		}
	}	
}

///////////////////////////////////////////////////////////////////////////
// Subscribe
///////////////////////////////////////////////////////////////////////////
void mqtt_state_subscribe(Mqtt *mqtt)
{
	char temp[512];

	if (mqtt->substate == E_MQTT_SUBSTATE_SEND)
	{
		// If there are topics to be subscribed
		if ((mqtt->subscribe_topics_number > 0) && 
				(mqtt->subscribe_topics_subscribed < mqtt->subscribe_topics_number))
		{
			// Send subscribe message
			sprintf(temp, "Sending SUBSCRIBE to topic \"%s\"", 
				mqtt->subscribe_topics[mqtt->subscribe_topics_subscribed]);
			logger_log_mqtt(TYPE_OUTPUT, temp);
			mqtt_subscribe(
				mqtt->subscribe_topics[mqtt->subscribe_topics_subscribed], 1);
			mqtt->wait_for_topic_subscribe = TRUE;
			mqtt->substate = E_MQTT_SUBSTATE_WAIT;
		}
		// All topics subscribed
		else
		{
			sprintf(temp, "All topics subscribed (%d)",
				mqtt->subscribe_topics_subscribed);
			logger_log_mqtt(TYPE_INFO, temp);
			mqtt->is_all_topics_subscribed = TRUE;
			mqtt_fsm_set_state(mqtt, E_MQTT_STATE_CONNECTED);
		}
	}
	// Waiting for subscribe response
	else if (mqtt->substate == E_MQTT_SUBSTATE_WAIT)
	{
		if (mqtt->wait_for_topic_subscribe)
		{
			logger_log_mqtt(TYPE_INFO, "Waiting response from SUBSCRIBE");
			(mqtt->retries)++;
			if (mqtt->retries >= MQTT_SUBSCRIBE_RESPONSE_MAX_RETRIES)
			{
				char temp[512];
				sprintf(temp, "SUBSCRIBE max retries exceeded (%d).",
					mqtt->retries);
				logger_log_mqtt(TYPE_ERROR, temp);
				(mqtt->subscribe_topics_subscribed)++;
				mqtt->wait_for_topic_subscribe = FALSE;
				mqtt_fsm_set_state(mqtt, E_MQTT_STATE_SUBSCRIBE);
			}
		}
		// Topic was subscribed
		else
		{
			(mqtt->subscribe_topics_subscribed)++;
			mqtt->wait_for_topic_subscribe = FALSE;
			mqtt_fsm_set_state(mqtt, E_MQTT_STATE_SUBSCRIBE);
		}
	}
}



