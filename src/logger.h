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

#define LOG_DUMP (TRUE)
#define LOG_DUMP_CONNECT (FALSE)
#define LOG_DUMP_PUBLISH (TRUE)
#define LOG_DUMP_PUBACK (TRUE)

// Parsed data dump
void dump_parsed_fixed_header(FixedHeader header);
void dump_parsed_connect_message(ConnectMessage mesage);
void dump_parsed_connack_message(ConnackMessage connack_message);

// Raw data dump
void dump_connect_message(ConnectMessage message);
void dump_publish_message(PublishMessage message);
void dump(char *data, uint32_t len);

// Log messages with a single function call
void log_connect_message(ConnectMessage connect_message);
void log_connack_message(ConnackMessage connack_message);
void log_publish_message(PublishMessage message);
void log_puback_message(PubAckMessage puback_message);

#endif // __LOGGER_H__
