/*
 * mqtt_fc.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_H__
#define __MQTT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "timer.h"
#include "mqtt_fc_fsm.h"
#include "mqtt_fc_qos.h"

extern Timer timer_mqtt_fsm;

#define TRUE (1)
#define FALSE (0)

#define MQTT_VERSION (4)
#define MQTT_TOPIC_NAME_MAX_LEN (255)
#define MQTT_SUBSCRIBE_TOPIC_NUM_MAX (128)

#define KEEP_ALIVE_TIMER_VALUE (60)

// Fields size
#define CONNECT_PROTOCOL_NAME_MAX_LEN (255)
#define CONNECT_CLIENT_ID_MAX_LEN (23)
#define CONNACK_MESSAGE_SIZE (4)
#define PUBLISH_PAYLOAD_MAX_LEN (255)
#define PUBACK_MESSAGE_SIZE (4)
#define PINGRESP_MESSAGE_SIZE (2)
#define SUBACK_MIN_MESSAGE_SIZE (5)

// Receive Publish messages
#define RECEIVED_PUBLISH_MIN_MESSAGE_SIZE (7)
#define MQTT_RECEIVED_PUBLISH_QUEUE_SIZE (32)

// Queue for sending Publish messages
#define MQTT_OUTPUT_PUBLISH_QUEUE_SIZE (64)

// Retries
#define TCP_CONNECT_MAX_RETRIES (5)
#define MQTT_MAX_PING_TIMEOUT (20)
#define MQTT_PING_RESPONSE_MAX_RETRIES (5)
#define MQTT_SUBSCRIBE_RESPONSE_MAX_RETRIES (5)

///////////////////////////////////////////////////////////////////////////////
// ENUMS
///////////////////////////////////////////////////////////////////////////////
typedef enum EMessageType_ {
	CONNECT = 0x1,
	CONNACK = 0x2,
	PUBLISH = 0x3,
	PUBACK = 0x4,
	PUBREC = 0x5,
	PUBREL = 0x6,
	PUBCOMP = 0x7,
	SUBSCRIBE = 0x8,
	SUBACK = 0x9,
	UNSUBSCRIBE = 0xA,
	UNSUBACK = 0xB,
	PINGREQ = 0xC,
	PINGRESP = 0xD,
	DISCONNECT = 0xE,
	MESSAGE_TYPE_COUNT
} EMessageType;

typedef enum EConnakReturnCode_ {
	E_CONNACK_CONNECTION_ACCEPTED = 0x0,
	E_CONNACK_UNACCEPTABLE_PROTOCOL_VERSION = 0x01,
	E_CONNACK_IDENTIFIER_REJECTED = 0x02,
	E_CONNACK_SERVER_UNAVAILABLE = 0x03,
	E_CONNACK_BAD_USER_NAME_OR_PASSWORD = 0x04,
	E_CONNACK_NOT_AUTHORIZED = 0x05,
} EConnakReturnCode;

///////////////////////////////////////////////////////////////////////////////
// STRUCTS
///////////////////////////////////////////////////////////////////////////////
typedef struct FixedHeader_ {
	union {
		uint8_t byte1;
		struct {
			uint8_t retain : 1;
			uint8_t qos : 2;
			uint8_t dup : 1;
			uint8_t message_type : 4;
		};
	};
	
	union
	{
		uint8_t byte2;
		uint8_t remaining_length;
	};
} FixedHeader;

// When a TCP/IP socket connection is established from a client to a server, 
// a protocol level session must be created using a CONNECT flow.
typedef struct ConnectMessage_ {
	FixedHeader header;

	union {
		uint16_t protocol_name_len;
		struct {
			uint8_t protocol_name_len_lsb;
			uint8_t protocol_name_len_msb;
		};
	};

	uint8_t protocol_name[CONNECT_PROTOCOL_NAME_MAX_LEN];
	uint8_t version;

	union {
		uint8_t flags;
		struct {
			uint8_t reserved_flag : 1;	
			uint8_t clean_session_flag : 1;
			uint8_t will_flag : 1;
			uint8_t will_qos_flag : 2;
			uint8_t will_retain_flag : 1;
			uint8_t password_flag : 1;
			uint8_t user_name_flag : 1;
		};
	};

	union {
		uint16_t keep_alive_timer;
		struct {
			uint8_t keep_alive_timer_lsb;
			uint8_t keep_alive_timer_msb;
		};
	};

	union
	{
		uint16_t client_id_len;
		struct {
			uint8_t client_id_len_lsb;
			uint8_t client_id_len_msb;
		};
	};

	uint8_t client_id[CONNECT_CLIENT_ID_MAX_LEN];
} ConnectMessage;

// The CONNACK message is the message sent by the server in response to a 
// CONNECT request from a client.
typedef struct ConnackMessage_ {
	FixedHeader header;
	union {
		struct {
			uint8_t byte1;
			uint8_t return_code;
		};
		uint8_t message[2];
	};
} ConnackMessage;

// A PUBLISH message is sent by a client to a server for distribution to 
// interested subscribers. Each PUBLISH message is associated with a topic name
// (also known as the Subject or Channel). This is a hierarchical name space 
// that defines a taxonomy of information sources for which subscribers can 
// register an interest. A message that is published to a specific topic name 
// is delivered to connected subscribers for that topic. If a client subscribes 
// to one or more topics, any message published to those topics are sent by the
// server to the client as a PUBLISH message.
typedef struct PublishMessage_ {
   FixedHeader header;

   union {
       uint16_t topic_name_len;
       struct {
               uint8_t topic_name_len_lsb;
               uint8_t topic_name_len_msb;
       };      
   };

   uint8_t topic_name[MQTT_TOPIC_NAME_MAX_LEN];

   union {
       uint16_t message_id;
       struct {
               uint8_t message_id_lsb;
               uint8_t message_id_msb;
       };      
   };

   uint8_t payload[PUBLISH_PAYLOAD_MAX_LEN];
   uint32_t payload_len;
} PublishMessage;

// A PUBACK message is the response to a PUBLISH message with QoS level 1. 
// A PUBACK message is sent by a server in response to a PUBLISH message from a 
// publishing client, and by a subscriber in response to a PUBLISH message from 
// the server.
typedef struct PubAckMessage_ {
	FixedHeader header;

	union {
		uint16_t message_id;
		struct {
			uint8_t message_id_lsb;
			uint8_t	message_id_msb;
		};	
	};

} PubAckMessage;

// The PINGREQ message is an "are you alive?" message that is sent from a 
// connected client to the server
typedef struct PingReqMessage_ {
	FixedHeader header;

} PingReqMessage;

// A PINGRESP message is the response sent by a server to a PINGREQ message and
// means "yes I am alive".
typedef struct PingRespMessage_ {
	FixedHeader header;

} PingRespMessage;

// The SUBSCRIBE message allows a client to register an interest in one or more
// topic names with the server. Messages published to these topics are delivered
// from the server to the client as PUBLISH messages. The SUBSCRIBE message also
// specifies the QoS level at which the subscriber wants to receive published
// messages.
typedef struct SubscribeMessage_ {
	FixedHeader header;
	
	union {
		uint16_t message_id;
		struct {
			uint8_t message_id_lsb;
			uint8_t	message_id_msb;
		};	
	};	

	union {
		uint16_t topic_name_len;
		struct {
			uint8_t topic_name_len_lsb;
			uint8_t topic_name_len_msb;
		};	
	};

	uint8_t topic_name[MQTT_TOPIC_NAME_MAX_LEN];

	uint8_t requested_qos;

} SubscribeMessage;

// A SUBACK message is sent by the server to the client to confirm receipt of a
// SUBSCRIBE message. A SUBACK message contains a list of granted QoS levels. 
// The order of granted QoS levels in the SUBACK message matches the order of 
// the topic names in the corresponding SUBSCRIBE message.
typedef struct SubAckMessage_ {
	FixedHeader header;
	
	union {
		uint16_t message_id;
		struct {
			uint8_t message_id_lsb;
			uint8_t	message_id_msb;
		};	
	};

	uint8_t granted_qos;

} SubAckMessage;

// The DISCONNECT message is sent from the client to the server to indicate 
// that it is about to close its TCP/IP connection. This allows for a clean
// disconnection, rather than just dropping the line. 
// If the client had connected with the clean session flag set, then all 
// previously maintained information about the client will be discarded.
// A server should not rely on the client to close the TCP/IP connection after
// receiving a DISCONNECT.
typedef struct DisconnectMessage_ {
	FixedHeader header;

} DisconnectMessage;

// Main struct
typedef struct Mqtt_ {
	// Buffers
	CircularBuffer *circular_buffer_rx;
	PublishMessage publish_message_queue[MQTT_OUTPUT_PUBLISH_QUEUE_SIZE];

	// Server address and port
	char server_address[512];
	uint32_t server_port;

	// State machine
	EMqttState state;
	EMqttSubstate substate;
	uint32_t retries;
	uint32_t ping_timeout;

	// Ping start timestamp in microseconds
	uint32_t ping_start_us;
	
	// Flags
	uint8_t connected;
	uint8_t pong_received;
	uint8_t wait_for_topic_subscribe;
	uint8_t is_all_topics_subscribed;

	// Subscribe topics
	char subscribe_topics[MQTT_SUBSCRIBE_TOPIC_NUM_MAX][MQTT_TOPIC_NAME_MAX_LEN];
	uint8_t subscribe_topics_number;
	uint8_t subscribe_topics_subscribed;

	// Received Publish messages
	PublishMessage received_publish_message_queue[MQTT_RECEIVED_PUBLISH_QUEUE_SIZE];
	uint32_t received_publish_counter;

	// Sending Publish messages
	uint32_t publish_message_queue_index;

} Mqtt;


///////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////////
void error(char *msg);
void mqtt_init(void);
void mqtt_start(Mqtt *mqtt);
void mqtt_restart(Mqtt *mqtt);
Mqtt * mqtt_get_instance(void);
void mqtt_reset_ping_timeout(Mqtt *mqtt);
void mqtt_set_subscribe_topics(Mqtt *mqtt, 
	char topics[][MQTT_TOPIC_NAME_MAX_LEN], uint32_t num_topics);

// Commands
uint8_t	mqtt_connect(char mqtt_protocol_name[], char mqtt_client_id[]);
uint8_t mqtt_publish(char topic_to_publish[], char message_to_publish[],
						EQosLevel qos);
uint8_t mqtt_ping_request(void);
uint8_t mqtt_subscribe(char topic_to_subscribe[], uint8_t requested_qos);

// Response for remote Publish messages
uint8_t	mqtt_send_response_to_publish_message(PublishMessage publish_message);
uint8_t mqtt_get_last_publish_received_message(Mqtt *mqtt, 
	PublishMessage *message);


#endif // __MQTT_H__
