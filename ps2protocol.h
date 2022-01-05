/*
 * $Header: /home/playground/src/atmega32/op2/ps2protocol.h,v 6c071e9e91df 2022/01/02 22:21:58 nkoch $
 */


#ifndef _PS2PROTOCOL_H
#define _PS2PROTOCOL_H


#include <setjmp.h>
#include "scancodebuf.h"


typedef struct
{
  void (*dbg) (const __flash char *fmt, ...);
  void (*set_clk) (bool state);
  void (*set_dta) (bool state);
  bool (*get_clk) ();
  bool (*get_dta) ();
  void (*from_host) (uint8_t command, uint8_t parameter);
  void (*error) (uint8_t t);
  struct sc_CBuf *scancodes;
  uint8_t state, prev_state;
  uint8_t rstate;
  uint8_t last_sent;
  jmp_buf inhibit, rts, resend;
  long reset_ack_delay, inhibit_wait_timeout;
}
PS2ProtocolContext;


enum PS2ProtocolError
{
  ps2_ERROR_UNKNOWN,
  ps2_ERROR_DTA_STUCK,
  ps2_ERROR_CLK_STUCK,
};


enum PS2Commands
{
  cINITOK               = 0xaa,
  cLEDS                 = 0xed,
  cECHO                 = 0xee,
  cSCANCODESET          = 0xf0,
  cREADID               = 0xf2,
  cTYPEMATIC            = 0xf3,
  cENABLE               = 0xf4,
  cDISABLE              = 0xf5,
  cDEFAULT              = 0xf6,
  cACK                  = 0xfa,
  cERROR                = 0xfc,
  cRESEND               = 0xfe,
  cRESET                = 0xff,
};


extern void ps2protocol (PS2ProtocolContext *c);
extern void initstate_ps2protocol (PS2ProtocolContext *c);
extern void initif_ps2protocol (PS2ProtocolContext *c,
                                struct sc_CBuf *scancodes,
                                void (*set_clk) (bool state),
                                void (*set_dta) (bool state),
                                bool (*get_clk) (),
                                bool (*get_dta) (),
                                void (*from_host) (uint8_t command, uint8_t parameter),
                                void (*error) (uint8_t error_code),
                                void (*dbg) (const __flash char *fmt, ...));


#endif
