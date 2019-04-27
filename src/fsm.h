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

#define FALSE (0)
#define TRUE (1)

typedef struct FsmState_ FsmState;

typedef struct FsmState_ {
	char name[128];
	uint8_t (*start)(void);
	uint8_t (*run)(void);
	uint8_t (*exit)(void);

	FsmState *next_true_state;
	FsmState *next_false_state;

	uint8_t started;

} FsmState;

typedef struct Fsm_ {
	FsmState *state;
	FsmState *next_state;

} Fsm;

uint8_t fsm_init(Fsm *fsm, FsmState *first_state);
void fsm_poll(Fsm *mqtt_fsm);

FsmState fsm_state_build(char *state_name, uint8_t (*start)(void), 
	uint8_t (*run)(void), uint8_t (*exit)(void));

#endif // __FSM_H__
