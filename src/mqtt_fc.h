/*
 * mqtt.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_H__
#define __MQTT_H__

#define TRUE (1)
#define FALSE (0)

#define CONNECT_PROTOCOL_NAME_MAX_LEN (255)
#define CONNECT_PAYLOAD_MAX_LEN (255)
#define PUBLISH_TOPIC_NAME_MAX_LEN (255)

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

typedef struct ConnectMessage_ {
	FixedHeader header;

	union {
		uint16_t length;
		struct {
			uint8_t length_lsb;
			uint8_t length_msb;
		};
	};

	uint8_t protocol_name[CONNECT_PROTOCOL_NAME_MAX_LEN];
	uint8_t version;
	uint8_t flags;

	union {
		uint16_t keep_alive_timer;
		struct {
			uint8_t keep_alive_timer_lsb;
			uint8_t keep_alive_timer_msb;
		};
	};

	uint8_t payload[CONNECT_PAYLOAD_MAX_LEN];
} ConnectMessage;

typedef struct PublishMessage_ {
	FixedHeader header;
	uint8_t topic_name[PUBLISH_TOPIC_NAME_MAX_LEN];

	union {
		uint8_t message_id[2];
		struct {
			uint8_t message_id_lsb;
			uint8_t	message_id_msb;
		};
	};
} PublishMessage;

#endif // __MQTT_H__
