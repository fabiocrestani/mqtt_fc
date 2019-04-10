/*
 * fsm.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "fsm.h"

uint8_t fsm_init(Fsm *mqtt_fsm)
{


}

FsmState fsm_state_build(char *state_name, void callback(void))
{
	FsmState state;
	strcpy(state.state_name, state_name);
	state.callback = callback;
	return state;
}

void fsm_poll(Fsm *mqtt_fsm)
{
	printf("fsm_poll\n");
}

