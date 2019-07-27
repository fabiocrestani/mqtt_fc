/*
 * sensor_dummy.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "sensor_dummy.h"
#include "timer.h"
#include "logger.h"
#include "utils.h"
#include "circular_buffer.h"

Timer timer_sensor_dummy;;

void dummy_sensor_poll(void)
{
	if (!timer_reached(&timer_sensor_dummy))
	{
		return;
	}

	time_t t;
	time(&t);
	struct tm *tm = localtime(&t);
	float noise = 0;
	float temperature = 0;

	noise = (rand() % 1000);
	noise = noise / 1000000;
	noise = noise*tm->tm_min;
	temperature = (22 - pow((float) tm->tm_hour - 12, 2.0)*0.1) + noise;
	
	char temp[512];
	sprintf(temp, "[sensor_dummy] Temperature is %0.3f °C", temperature);
	logger_log(temp);
}
