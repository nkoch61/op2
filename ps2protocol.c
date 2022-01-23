/*
 * $Header: /home/playground/src/atmega32/op2/ps2protocol.c,v bc5c1b69a904 2022/01/22 21:53:05 nkoch $
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


#define IDLE()          (c->get_dta () && c->get_clk ())
#define HOSTREQ()       (!c->get_clk () || !c->get_dta ())
#define INHIBIT()       (!c->get_clk ())
#define RTS()           (c->get_clk () && !c->get_dta ())


enum SendState
{
  S0, S1, S2
};


static inline void ps2_transmit_bit (PS2ProtocolContext *c, bool bit)
{
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
  if (byte != cRESEND)
  {
    c->last_sent = byte;
  }
}


static inline bool ps2_receive_bit (PS2ProtocolContext *c, bool ack)
{
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
    _delay_us (15);
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
    cli ();
    for (int i = -1; ++i < 11;)
    {
      if (c->get_dta () == HI)
      {
        break;
      };
      ps2_receive_bit (c, true);
    };
    sei ();
    if (c->get_dta () == LO)
    {
      c->error (ps2_ERROR_DTA_STUCK);
    };
    return 0xef;        /* illegales Kommando */
  };
  return byte;
}


static void ps2_receive_handler (PS2ProtocolContext *c, uint8_t byte)
{
  const uint8_t last_received = c->last_received;

  c->last_received = byte;
  reply_clear (&c->reply);

  switch (byte)
  {
    case cECHO:
      reply_put (&c->reply, cECHO);
      return;

    case cRESEND:
      reply_put (&c->reply, c->last_sent);
      return;

    case cRESET:
      reply_put (&c->reply, cACK);
      c->from_host (cDISABLE, 0);
      mark (&c->reset_delay);
      c->reset = true;
      return;

    case cLEDS:
      reply_put (&c->reply, cACK);
      return;

    case cSCANCODESET:
      reply_put (&c->reply, cACK);
      return;

    case cREADID:
      reply_put (&c->reply, cACK);
      reply_put (&c->reply, 0xab);
      reply_put (&c->reply, 0x83);
      return;

    case cTYPEMATIC:
      reply_put (&c->reply, cACK);
      return;

    case cENABLE:
    case cDISABLE:
    case cDEFAULT:
      reply_put (&c->reply, cACK);
      c->from_host (byte, 0);
      return;

    case cKTTM:
    case cKTMB:
    case cKTM:
    case cALLKTTM:
    case cALLKTMB:
    case cALLKTM:
    case cALLKTMBT:
      reply_put (&c->reply, cACK);
      return;


    default:
      switch (last_received)
      {
        case cLEDS:
          reply_put (&c->reply, cACK);
          c->from_host (cLEDS, byte);
          return;

        case cSCANCODESET:
          reply_put (&c->reply, cACK);
          switch (byte)
          {
            case 0:
              reply_put (&c->reply, 2);
              return;

            default:
              c->from_host (cSCANCODESET, byte);
              return;
          };

        case cTYPEMATIC:
          reply_put (&c->reply, cACK);
          c->from_host (cTYPEMATIC, byte);
          return;

        case cKTTM:
        case cKTMB:
        case cKTM:
          reply_put (&c->reply, cACK);
          c->last_received = cKTM;
          break;

        default:
          reply_put (&c->reply, cRESEND);
          return;
      }
  }
}


void ps2protocol (PS2ProtocolContext *c)
{
  Scancode scancode;

  if (INHIBIT())
  {
    for (int i = -1; INHIBIT() && ++i < 200/5;)
    {
      _delay_us (5);
    };
    if (INHIBIT())
    {
      c->error (ps2_ERROR_CLK_STUCK);
      return;
    }
  };
  if (RTS())
  {
    ps2_receive_handler (c, ps2_receive_byte (c));
    c->send_state = S0;
  };

  if (!reply_empty (&c->reply))
  {
    ps2_transmit_byte (c, reply_get (&c->reply));
    return;
  };

  if (c->reset)
  {
    if (elapsed (&c->reset_delay) >= 500)
    {
      reply_put (&c->reply, cINITOK);
      c->from_host (cENABLE, 0);
      c->reset = false;
    };
    return;
  };

  if (!sc_empty (c->scancodes))
  {
    scancode = sc_peek (c->scancodes);
    switch (c->send_state)
    {
      case S0:
        if (scancode.flags & E0)
        {
          ps2_transmit_byte (c, 0xe0);
          c->send_state = S1;
          return;
        };
        /* FALLTHROUGH */

      case S1:
        if (scancode.flags & Brk)
        {
          ps2_transmit_byte (c, 0xf0);
          c->send_state = S2;
          return;
        };
        /* FALLTHROUGH */

      case S2:
        ps2_transmit_byte (c, scancode.code);
        sc_less (c->scancodes);
        c->send_state = S0;
        return;

      default:
        c->send_state = S0;
        return;
    }
  }
}


void initstate_ps2protocol (PS2ProtocolContext *c)
{
  mark (&c->reset_delay);
  c->reset = true;
  c->send_state = S0;
  c->last_sent = 0;
  c->last_received = 0;
  reply_init (&c->reply);
  c->set_clk (HI);
  c->set_dta (HI);
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
