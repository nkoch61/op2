/*
 * $Header: /home/playground/src/atmega32/op2/ps2io.c,v b5d576feef86 2022/01/08 21:46:27 nkoch $
 */


#include <stdbool.h>
#include <avr/io.h>

#include "ps2io.h"
#include "defs.h"


void ps2_set_clk (bool state)
{
  if (state == HI)
  {
    PORT_CLK |=  MASK_CLK;
    DDR_CLK  &= ~MASK_CLK;
  }
  else
  {
    DDR_CLK  |=  MASK_CLK;
    PORT_CLK &= ~MASK_CLK;
  }
}


void ps2_set_dta (bool state)
{
  if (state == HI)
  {
    PORT_DTA |=  MASK_DTA;
    DDR_DTA  &= ~MASK_DTA;
  }
  else
  {
    DDR_DTA  |=  MASK_DTA;
    PORT_DTA &= ~MASK_DTA;
  }
}


bool ps2_get_clk ()
{
  return !!(PIN_CLK & MASK_CLK);
}


bool ps2_get_dta ()
{
  return !!(PIN_DTA & MASK_DTA);
}


void ps2_init ()
{
  ps2_set_clk (HI);
  ps2_set_dta (HI);
}
