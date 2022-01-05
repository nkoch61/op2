/*
 * $Header: /home/playground/src/atmega32/op2/kbdscan.h,v c58c54c433e5 2021/12/24 16:35:10 nkoch $
 */


#ifndef _KBDSCAN_H
#define _KBDSCAN_H


#include <stdint.h>
#include "defs.h"


typedef struct
{
  long debounce;
  long initial_delay;
  long delay;
}
KeyboardTiming;


extern void kbdscan (void *s,
                     void (*mak) (void *s, int8_t idx),
                     void (*brk) (void *s, int8_t idx),
                     void (*dbg) (const __flash char *fmt, ...),
                     const KeyboardTiming *timing);


#endif
