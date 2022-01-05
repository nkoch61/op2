/*
 * $Header: /home/playground/src/atmega32/op2/switches.h,v 33f135374e37 2022/01/04 15:29:52 nkoch $
 */


#ifndef _SWITCHES_H
#define _SWITCHES_H


extern uint8_t read_switches ();
extern bool sw_autostart_loop ();
extern bool sw_loop_forever ();
extern bool sw_no_debug ();
extern uint8_t sw_baudrate ();


#endif
