# MQTT_FC

I have implemented a MQTT client for no specific reason. It was developed in a Linux environment but could be easily ported to a microcontroller. The code has less than 3000 lines of code and currently does not implement 100% of the protocol

## Compilation
~~~bash
./configure
cd src
make
~~~

## Usage

~~~bash
build/mqtt_fc [mqtt broker address] [mqtt broker tcp port number]
~~~

## Attaching a data producer

Follow the example of the *dummy sensor* in main.c and set two references to the data producer: a reference to *mqtt and a reference to a function (callback to be called when the producer needs to send data to the mqtt output queue):

~~~c
dummy_sensor_set_mqtt_reference(mqtt);
dummy_sensor_set_mqtt_callback(mqtt_publish_handler_add_data_to_queue);
~~~

In the producer (see *sensor_dummy.c*), call the callback function with: mqtt reference, topic name, data length, data, and QoS level):

~~~c
mqtt_add_data_ptr(mqtt_ref, "topic_temperature_sensor", strlen(data), data, E_QOS_NONE);
~~~

For EQosLevel struct, include *mqtt_fc_qos.h*

## Implementation details

![State Machine Diagram](https://github.com/fabiocrestani/mqtt_fc/blob/master/docs/mqtt_fsm.png)

