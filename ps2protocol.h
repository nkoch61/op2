/*
 * $Header: /home/playground/src/atmega32/op2/ps2protocol.h,v 210edcd2f2d2 2022/01/23 16:30:50 nkoch $
 */


#ifndef _PS2PROTOCOL_H
#define _PS2PROTOCOL_H


#include <setjmp.h>
#include "scancodebuf.h"
#include "replybuf.h"


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
  struct reply_CBuf reply;
  uint8_t send_state;
  uint8_t last_sent, last_received;
  bool bat_active;
  long bat_delay;
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
  cALLKTTM              = 0xf7,
  cALLKTMB              = 0xf8,
  cALLKTM               = 0xf9,
  cALLKTMBT             = 0xfa,
  cKTTM                 = 0xfb,
  cKTMB                 = 0xfc,
  cKTM                  = 0xfd,
  cRESEND               = 0xfe,
  cRESET                = 0xff,

  cACK                  = 0xfa,
  cERROR                = 0xfc,
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
