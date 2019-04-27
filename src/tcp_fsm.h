/*
 * tcp_fsm.h
 * Author: Fabio Crestani
 */

#ifndef __TCP_FSM_H__
#define __TCP_FSM_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "tcp.h"

typedef enum ENUM_TcpState_
{
	TCP_IDLE = 0,
	TCP_CONNECT,
	TCP_CONNECTED,
	TCP_ERROR
} ENUM_TcpState;

void tcp_fsm_init(char server_address[], uint32_t server_port);
void tcp_fsm_poll(void);

void tcp_fsm_set_idle(void);
void tcp_fsm_set_connect(void);
void tcp_fsm_set_connected(void);

#endif // __TCP_FSM_H__
