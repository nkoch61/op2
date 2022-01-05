/*
 * $Header: /home/playground/src/atmega32/op2/scancode.h,v 7dd359e2cbdc 2021/12/27 17:44:04 nkoch $
 */


#ifndef _SCANCODE_H
#define _SCANCODE_H


#include <stdint.h>


enum ScancodeFlags
{
  Mak = 0,
  Brk = 1,
  E0  = 2
};


typedef struct
{
  uint8_t code, flags;
}
Scancode;


#endif
