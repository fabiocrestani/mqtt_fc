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

// TODO Remove
#include "mqtt/mqtt_fc.h"

Timer timer_sensor_dummy;

uint8_t (*mqtt_add_publish_message_to_queue_ptr)(PublishMessage);

void dummy_sensor_set_mqtt_callback(uint8_t (*mqtt_callback_ptr)(PublishMessage))
{
	mqtt_add_publish_message_to_queue_ptr = mqtt_callback_ptr;
}

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
	sprintf(temp, "Temperature is %0.3f °C", temperature);
	logger_log2("sensor_dummy", temp, COLOR_MAGENTA);

	if (mqtt_add_publish_message_to_queue_ptr)
	{
		PublishMessage message;
		mqtt_add_publish_message_to_queue_ptr(message);
	}

}
