/*
 * mqtt.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_H__
#define __MQTT_H__

#define TRUE (1)
#define FALSE (0)

typedef struct FixedHeader_ {
	union {
		uint8_t byte1;
		struct {
			uint8_t retain : 1;
			uint8_t qos_flags : 2;
			uint8_t dup_flag : 1;
			uint8_t message_type : 4;
		};
	};
	
	union
	{
		uint8_t byte2;
		uint8_t remaining_length;
	};

} FixedHeader;

#endif // __MQTT_H__
