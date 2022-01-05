/*
 * $Header: /home/playground/src/atmega32/op2/ps2io.h,v fe61d670f29c 2021/12/26 17:34:13 nkoch $
 */


#ifndef _PS2IO_H
#define _PS2IO_H


#include <stdbool.h>


extern void ps2_set_clk (bool state);
extern void ps2_set_dta (bool state);
extern bool ps2_get_clk ();
extern bool ps2_get_dta ();
extern void ps2_init ();


#endif
