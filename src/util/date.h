/*
 * date.h
 * Author: Fabio Crestani
 */

#ifndef __DATE_H__
#define __DATE_H__

#include <time.h>
#include <sys/time.h>

char * get_current_date_time(void);
int get_current_us(void);

#endif // __DATE_H__
