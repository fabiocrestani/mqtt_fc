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

#include "mqtt_fc_qos.h"

Timer timer_sensor_dummy;
Mqtt *mqtt_ref;

// Callback to call mqtt and send data when data is ready
uint8_t (*mqtt_add_data_ptr)(Mqtt *, char *, uint32_t, char *, EQosLevel);

void dummy_sensor_set_mqtt_callback(
	uint8_t (*mqtt_callback_ptr)(Mqtt *, char *, uint32_t, char *, EQosLevel)
)
{
	mqtt_add_data_ptr = mqtt_callback_ptr;
}

void dummy_sensor_set_mqtt_reference(Mqtt *new_mqtt_ref)
{
	mqtt_ref = new_mqtt_ref;
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

	// Call mqtt to send data
	if (mqtt_add_data_ptr)
	{
		char data[512];
		sprintf(data, "%0.3f", temperature);
		mqtt_add_data_ptr(mqtt_ref, "topic_temperature_sensor", strlen(data), 
			data, E_QOS_NONE);
	}

}
