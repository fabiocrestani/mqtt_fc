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

#define LOG_CONNECT (FALSE)
#define LOG_CONNACK (FALSE)
#define LOG_PUBLISH (FALSE)
#define LOG_PUBACK (FALSE)
#define LOG_PINGREQ (FALSE)
#define LOG_PINGRESP (FALSE)
#define LOG_SUBSCRIBE (TRUE)

#define LOG_DUMP (TRUE)
#define LOG_DUMP_CONNECT (FALSE)
#define LOG_DUMP_PUBLISH (FALSE)
#define LOG_DUMP_PUBACK (FALSE)
#define LOG_DUMP_PINGREQ (FALSE)
#define LOG_DUMP_SUBSCRIBE (TRUE)

// Log
void logger_log(char buffer[]);

// Parsed data dump
/*void dump_parsed_fixed_header(FixedHeader header);
void dump_parsed_connect_message(ConnectMessage mesage);
void dump_parsed_connack_message(ConnackMessage connack_message);

// Raw data dump
void dump_connect_message(ConnectMessage message);
void dump_publish_message(PublishMessage message);
void dump(char *data, uint32_t len);
*/

// Log messages with a single function call
void log_connect_message(ConnectMessage connect_message);
void log_connack_message(ConnackMessage connack_message);
void log_publish_message(PublishMessage message);
void log_subscribe_message(SubscribeMessage subscribe_message);
void log_puback_message(PubAckMessage puback_message);
void log_pingreq_message(PingReqMessage pingreq_message);
void log_pingresp_message(PingRespMessage pingresp_message);

// Translators
const char * translate_message_type(MessageType message_type);
const char * translate_connack_return_code(EConnakReturnCode return_code);
const char * translate_qos_level(EQosLevel qos_level);

#endif // __LOGGER_H__
