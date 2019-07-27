/*
 * timer.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include "timer.h"

static uint32_t next_timer_id = 0;

void *timer_thread_handler(void *args);

uint8_t timer_init(Timer *timer, uint32_t period_us, uint32_t counter_max)
{
	if (!timer || (counter_max < 1) || (period_us < TIMER_MIN_PERIOD_US))
	{
		return FALSE;
	}

	timer->id = next_timer_id++;
	timer->counter = 0;
	timer->period_us = period_us;
	timer->counter_max = counter_max;

	printf("timer_init Timer initialized with period of %u us (%0.6f s) \n", 
		period_us, (period_us / 1000000.0));

	return TRUE;
}
	
uint8_t timer_start(Timer *timer)
{
	uint32_t ret;

	if (!timer)
	{
		return FALSE;
	}

	ret = pthread_create(&(timer->thread), NULL, timer_thread_handler, 
							(void *) timer);

	if (ret)
	{
		printf("timer_start error - pthread_create() return code: %d\n", ret);
		return FALSE;
	}

	return TRUE;
}

void *timer_thread_handler(void *args)
{
	Timer *timer = (Timer *) args;

	while (1)
	{
#ifdef LOG_TIMER_PROGRESS
		printf("Thread running... Timer %u Period: %u us Counter: %u / %u\n",
			timer->id, timer->period_us, timer->counter, timer->counter_max);
#endif
		usleep(timer->period_us);
		(timer->counter)++;
	}
	
	printf("Thread stopping\n");
	return (void *) 0;
}

uint8_t timer_reached(Timer *timer)
{
	if (timer->counter >= timer->counter_max)
	{
		timer->counter = 0;
		return TRUE;
	}

	return FALSE;
}

uint32_t timer_get_current_tick(Timer *timer)
{
	return timer->counter;
}

