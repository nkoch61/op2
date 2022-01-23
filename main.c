/*
 * $Header: /home/playground/src/atmega32/op2/main.c,v cf4608cb5f99 2022/01/22 16:14:02 nkoch $
 */


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "common.h"
#include "switches.h"
#include "cmdint.h"
#include "uart.h"
#include "timerint.h"

#include "timer.h"
#include "scancodefuncs.h"
#include "scancodebuf.h"
#include "kbdscan.h"
#include "typematic.h"
#include "ps2io.h"
#include "ps2protocol.h"

#include "defs.h"


static const __flash char program_version[] = "1.0.15 " __DATE__ " " __TIME__;


static volatile long badcount;


/*******************
 * Fehlerinterrupt *
 *******************/

ISR (BADISR_vect)
{
  ++badcount;
}


static void background_sleep (void)
{
  sleep ();
}


/*****************************
 * Kommandoschnittstelle (1) *
 *****************************/

static void uart_cancel (void)
{
  uart_puts_P (PSTR ("#"));
}


static void uart_prompt (void)
{
  uart_puts_P (PSTR (">"));
}


/*****************************
 * Kommandoschnittstelle (2) *
 *****************************/

static int8_t op2_istat (int8_t argc, char **argv);
static int8_t op2_kbdin (int8_t argc, char **argv);
static int8_t op2_kbdout (int8_t argc, char **argv);
static int8_t op2_scan1 (int8_t argc, char **argv);
static int8_t op2_scan (int8_t argc, char **argv);
static int8_t op2_kbd (int8_t argc, char **argv);
static int8_t op2_kbdtranslate (int8_t argc, char **argv);
static int8_t op2_switches (int8_t argc, char **argv);
static int8_t op2_clkp (int8_t argc, char **argv);
static int8_t op2_dtap (int8_t argc, char **argv);
static int8_t op2_help (int8_t argc, char **argv);
static int8_t op2_version (int8_t argc, char **argv);


static const __flash struct CMD
{
  const __flash char *name;
  int8_t (*func) (int8_t argc, char **argv);
} cmds[] =
{
  { .name = FSTR("istat"),       .func = op2_istat       },
  { .name = FSTR("kbdin"),       .func = op2_kbdin       },
  { .name = FSTR("kbdout"),      .func = op2_kbdout      },
  { .name = FSTR("scan1"),       .func = op2_scan1       },
  { .name = FSTR("scan"),        .func = op2_scan        },
  { .name = FSTR("kbd"),         .func = op2_kbd         },
  { .name = FSTR("kbdtr"),       .func = op2_kbdtranslate},
  { .name = FSTR("switches"),    .func = op2_switches    },
  { .name = FSTR("clkp"),        .func = op2_clkp        },
  { .name = FSTR("dtap"),        .func = op2_dtap        },
  { .name = FSTR("?"),           .func = op2_help        },
  { .name = FSTR("ver"),         .func = op2_version     },
};


/* Interruptstatistik */
static int8_t op2_istat (int8_t argc, char **argv)
{
  uart_printf_P (PSTR("up=%lums bad=%lu"), millisecs, badcount);
  return 0;
}


/* Tastaturmatrix */
static int8_t op2_kbdin (int8_t argc, char **argv)
{
  uart_printf_P (PSTR("%x"), (PIN_KBDIN & MASK_KBDIN) ^ MASK_KBDIN);
  return 0;
}


/* Tastaturmatrix */
static int8_t op2_kbdout (int8_t argc, char **argv)
{
  uint16_t row;
  char c;

  if (argc == 1 || strcmp_P (argv[1], PSTR("off")) == 0)
  {
    PORT_KBDOUT = 1;
    return 0;
  };

  if (argc == 2 && sscanf_P (argv[1], PSTR ("%u%c"), &row, &c) == 1
      &&
      row < ROWS)
  {
    PORT_KBDOUT = row<<1;
    return 0;
  };

  return -1;
}


/* Tastaturmatrix */
static int8_t op2_scan1 (int8_t argc, char **argv)
{
  uint16_t row;
  char c;

  if (argc == 2 && sscanf_P (argv[1], PSTR ("%u%c"), &row, &c) == 1
      &&
      row < ROWS)
  {
    uart_puts ("\e[?25l");
    PORT_KBDOUT = row<<1;
    do
    {
      uart_putc ('[');
      const uint8_t in = (PIN_KBDIN & MASK_KBDIN) ^ MASK_KBDIN;
      for (int8_t column = -1; ++column < COLUMNS;)
      {
        uart_putc ((in & (1<<column)) ? '#' : '_');
      };
      uart_puts ("]\r");
    }
    while (uart_getc_nowait () == -1);
    PORT_KBDOUT = 1;
    uart_putsln ("\e[?25h");
    return 0;
  };
  return -1;
}


/* Tastaturmatrix */
static int8_t op2_scan (int8_t argc, char **argv)
{
  uart_puts ("\e[?25l");
  do
  {
    uart_putc ('[');
    for (int8_t row = -1; ++row < ROWS;)
    {
      PORT_KBDOUT = row<<1;
      _delay_us (1);
      const uint8_t in = (PIN_KBDIN & MASK_KBDIN) ^ MASK_KBDIN;
      for (int8_t column = -1; ++column < COLUMNS;)
      {
        uart_putc ((in & (1<<column)) ? '#' : '_');
      }
    };
    PORT_KBDOUT = 1;
    uart_puts ("]\r");
  }
  while (uart_getc_nowait () == -1);
  uart_putsln ("\e[?25h");
  return 0;
}


/* Tastaturmatrix */
static int8_t op2_keys (int8_t argc, char **argv)
{
  do
  {
    for (int8_t i = -1; ++i < 16;)
    {
      PORT_KBDOUT = i<<1;
      _delay_us (1);
      const uint8_t in = (PIN_KBDIN & MASK_KBDIN) ^ MASK_KBDIN;
      for (int8_t j = -1; ++j < 8;)
      {
        if (in & (1<<j))
        {
          uart_printf_P (PSTR("%02x,%02x\r\n"), i, j);
        }
      }
    };
    PORT_KBDOUT = 1;
  }
  while (uart_getc_nowait () == -1);
  return 0;
}


static void no_printf_P (const __flash char *fmt, ...)
{
}


static void debug_printf_P (const __flash char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  uart_printf_P (PSTR("%08lx: "), now ());
  uart_vsnprintf_P (fmt, ap);
  va_end (ap);
}


static void op2_scanrepeat_mak (void *s, int8_t idx)
{
  debug_printf_P (PSTR("mak %x\r\n"), idx);
}


static void op2_scanrepeat_brk (void *s, int8_t idx)
{
  debug_printf_P (PSTR("brk %x\r\n"), idx);
}


/* Tastaturmatrix */
static int8_t op2_kbd (int8_t argc, char **argv)
{
  static const KeyboardTiming timing =
  {
    .debounce = 5,
    .initial_delay = 500,
    .delay = 100
  };

  do
  {
    sleep_for (1);
    kbdscan (NULL, op2_scanrepeat_mak, op2_scanrepeat_brk, debug_printf_P, &timing);
  }
  while (uart_getc_nowait () == -1);
  return 0;
}


/* Tastaturfunktion mit PS/2-Protokoll */
static KeyboardTiming timing;
static struct sc_CBuf scancodes;
static bool force_numlock = false, enabled = false;


static void op2_kbdtr_from_host (uint8_t command,
                                 uint8_t parameter)
{
  void (* const dbg) (const __flash char *fmt, ...) = sw_no_debug () ? no_printf_P : debug_printf_P;

  switch (command)
  {
    case cLEDS:
      {
        const bool scroll = !!(parameter & 0b001);
        const bool num    = !!(parameter & 0b010);
        const bool caps   = !!(parameter & 0b100);
        dbg (PSTR("leds scroll=%u num=%u caps=%u\r\n"), scroll, num, caps);
        if (!num)
        {
          force_numlock = true;
        }
      };
      break;

    case cSCANCODESET:
      dbg (PSTR("scancode set %u\r\n"), parameter);
      break;

    case cTYPEMATIC:
      typematic_to_timing (parameter, &timing);
      dbg (PSTR("typematic %ums %ums\r\n"), timing.initial_delay, timing.delay);
      break;

    case cENABLE:
      enabled = true;
      sc_clear (&scancodes);
      dbg (PSTR("enable\r\n"), parameter);
      break;

    case cDISABLE:
      enabled = false;
      default_timing (&timing);
      dbg (PSTR("disable\r\n"), parameter);
      break;

    case cDEFAULT:
      default_timing (&timing);
      dbg (PSTR("default\r\n"), parameter);
      break;

    default:
      dbg (PSTR("from_host %x %x\r\n"), command, parameter);
      break;
  }
}


static void op2_kbdtr_error (uint8_t t)
{
  if (!sw_no_debug ())
  {
    debug_printf_P (PSTR("error %x\r\n"), t);
  }
}


static bool op2_kbdtr_abort ()
{
  return !sw_loop_forever () && uart_getc_nowait () != -1;
}


static int8_t op2_kbdtranslate (int8_t argc, char **argv)
{
  ScancodeContext sc;
  PS2ProtocolContext pc;
  void (* const dbg) (const __flash char *fmt, ...) = sw_no_debug () ? no_printf_P : debug_printf_P;
  bool poweron = argc == 1 && strcmp_P (argv[0], PSTR("poweron")) == 0;

  default_timing (&timing);
  sc_init (&scancodes);
  initif_scancode (&sc,
                   dbg,
                   kbdscan,
                   &scancodes,
                   &timing);
  initif_ps2protocol (&pc,
                      &scancodes,
                      ps2_set_clk,
                      ps2_set_dta,
                      ps2_get_clk,
                      ps2_get_dta,
                      op2_kbdtr_from_host,
                      op2_kbdtr_error,
                      dbg);
  initstate_scancode (&sc);
  initstate_ps2protocol (&pc);

  enabled = true;

  do
  {
    sleep ();
    poweron = poweron && now () < 550;
    if (enabled && !poweron)
    {
      next_scancode (&sc);
    };
    ps2protocol (&pc);
    if (force_numlock && sc_empty (&scancodes))
    {
      static const Scancode mak_numlock = { .code = 0x77, .flags = Mak }, brk_numlock = { .code = 0x77, .flags = Brk };
      sc_put (&scancodes, mak_numlock);
      sc_put (&scancodes, brk_numlock);
      force_numlock = false;
    }
  }
  while (!op2_kbdtr_abort ());
  return 0;
}


/* sCLK/DTA-Pulse */
static int8_t op2_clkp (int8_t argc, char **argv)
{
  do
  {
    sleep_for (1);
    ps2_set_clk (false);
    sleep_for (1);
    ps2_set_clk (true);
  }
  while (uart_getc_nowait () == -1);
  return 0;
}


static int8_t op2_dtap (int8_t argc, char **argv)
{
  do
  {
    sleep_for (1);
    ps2_set_dta (false);
    sleep_for (1);
    ps2_set_dta (true);
  }
  while (uart_getc_nowait () == -1);
  return 0;
}


/* DIP-Switches */
static int8_t op2_switches (int8_t argc, char **argv)
{
  uart_printf_P (PSTR("%x\r\n"), read_switches ());
  return 0;
}


static int8_t op2_help (int8_t argc, char **argv)
{
  for (int8_t i = -1; ++i < LENGTH (cmds);)
  {
    uart_putc (' ');
    uart_puts_P (cmds[i].name);
  };
  return 0;
}


static int8_t op2_version (int8_t argc, char **argv)
{
  uart_puts_P (program_version);
  return 0;
}


static int8_t op2_line (void)
{
  return 0;
}


static int8_t op2_interp (int8_t argc, char **argv)
{
  if (argc >= 1)
  {
    for (int8_t i = -1; ++i < LENGTH (cmds);)
    {
      const __flash char *name = cmds[i].name;
      if (strcmp_P (argv[0], name) == 0)
      {
        if (cmds[i].func (argc, argv) == 0)
        {
          uart_crlf ();
          return 0;
        };
        break;
      }
    }
  };
  uart_putsln_P (PSTR ("?"));
  return 0;
}


/*******************
 * Initialisierung *
 *******************/

static void init (void)
{
  cli ();

  /* PS2-Interface CLK/DTA */
  ps2_init ();

  /* Eingaenge DIP-Switches */
  DDR_SWITCHES  &= ~MASK_SWITCHES;
  PORT_SWITCHES |=  MASK_SWITCHES;

  uart_init ();
  timerint_init ();

  /* Ausgaenge Tastaturmatrix*/
  DDR_KBDOUT    |=  MASK_KBDOUT;
  PORT_KBDOUT    =  1;

  /* Eingaenge Tastaturmatrix */
  DDR_KBDIN      =  0x00;
  PORT_KBDIN     =  0xFF;       /* Pull-Up */

  /* Ausgaenge Debugportbits */
  DDR_DEBUG     |=  MASK_DEBUG;
  PORT_DEBUG    &= ~MASK_DEBUG;

  /* Ausgang Interrupttoggle */
  DDR_INTR      |=  MASK_INTR;
  PORT_INTR     &= ~MASK_INTR;

  /* ungenutzte Eingaenge */
  DDRC          &= ~UNUSED_C;
  PORTC         |=  UNUSED_C;   /* Pull-Up */

  uart_sleep = background_sleep;

  sei ();
}


static void signon_message (void)
{
  uart_puts_P (PSTR ("\r\n\n\nOPERATOR-II "));
  uart_putsln_P (program_version);
}


/*************************
 * Hauptprogrammschleife *
 *************************/

int main (void)
{
  static const struct CmdIntCallback cic =
  {
    .getc_f   = uart_getc,
    .putc_f   = uart_putc,
    .bs_f     = uart_bs,
    .crlf_f   = uart_crlf,
    .cancel_f = uart_cancel,
    .prompt_f = uart_prompt,
    .interp_f = op2_interp,
    .line_f   = op2_line
  };

  init ();
  signon_message ();

  if (sw_autostart_loop ())
  {
    char *argv[2] = { "poweron", NULL };
    uart_puts_P (PSTR ("\n*** keyboard loop ***\n"));
    op2_kbdtranslate (1, argv);
  };
  for (;;)
  {
    cmdint (&cic);
  };
  return 0;
}
