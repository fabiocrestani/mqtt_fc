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

void fsm_next_true_state(Fsm *fsm);
void fsm_next_false_state(Fsm *fsm);

void fsm_print_debug_state(FsmState *state, uint32_t level)
{
	uint8_t previous_level = level;

	printf("%s\n", state->name);

	for (uint32_t i = 0; i++ < level; printf("         "));

	printf("true  -> ");
	if (state->next_true_state) 
	{
		if (state->next_true_state == state)
		{
			printf("<- %s\n", state->name);
		}
		else
		{
			fsm_print_debug_state(state->next_true_state, ++level);
		}
	}
	else
	{
		printf("(null)\n");
	}

	for (uint32_t i = 0; i++ < level; printf("         "));

	printf("false -> ");
	if (state->next_false_state) 
	{
		if (state->next_false_state == state)
		{
			printf("<- %s\n", state->name);
		}
		else
		{
			if (level == previous_level) level++;
			fsm_print_debug_state(state->next_false_state, level);
		}
	}
	else
	{
		printf("(null)\n");
	}
}

void fsm_print_debug_tree(Fsm *fsm)
{
	printf("Printing FSM tree: \n");
	fsm_print_debug_state(fsm->state, 0);

	printf("\n\n\n");
}

uint8_t fsm_init(Fsm *fsm, FsmState *first_state)
{
	printf("fsm_init\n");

	fsm->state = first_state;

	fsm_print_debug_tree(fsm);

	return 0;
}

FsmState fsm_state_build(char *state_name, uint8_t (*start)(void), 
	uint8_t (*run)(void), uint8_t (*exit)(void))
{
	FsmState state;
	strcpy(state.name, state_name);

	state.next_true_state = 0;
	state.next_false_state = 0;

	state.start = (start == 0) ? 0 : (void *) start;
	state.run = (run == 0) ? 0 : (void *) run;
	state.exit = (exit == 0) ? 0 : (void *) exit;

	state.started = FALSE;

	return state;
}

void fsm_poll(Fsm *fsm)
{
	uint8_t ret = 0;
	printf("fsm_poll. State: %s\n", fsm->state->name);

	if (!fsm) return;
	//if (!(fsm->state)) return;

	if (!fsm->state->started)
	{
		if (fsm->state->start)
		{
			ret = (*fsm->state->start)();
		}
		fsm->state->started = TRUE;


		if (ret == 1)
		{
			fsm_next_true_state(fsm);
		}
		else
		{
			fsm_next_false_state(fsm);
		}

		return;
	}

	if (fsm->state->run)
	{
		ret = (*fsm->state->run)();
	}

	if (ret == 1)
	{
		fsm_next_true_state(fsm);
	}
	else
	{
		fsm_next_false_state(fsm);
	}

	return;
	
	(void) fsm;
}

void fsm_next_true_state(Fsm *fsm)
{
	printf("fsm changing state from %s to %s\n", 
		fsm->state->name, fsm->state->next_true_state->name);
	fsm->state = fsm->state->next_true_state;
}

void fsm_next_false_state(Fsm *fsm)
{
	printf("fsm changing state from %s to %s\n", 
		fsm->state->name, fsm->state->next_false_state->name);
	fsm->state = fsm->state->next_false_state;
}


