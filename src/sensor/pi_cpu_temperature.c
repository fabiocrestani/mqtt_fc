/*
 * pi_cpu_temperature.c
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

#include "pi_cpu_temperature.h"
#include "timer.h"
#include "logger.h"
#include "utils.h"
#include "circular_buffer.h"

#include "mqtt_fc_qos.h"

Timer timer_pi_cpu_temperature;
Mqtt *mqtt_ref;

// Callback to call mqtt and send data when data is ready
uint8_t (*mqtt_add_data_ptr)(Mqtt *, char *, uint32_t, char *, EQosLevel);

void pi_cpu_temperature_set_mqtt_callback(
	uint8_t (*mqtt_callback_ptr)(Mqtt *, char *, uint32_t, char *, EQosLevel)
)
{
	mqtt_add_data_ptr = mqtt_callback_ptr;
}

void pi_cpu_temperature_set_mqtt_reference(Mqtt *new_mqtt_ref)
{
	mqtt_ref = new_mqtt_ref;
}

void pi_cpu_temperature_poll(void)
{
	if (!timer_reached(&timer_pi_cpu_temperature))
	{
		return;
	}

	FILE* f;
	f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	if (f == NULL)
	{
		logger_log_mqtt(TYPE_ERROR, "Error opening temperature file to read");
		return;
	}

	// Read from file and convert to float
	char buff[32];
	fgets(buff, 32, f);
	int temp_int = atoi(buff);
	float temperature = temp_int / 10000.0;
	
	// Log
	char temp[512];
	sprintf(temp, "Temperature is %0.3f °C", temperature);
	logger_log2("pi_cpu_temperature", temp, COLOR_MAGENTA);

	// Call mqtt to send data
	if (mqtt_add_data_ptr)
	{
		char data[512];
		sprintf(data, "{\"cpu_temperature\": %0.3f}", temperature);
		mqtt_add_data_ptr(
			mqtt_ref,
			"house/measurements/pi",
			strlen(data), 
			data, E_QOS_NONE
		); //E_QOS_PUBACK);
	}

}
