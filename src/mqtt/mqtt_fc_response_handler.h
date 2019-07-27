/*
 * mqtt_fc_response_handler.h
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "mqtt_fc.h"
#include "mqtt_fc_pack.h"
#include "mqtt_fc_unpack.h"
#include "mqtt_fc_build.h"
#include "mqtt_fc_send_receive.h"
#include "mqtt_fc_fsm.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"

// The CONNACK message is the message sent by the server in response to a 
// CONNECT request from a client.
uint8_t mqtt_handle_received_connack(char *buffer, uint32_t len);

// A PUBACK message is the response to a PUBLISH message with QoS level 1. 
// A PUBACK message is sent by a server in response to a PUBLISH message from a 
// publishing client, and by a subscriber in response to a PUBLISH message from
// the server.
uint8_t mqtt_handle_received_puback(char *buffer, uint32_t len);

// A PINGRESP message is the response sent by a server to a PINGREQ message and
// means "yes I am alive".
uint8_t mqtt_handle_received_pingresp(char *buffer, uint32_t len);

// A SUBACK message is sent by the server to the client to confirm receipt of a
// SUBSCRIBE message. A SUBACK message contains a list of granted QoS levels. 
// The order of granted QoS levels in the SUBACK message matches the order of 
// the topic names in the corresponding SUBSCRIBE message.
uint8_t mqtt_handle_received_suback(char *buffer, uint32_t len);

// A PUBLISH message is sent by a client to a server for distribution to 
// interested subscribers. Each PUBLISH message is associated with a topic name
// (also known as the Subject or Channel). This is a hierarchical name space
// that defines a taxonomy of information sources for which subscribers can 
// register an interest. A message that is published to a specific topic name is
// delivered to connected subscribers for that topic. If a client subscribes to 
// one or more topics, any message published to those topics are sent by the
// server to the client as a PUBLISH message.
uint8_t mqtt_handle_received_publish(char *buffer, uint32_t len);

