/*
 * sensor_dummy.h
 * Author: Fabio Crestani
 */

#ifndef __SENSOR_DUMMY_H__
#define __SENSOR_DUMMY_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "timer.h"
#include "logger.h"
#include "utils.h"
#include "circular_buffer.h"

extern Timer timer_sensor_dummy;

void dummy_sensor_set_mqtt_callback(uint8_t (*mqtt_callback_ptr)(PublishMessage));

void dummy_sensor_poll(void);

#endif // __SENSOR_DUMMY_H__
