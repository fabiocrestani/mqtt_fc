/*
 * string_utils.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "string_utils.h"

uint8_t equals(char * string1, char * string2)
{
	return (strcmp(string1, string2) == 0);
}
