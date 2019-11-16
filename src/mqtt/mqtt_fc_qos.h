/*
 * mqtt_fc_qos.h
 * Author: Fabio Crestani
 */

#ifndef __MQTT_QOS_H__
#define __MQTT_QOS_H__

typedef enum EQosLevel_ {
	E_QOS_NONE = 0,
	E_QOS_PUBACK = 1,
	E_QOS_PUBREC = 2
} EQosLevel;

#endif // __MQTT_QOS_H__
