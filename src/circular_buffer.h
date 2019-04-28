/*
 * circular_buffer.h
 * Author: Fabio Crestani
 */

#ifndef __CIRCULARBUFFER_H__
#define __CIRCULARBUFFER_H__

#include <stdint.h>

#define BUFFER_SIZE 1024*2

typedef struct _circularBuffer {
	uint16_t    idxProd;
	uint16_t    idxCons;
	uint16_t    numEl;
	uint8_t     data[BUFFER_SIZE];
} CircularBuffer;

void buffer_init(CircularBuffer *buf);
void buffer_put(CircularBuffer *buf, uint8_t newEl);
uint8_t buffer_pop(CircularBuffer *buf);
uint8_t buffer_is_empty(CircularBuffer *buf);
uint8_t buffer_has_room_for(CircularBuffer *buf, uint32_t len);

#endif
