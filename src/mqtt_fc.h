/*
 * mqtt_fc.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_H__
#define __MQTT_H__

#define TRUE (1)
#define FALSE (0)

#define MQTT_VERSION (4)

#define KEEP_ALIVE_TIMER_VALUE (60)

#define CONNECT_PROTOCOL_NAME_MAX_LEN (255)
#define CONNECT_CLIENT_ID_MAX_LEN (23)

#define CONNACK_MESSAGE_SIZE (4)

#define PUBLISH_TOPIC_NAME_MAX_LEN (255)
#define PUBLISH_PAYLOAD_MAX_LEN (255)

#define PUBACK_MESSAGE_SIZE (4)

///////////////////////////////////////////////////////////////////////////////
// ENUMS
///////////////////////////////////////////////////////////////////////////////
typedef enum MessageType_ {
	CONNECT = 0x1,
	CONNACK = 0x2,
	PUBLISH = 0x3,
	PUBACK = 0x4,
	PBUREC = 0x5,
	PUBREL = 0x6,
	PUBCOMP = 0x7,
	SUBSCRIBE = 0x8,
	SUBACK = 0x9,
	UNSUBSCRIBE = 0xA,
	UNSUBACK = 0xB,
	PINGREQ = 0xC,
	PINGRESP = 0xD,
	DISCONNECT = 0xE
} MessageType;

typedef enum EQosLevel_ {
	E_QOS_NONE = 0,
	E_QOS_PUBACK = 1,
	E_QOS_PUBREC = 2
} EQosLevel;

typedef enum {
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
			MessageType message_type : 4;
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

	uint8_t topic_name[PUBLISH_TOPIC_NAME_MAX_LEN];

	union {
		uint16_t message_id;
		struct {
			uint8_t message_id_lsb;
			uint8_t	message_id_msb;
		};	
	};

	uint8_t payload[PUBLISH_PAYLOAD_MAX_LEN];
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

///////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////////
uint32_t mqtt_pack_publish_message(PublishMessage message, char *buffer);
uint32_t mqtt_pack_connect_message(ConnectMessage message, char *buffer);
uint32_t mqtt_pack_puback_message(PubAckMessage message, char *buffer);


#endif // __MQTT_H__
