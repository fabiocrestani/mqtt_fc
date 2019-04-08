/*
 * timer.h
 * Author: Fabio Crestani
 */

#ifndef __TIMER_H__
#define __ TIMER_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define TRUE (1)
#define FALSE (0)
#define TIMER_MIN_PERIOD_US (10)

typedef struct Timer_
{
	pthread_t thread;
	uint32_t id;
	uint32_t counter;
	uint32_t period_us;
	uint32_t counter_max;
} Timer;

uint8_t timer_init(Timer *timer, uint32_t period_us, uint32_t counter_max);
uint8_t timer_start(Timer *timer);

#endif // __TIMER_H__


