/*
 * circular_buffer.c
 * Author: Fabio Crestani
 */

#include <stdio.h>

#include "circular_buffer.h"

void buffer_init(CircularBuffer *buf)
{
    buf->numEl = 0;
    buf->idxProd = 0;
    buf->idxCons = 0;
}

void buffer_put(CircularBuffer *buf, char newEl)
{
	if (buf->numEl < BUFFER_SIZE)
	{
		buf->data[buf->idxProd++] = newEl;
		buf->numEl++;
		if (buf->idxProd >= BUFFER_SIZE)
			buf->idxProd = 0;
	}
}

void buffer_put_array(CircularBuffer *buf, char *input, uint32_t len)
{
#ifdef DEBUG_CIRCULAR_BUFFER
	printf("[buffer] Trying to insert %d elements into circular buffer\n", len);
#endif
	if ((buf->numEl + len) < BUFFER_SIZE)
	{
		for (uint32_t i = 0; i < len; i++)
		{
			buf->data[buf->idxProd++] = input[i];
			buf->numEl++;
			if (buf->idxProd >= BUFFER_SIZE)
			{
				buf->idxProd = 0;
			}
		}
#ifdef DEBUG_CIRCULAR_BUFFER
		printf("[buffer] %d elements added into circular buffer\n", len);
#endif
	}
}

char buffer_pop(CircularBuffer *buf)
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

uint32_t buffer_pop_array(CircularBuffer *buf, char *output)
{
	uint32_t counter = 0;

	while (buf->numEl > 0)
	{
		output[counter++] = buffer_pop(buf);		
	}

	return counter;
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




