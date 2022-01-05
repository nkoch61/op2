/*
 * $Header: /home/playground/src/atmega32/op2/switches.c,v 33f135374e37 2022/01/04 15:29:52 nkoch $
 */


#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <avr/io.h>

#include "common.h"
#include "defs.h"
#include "switches.h"


uint8_t read_switches ()
{
  return (PIN_SWITCHES & MASK_SWITCHES) ^ MASK_SWITCHES;
}


bool sw_autostart_loop ()
{
  return read_switches () & 0b00001;
}


bool sw_loop_forever ()
{
  return read_switches () & 0b00010;
}


bool sw_no_debug ()
{
  return read_switches () & 0b00100;
}


uint8_t sw_baudrate ()
{
  return (read_switches () & 0b11000) >> 3;
}
