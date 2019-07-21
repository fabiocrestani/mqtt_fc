/*
 * circular_buffer.h
 * Author: Fabio Crestani
 */

#ifndef __CIRCULARBUFFER_H__
#define __CIRCULARBUFFER_H__

#include <stdint.h>

#define BUFFER_SIZE 1024*2

// #define DEBUG_CIRCULAR_BUFFER

typedef struct _circularBuffer {
	uint16_t    idxProd;
	uint16_t    idxCons;
	uint16_t    numEl;
	char     	data[BUFFER_SIZE];
} CircularBuffer;

void buffer_init(CircularBuffer *buf);
void buffer_put(CircularBuffer *buf, char newEl);
void buffer_put_array(CircularBuffer *buf, char *input, uint32_t len);
char buffer_pop(CircularBuffer *buf);
uint32_t buffer_pop_array(CircularBuffer *buf, char *output);
uint8_t buffer_is_empty(CircularBuffer *buf);
uint8_t buffer_has_room_for(CircularBuffer *buf, uint32_t len);

#endif
