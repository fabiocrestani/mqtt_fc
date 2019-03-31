/*
 * mqtt_fc_send_receive.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_FC_SEND_RECEIVE_H__
#define __MQTT_FC_SEND_RECEIVE_H__

#include "mqtt_fc.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"

uint8_t mqtt_send(void * message);

#endif // __MQTT_FC_SEND_RECEIVE_H__
