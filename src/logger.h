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


void dump_parsed_fixed_header(FixedHeader header);
void dump_parsed_connect_message(ConnectMessage mesage);
void dump_connect_message(ConnectMessage message);
void dump(char *data, uint32_t len);

#endif // __LOGGER_H__
