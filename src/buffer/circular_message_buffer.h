/*
 * circular_message_buffer.h
 * Author: Fabio Crestani
 */

#ifndef __CIRCULAR_MESSAGE_BUFFER_H__
#define __CIRCULAR_MESSAGE_BUFFER_H__

#include <stdint.h>

#include "mqtt_fc.h"

#define MESSAGE_BUFFER_SIZE 64

typedef struct PublishMessage_ PublishMessage;

// #define DEBUG_CIRCULAR_MESSAGE_BUFFER

typedef struct _circularMessageBuffer {
	uint16_t idxProd;
	uint16_t idxCons;
	uint16_t numEl;
	PublishMessage data[MESSAGE_BUFFER_SIZE];

} CircularMessageBuffer;

void message_buffer_init(CircularMessageBuffer *buf);
void message_buffer_put(CircularMessageBuffer *buf, PublishMessage newEl);
PublishMessage message_buffer_pop(CircularMessageBuffer *buf);
uint8_t message_buffer_is_empty(CircularMessageBuffer *buf);
uint8_t message_buffer_has_room_for(CircularMessageBuffer *buf, uint32_t len);

#endif // __CIRCULAR_MESSAGE_BUFFER_H__
