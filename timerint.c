/*
 * $Header: /home/playground/src/atmega32/op2/timerint.c,v 2b9dee575e86 2022/01/02 17:03:18 nkoch $
 */


#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "common.h"
#include "defs.h"
#include "timerint.h"
#include "timer.h"


#define F_INTERRUPT     1000    // Hz
#define PRESCALER       64
//  F_CPU/(PRESCALER*F_INTERRUPT)
#define TIMERVALUE      125


/***********************
 * 1ms Timerinterrupt *
 ***********************/

ISR (TIMER1_COMPA_vect)
{
  PORT_INTR |=  MASK_INTR;
  ++millisecs;
  PORT_INTR &= ~MASK_INTR;
}


void timerint_init (void)
{
  TCCR1A = 0;
  TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);
  OCR1AH = TIMERVALUE / 256;
  OCR1AL = TIMERVALUE % 256;
  TCNT1H = 0;
  TCNT1L = 0;
  TIMSK  = _BV(OCIE1A);
}
