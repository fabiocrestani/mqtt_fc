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
#define LOG_PUBLISH (TRUE)
#define LOG_PUBACK (FALSE)
#define LOG_PINGREQ (FALSE)
#define LOG_PINGRESP (FALSE)
#define LOG_SUBSCRIBE (FALSE)
#define LOG_SUBACK (FALSE)

#define LOG_DUMP (TRUE)
#define LOG_DUMP_CONNECT (FALSE)
#define LOG_DUMP_PUBLISH (TRUE)
#define LOG_DUMP_PUBACK (FALSE)
#define LOG_DUMP_PINGREQ (FALSE)
#define LOG_DUMP_SUBSCRIBE (FALSE)
#define LOG_DUMP_SUBACK (FALSE)

// Log
void logger_log(char buffer[]);
void dump(char *data, uint32_t len);

// Log messages with a single function call
void log_connect_message(ConnectMessage connect_message);
void log_connack_message(ConnackMessage connack_message);
void log_publish_message(PublishMessage message);
void log_puback_message(PubAckMessage puback_message);
void log_subscribe_message(SubscribeMessage subscribe_message);
void log_suback_message(SubAckMessage suback_message);
void log_pingreq_message(PingReqMessage pingreq_message);
void log_pingresp_message(PingRespMessage pingresp_message);
void log_pingresp_message(PingRespMessage pingresp_message);

// Translators
const char * translate_message_type(MessageType message_type);
const char * translate_connack_return_code(EConnakReturnCode return_code);
const char * translate_qos_level(EQosLevel qos_level);

#endif // __LOGGER_H__
