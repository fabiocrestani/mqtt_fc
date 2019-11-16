/*
 * circular_message_buffer.c
 * Author: Fabio Crestani
 */

#include <stdio.h>

#include "circular_message_buffer.h"

void message_buffer_init(CircularMessageBuffer *buf)
{
    buf->numEl = 0;
    buf->idxProd = 0;
    buf->idxCons = 0;
}

void message_buffer_put(CircularMessageBuffer *buf, PublishMessage newEl)
{
	if (buf->numEl < MESSAGE_BUFFER_SIZE)
	{
		buf->data[buf->idxProd++] = newEl;
		buf->numEl++;
		if (buf->idxProd >= MESSAGE_BUFFER_SIZE)
		{
			buf->idxProd = 0;
		}
	}
}

PublishMessage message_buffer_pop(CircularMessageBuffer *buf)
{
    PublishMessage ret;

	if (buf->numEl)
	{
		ret = buf->data[buf->idxCons++];
		buf->numEl--;
		if (buf->idxCons >= MESSAGE_BUFFER_SIZE)
		{
			buf->idxCons = 0;
		}
	}
    return ret;
}

uint8_t message_buffer_is_empty(CircularMessageBuffer *buf)
{
	uint8_t isEmpty = 0;
	if (buf->numEl == 0)
	{
		isEmpty = 1;
	}
	return isEmpty;
}

uint8_t message_buffer_has_room_for(CircularMessageBuffer *buf, uint32_t len)
{
	return ((buf->numEl + len) < MESSAGE_BUFFER_SIZE);
}

