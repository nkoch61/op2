/*
 * $Header: /home/playground/src/atmega32/op2/keytable.h,v e091880f3f5c 2022/01/05 15:52:39 nkoch $
 */


#ifndef _KEYTABLE_H
#define _KEYTABLE_H


#include <stdint.h>

#include "defs.h"


enum SpecialKeys
{
  NO_SPECIAL,
  SPECIAL_CTRL,
  SPECIAL_SHIFT,
  SPECIAL_REP,
  SPECIAL_3,
  SPECIAL_7,
  SPECIAL_0,
  SPECIAL_SZ,
  SPECIAL_UL,
  SPECIAL_DEL,
  SPECIAL_RES,
  SPECIAL_COMMA,
  SPECIAL_PERIOD,
  SPECIAL_SLASH,
  SPECIAL_COLON,
  SPECIAL_PLUS,
  SPECIAL_AE,
  SPECIAL_OE,
  SPECIAL_UE,
  SPECIAL_PROG,
  SPECIAL_00,
  SPECIAL_LF,
  SPECIAL_LOCK,
  SPECIAL_NUMMINUS,
  SPECIAL_NUMPLUS,
  SPECIAL_NUMSTAR,
  SPECIAL_NUMSLASH,
};


enum KeyTableFlagBits
{
  NO_FLAGS      = 0x00,
  E0_PREFIX     = 0x02,
  SPECIAL       = 0x80,
};


typedef struct
{
  uint8_t f;
  uint8_t m;
  const __flash char *d;
}
KeyTableEntry;


extern const __flash KeyTableEntry keytable[ROWS*COLUMNS];


#endif
