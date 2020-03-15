/*
 * logger.h
 * Author: Fabio Crestani
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mqtt_fc.h"

typedef enum EMessageType_ EMessageType;
typedef enum EQosLevel_ EQosLevel;
typedef struct PublishMessage_ PublishMessage;
typedef enum EConnakReturnCode_ EConnakReturnCode;

#define LOG_CONNECT (FALSE)
#define LOG_CONNACK (FALSE)
#define LOG_PUBLISH (FALSE)
#define LOG_PUBACK (FALSE)
#define LOG_PINGREQ (FALSE)
#define LOG_PINGRESP (FALSE)
#define LOG_SUBSCRIBE (FALSE)
#define LOG_SUBACK (FALSE)

#define LOG_PUBLISH_PAYLOAD (FALSE)

#define LOG_CONFIGURATION_FILE_PARSER (FALSE)

#define LOG_DUMP (FALSE)
#define LOG_DUMP_CONNECT (FALSE)
#define LOG_DUMP_PUBLISH (FALSE)
#define LOG_DUMP_PUBACK (FALSE)
#define LOG_DUMP_PINGREQ (FALSE)
#define LOG_DUMP_SUBSCRIBE (FALSE)
#define LOG_DUMP_SUBACK (FALSE)

#define LOG_COLORED (TRUE)

// For debugging only
//#define LOG_MQTT_FSM_POLL
//#define LOG_TIMER_PROGRESS

typedef enum ELogColor_ {
	COLOR_RESET,
	COLOR_RED,
	COLOR_RED_BOLD,
	COLOR_GREEN,
	COLOR_GREEN_BOLD,
	COLOR_YELLOW,
	COLOR_YELLOW_BOLD,
	COLOR_BLUE,
	COLOR_BLUE_BOLD,
	COLOR_MAGENTA,
	COLOR_MAGENTA_BOLD,
	COLOR_CYAN,
	COLOR_CYAN_BOLD
} ELogColor;

typedef enum ELogType_ {
	TYPE_OUTPUT,
	TYPE_INPUT,
	TYPE_INFO,
	TYPE_ERROR,
	TYPE_OK
} ELogType;

// Log
void logger_log(char buffer[]);
void logger_log2(char header[], char text[], ELogColor color);
void logger_err(char buffer[]);
void dump(char *data, uint32_t len);

// Special formated logger helpers
void logger_log_tcp(ELogType type, char buffer[]);
void logger_log_mqtt(ELogType type, char buffer[]);

// Log messages with a single function call
void log_message(void * message);
void log_publish_message_payload(PublishMessage *publish_message);

// Translators
const char * translate_message_type(EMessageType message_type);
const char * translate_connack_return_code(EConnakReturnCode return_code);
const char * translate_qos_level(EQosLevel qos_level);

#endif // __LOGGER_H__
