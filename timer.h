/*
 * $Header: /home/playground/src/atmega32/op2/timer.h,v 0e70a97349fc 2022/01/02 14:30:24 nkoch $
 */


#ifndef _TIMER_H
#define _TIMER_H


#include <stdbool.h>


extern volatile long millisecs;

extern long now (void);
extern void mark (long *since);
extern long elapsed (long *since);
extern long elapsed_mark (long *since);
extern bool is_elapsed (long *since, long howlong);
extern bool is_elapsed_mark (long *since, long howlong);
extern void sleep (void);
extern void sleep_until (const long ms);
extern void sleep_for (const long ms);


#endif
