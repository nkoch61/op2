/*
 * $Header: /home/playground/src/atmega32/op2/typematic.h,v ec5d23a8d2e5 2022/01/02 20:01:14 nkoch $
 */


#ifndef _TYPEMATIC_H
#define _TYPEMATIC_H


extern void typematic_to_timing (uint8_t typematic, KeyboardTiming *timing);
extern void default_timing (KeyboardTiming *timing);


#endif
