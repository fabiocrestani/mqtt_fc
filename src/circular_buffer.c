/*
 * circular_buffer.c
 * Author: Fabio Crestani
 */

#include "circular_buffer.h"

void buffer_init(CircularBuffer *buf)
{
    buf->numEl = 0;
    buf->idxProd = 0;
    buf->idxCons = 0;
}

void buffer_put(CircularBuffer *buf, uint8_t newEl)
{
	if (buf->numEl < BUFFER_SIZE)
	{
		buf->data[buf->idxProd++] = newEl;
		buf->numEl++;
		if (buf->idxProd >= BUFFER_SIZE)
			buf->idxProd = 0;
	}
}

uint8_t buffer_pop(CircularBuffer *buf)
{
    uint8_t ret = 0;
	if( buf->numEl )
	{
		ret = buf->data[buf->idxCons++];
		buf->numEl--;
		if( buf->idxCons >= BUFFER_SIZE )
			buf->idxCons = 0;
	}
    return ret;
}

uint8_t buffer_is_empty(CircularBuffer *buf)
{
	uint8_t isEmpty = 0;
	if (buf->numEl == 0)
	{
		isEmpty = 1;
	}
	return isEmpty;
}

uint8_t buffer_has_room_for(CircularBuffer *buf, uint32_t len)
{
	return ((buf->numEl + len) < BUFFER_SIZE);
}




