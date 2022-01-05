/*
 * $Header: /home/playground/src/atmega32/op2/ps2protocol.c,v 6c071e9e91df 2022/01/02 22:21:58 nkoch $
 */


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/parity.h>

#include "common.h"
#include "defs.h"

#include "timer.h"
#include "scancodebuf.h"
#include "ps2protocol.h"


enum PS2ProtocolState
{
  ps2_INIT,
  ps2_RESEND,
  ps2_RESET_ACK,
  ps2_RESET,
  ps2_IDLE,
  ps2_INHIBIT,
  ps2_INHIBIT_WAIT,
  ps2_RECEIVE,
  ps2_TRANSMIT,
};


enum PS2ProtocolReceiveState
{
  ps2_RECEIVE_COMMAND,
  ps2_RECEIVE_LEDS,
  ps2_RECEIVE_SCANCODESET,
  ps2_RECEIVE_TYPEMATIC,
};


#define IDLE()          (c->get_dta () && c->get_clk ())
#define INHIBIT()       (!c->get_clk ())
#define RTS()           (c->get_clk () && !c->get_dta ())


static inline void ps2_inhibit_check (PS2ProtocolContext *c)
{
  if (INHIBIT ())
  {
    sei ();
    longjmp (c->inhibit, 1);
  }
}


static inline void ps2_rts_check (PS2ProtocolContext *c)
{
  if (RTS())
  {
    longjmp (c->rts, 1);
  }
}


static inline void ps2_transmit_bit (PS2ProtocolContext *c, bool bit)
{
  ps2_inhibit_check (c);
  c->set_dta (bit);
  _delay_us (15);
  c->set_clk (LO);
  _delay_us (30);
  c->set_clk (HI);
  _delay_us (15);
}


static void ps2_transmit_byte (PS2ProtocolContext *c, uint8_t byte)
{
  _delay_us (50);
  cli ();
  ps2_transmit_bit (c, LO);
  for (uint8_t mask = 0b1; mask; mask <<= 1)
  {
    ps2_transmit_bit (c, (byte & mask) ? HI : LO);
  };
  ps2_transmit_bit (c, parity_even_bit (byte) ? LO : HI);
  ps2_transmit_bit (c, HI);
  sei ();
  if (byte != cRESEND && byte != cACK)
  {
    c->last_sent = byte;
  }
}


static inline bool ps2_receive_bit (PS2ProtocolContext *c, bool ack)
{
  ps2_inhibit_check (c);
  _delay_us (15);
  const bool bit = c->get_dta ();
  if (ack)
  {
    c->set_dta (LO);
  };
  _delay_us (15);
  c->set_clk (LO);
  _delay_us (30);
  c->set_clk (HI);
  if (ack)
  {
    c->set_dta (HI);
  };
  return bit;
}


static uint8_t ps2_receive_byte (PS2ProtocolContext *c)
{
  cli ();
  const bool startbit = ps2_receive_bit (c, false);
  uint8_t byte = 0;
  for (uint8_t mask = 0b1; mask; mask <<= 1)
  {
    byte |= (ps2_receive_bit (c, false) == HI) ? mask : 0;
  };
  const bool parity = ps2_receive_bit (c, false);
  const bool stopbit = ps2_receive_bit (c, true);
  sei ();
  if (startbit != LO
      ||
      parity_even_bit (byte) == parity
      ||
      stopbit != HI)
  {
    longjmp (c->resend, 1);
  };
  return byte;
}


static void ps2_receive_command_handler (PS2ProtocolContext *c, uint8_t byte)
{
  switch (byte)
  {
    case cECHO:
      ps2_transmit_byte (c, cECHO);
      return;

    case cRESEND:
      ps2_transmit_byte (c, c->last_sent);
      return;

    case cRESET:
      c->state = ps2_RESET_ACK;
      mark (&c->reset_ack_delay);
      return;

    default:
      ps2_transmit_byte (c, cACK);
      break;
  };
  switch (byte)
  {
    case cLEDS:
      c->rstate = ps2_RECEIVE_LEDS;
      break;

    case cSCANCODESET:
      c->rstate = ps2_RECEIVE_SCANCODESET;
      break;

    case cREADID:
      ps2_transmit_byte (c, 0xab);
      ps2_transmit_byte (c, 0x83);
      break;

    case cTYPEMATIC:
      c->rstate = ps2_RECEIVE_TYPEMATIC;
      break;

    case cENABLE:
    case cDISABLE:
    case cDEFAULT:
      c->from_host (byte, 0);
      break;

    default:
      c->state = ps2_RESET;
      break;
  }
}


static void ps2_receive_handler (PS2ProtocolContext *c, uint8_t byte)
{
  switch (c->rstate)
  {
    case ps2_RECEIVE_COMMAND:
      ps2_receive_command_handler (c, byte);
      return;

    case ps2_RECEIVE_LEDS:
      ps2_transmit_byte (c, cACK);
      c->from_host (cLEDS, byte);
      break;

    case ps2_RECEIVE_SCANCODESET:
      ps2_transmit_byte (c, cACK);
      switch (byte)
      {
        case 0:
          ps2_transmit_byte (c, 2);
          break;

        default:
          c->from_host (cSCANCODESET, byte);
          break;
      };
      break;

    case ps2_RECEIVE_TYPEMATIC:
      ps2_transmit_byte (c, cACK);
      c->from_host (cTYPEMATIC, byte);
      break;

    default:
      c->state = ps2_RESET;
      break;
  };
  c->rstate = ps2_RECEIVE_COMMAND;
}


void ps2protocol (PS2ProtocolContext *c)
{
  Scancode scancode;

  if (setjmp (c->inhibit))
  {
    c->state = ps2_INHIBIT;
  };
  if (setjmp (c->rts))
  {
    c->state = ps2_RECEIVE;
  };
  if (setjmp (c->resend))
  {
    c->state = ps2_RESEND;
  };

  for (;;)
  {
    if (c->state != c->prev_state)
    {
      PORT_DEBUG ^= MASK_DEBUG & ((c->state ^ c->prev_state) << SHIFT_DEBUG);
      c->prev_state = c->state;
    };

    switch (c->state)
    {
      case ps2_RESEND:
        for (int i = -1; ++i < 11;)
        {
          if (c->get_dta () == HI)
          {
            break;
          };
          ps2_receive_bit (c, true);
        };
        if (c->get_dta () == LO)
        {
          c->error (ps2_ERROR_DTA_STUCK);
          return;
        };
        ps2_transmit_byte (c, cRESEND);
        c->state = ps2_IDLE;
        return;

      case ps2_RESET_ACK:
        ps2_inhibit_check (c);
        ps2_rts_check (c);
        if (elapsed (&c->reset_ack_delay) >= 300)
        {
          ps2_transmit_byte (c, cACK);
          c->state = ps2_RESET;
        };
        return;

      case ps2_RESET:
        c->rstate = ps2_RECEIVE_COMMAND;
        c->set_dta (HI);
        c->set_clk (HI);
        ps2_inhibit_check (c);
        ps2_rts_check (c);
        _delay_us (50);
        if (IDLE())
        {
          ps2_transmit_byte (c, cINITOK);
          c->state = ps2_IDLE;
        };
        return;

      case ps2_IDLE:
        c->set_dta (true);
        c->set_clk (true);
        ps2_inhibit_check (c);
        ps2_rts_check (c);
        if (c->rstate == ps2_RECEIVE_COMMAND && !sc_empty (c->scancodes))
        {
          c->state = ps2_TRANSMIT;
        };
        return;

      case ps2_INHIBIT:
        mark (&c->inhibit_wait_timeout);
        c->state = ps2_INHIBIT_WAIT;
        /* FALLTHROUGH */

      case ps2_INHIBIT_WAIT:
        _delay_us (5);
        ps2_rts_check (c);
        if (!INHIBIT())
        {
          c->state = ps2_IDLE;
        }
        else if (elapsed (&c->inhibit_wait_timeout) >= 20)
        {
          c->error (ps2_ERROR_CLK_STUCK);
        };
        return;

      case ps2_RECEIVE:
        c->state = ps2_IDLE;
        ps2_receive_handler (c, ps2_receive_byte (c));
        return;

      case ps2_TRANSMIT:
        c->state = ps2_IDLE;
        scancode = sc_peek (c->scancodes);
        if (scancode.flags & E0)
        {
          ps2_transmit_byte (c, 0xe0);
        };
        if (scancode.flags & Brk)
        {
          ps2_transmit_byte (c, 0xf0);
        };
        ps2_transmit_byte (c, scancode.code);
        sc_less (c->scancodes);
        return;

      default:
        c->state = ps2_RESET;
        return;
    }
  }
}


void initstate_ps2protocol (PS2ProtocolContext *c)
{
  c->state = ps2_RESET;
  c->prev_state = ps2_INIT;
  c->rstate = ps2_RECEIVE_COMMAND;
  PORT_DEBUG &= ~MASK_DEBUG;
}


void initif_ps2protocol (PS2ProtocolContext *c,
                         struct sc_CBuf *scancodes,
                         void (*set_clk) (bool state),
                         void (*set_dta) (bool state),
                         bool (*get_clk) (),
                         bool (*get_dta) (),
                         void (*from_host) (uint8_t command, uint8_t parameter),
                         void (*error) (uint8_t error_code),
                         void (*dbg) (const __flash char *fmt, ...))
{
  c->dbg = dbg;
  c->set_clk = set_clk;
  c->set_dta = set_dta;
  c->get_clk = get_clk;
  c->get_dta = get_dta;
  c->from_host = from_host;
  c->error = error;
  c->scancodes = scancodes;
}
