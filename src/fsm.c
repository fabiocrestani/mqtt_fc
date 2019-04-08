/*
 * fsm.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef struct FsmState_ {

} FsmState;

typedef struct Fsm_ {

	FsmState state;

} Fsm;


uint8_t fsm_init(FsmState initial_state)
{


}
