/*
 * fsm.h
 * Author: Fabio Crestani
 */

#ifndef __FSM_H__
#define __FSM_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef struct FsmState_ {
	char state_name[128];
	void (*callback)(int);

} FsmState;

typedef struct Fsm_ {
	FsmState state;
	FsmState *next_true_state;
	FsmState *next_false_state;

} Fsm;

uint8_t fsm_init(Fsm *mqtt_fsm);
void fsm_poll(Fsm *mqtt_fsm);

FsmState fsm_state_build(char *state_name, void callback(void));

#endif // __FSM_H__
