/*
 * pi_cpu_temperature.h
 * Author: Fabio Crestani
 */

#ifndef __PI_CPU_TEMPERATURE_H__
#define __PI_CPU_TEMPERATURE_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "timer.h"
#include "logger.h"
#include "utils.h"
#include "circular_buffer.h"

extern Timer timer_pi_cpu_temperature;

void pi_cpu_temperature_set_mqtt_reference(Mqtt *new_mqtt_ref);
void pi_cpu_temperature_set_mqtt_callback(
	uint8_t (*mqtt_callback_ptr)(Mqtt *, char *, uint32_t, char *, EQosLevel));

void pi_cpu_temperature_poll(void);

#endif // __PI_CPU_TEMPERATURE_H__
