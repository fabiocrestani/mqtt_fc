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

void mqtt_poll(Mqtt *mqtt);
void mqtt_set_circular_buffer_rx(Mqtt *mqtt, CircularBuffer *ptr_buffer);
void mqtt_set_circular_message_buffer_tx(Mqtt *mqtt, 
	CircularMessageBuffer *ptr_buffer);

uint8_t mqtt_send(void * message);

uint8_t mqtt_handle_received_message(char *buffer, uint32_t len);

uint8_t mqtt_handle_received_connack(char *buffer, uint32_t len);
uint8_t mqtt_handle_received_puback(char *buffer, uint32_t len);

#endif // __MQTT_FC_SEND_RECEIVE_H__
