/*
 * $Header: /home/playground/src/atmega32/op2/scancodefuncs.h,v e091880f3f5c 2022/01/05 15:52:39 nkoch $
 */


#ifndef _SCANCODEFUNCS_H
#define _SCANCODEFUNCS_H


#include <stdint.h>
#include "kbdscan.h"
#include "scancode.h"
#include "scancodebuf.h"


typedef struct
{
  void (*dbg) (const __flash char *fmt, ...);
  void (*scan) (void *s,
                void (*mak) (void *s, int8_t idx),
                void (*brk) (void *s, int8_t idx),
                void (*dbg) (const __flash char *fmt, ...),
                const KeyboardTiming *timing);
  struct sc_CBuf *scancodes;
  const KeyboardTiming *timing;
  struct
  {
    uint8_t pks_pressed, pks_sent;
    uint8_t alt_prog_mask;
    uint16_t mak_3, mak_7, mak_0, mak_sz, mak_ul, mak_del, mak_comma;
    uint16_t mak_period, mak_colon, mak_plus, mak_ae;
    uint16_t mak_oe, mak_ue, mak_percent, mak_00;
    uint16_t mak_res, mak_slash;
    uint16_t mak_numminus, mak_numplus;
    uint16_t mak_numstar, mak_numslash;
  }
  s;
}
ScancodeContext;


extern void next_scancode (ScancodeContext *c);

extern void initstate_scancode (ScancodeContext *c);

extern void initif_scancode (ScancodeContext *c,
                             void (*dbg) (const __flash char *fmt, ...),
                             void (*scan) (void *s,
                                           void (*mak) (void *s, int8_t idx),
                                           void (*brk) (void *s, int8_t idx),
                                           void (*dbg) (const __flash char *fmt, ...),
                                           const KeyboardTiming *timing),
                             struct sc_CBuf *scancodes,
                             const KeyboardTiming *timing);


#endif
