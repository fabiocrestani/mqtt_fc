/*
 * date.c
 * Author: Fabio Crestani
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "date.h"

char * get_current_date_time(void)
{
	static char buffer[64];
	time_t t;
	time(&t);
	struct tm *tm = localtime(&t);

	sprintf(buffer, "%02d.%02d.%02d %02d:%02d:%02d", 
		tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, 
		tm->tm_min, tm->tm_sec);

	return buffer;
}

