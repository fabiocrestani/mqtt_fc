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

void dump_fixed_header(uint8_t byte1, uint8_t byte2);
void dump(char *data, uint32_t len);

#endif // __LOGGER_H__
