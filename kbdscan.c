/* kbdscan.c */


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "kbdscan.h"
#include "timer.h"
#include "defs.h"


typedef struct
{
  uint8_t s[ROWS];
}
KbdState;


static void nodbg (const __flash char *fmt, ...)
{
}



static void read_keys (KbdState *state)
{
  for (int8_t row = -1; ++row < ROWS;)
  {
    PORT_KBDOUT = row<<1;
    _delay_us (1);
    state->s[row] = (PIN_KBDIN & MASK_KBDIN) ^ MASK_KBDIN;
  };
  PORT_KBDOUT = 1;
}


static bool debounced (KbdState *in,
                       KbdState *out,
                       void (*dbg) (const __flash char *fmt, ...),
                       long debounce_delay)
{
  enum S
  {
    S0 = 0,
    S1,
    S2
  };
  static uint8_t state = S0, prev_state = S0;
  static KbdState temp;
  static long debounce_start;
  bool changed = false;

  switch (state)
  {
    default:
      state = S0;
      /* FALLTHROUGH */

    case S0:
      memset (out, 0, sizeof *out);
      if (memcmp (in, out, sizeof *in) == 0)
      {
        /* keine Taste gedrueckt */
        state = S1;
      };
      break;

    case S1:
      if (memcmp (in, out, sizeof *in) != 0)
      {
        state = S2;
        temp = *in;
        mark (&debounce_start);
      };
      break;

    case S2:
      if (memcmp (in, &temp, sizeof temp) != 0)
      {
        /* prellt */
        temp = *in;
        mark (&debounce_start);
      }
      else if (is_elapsed (&debounce_start, debounce_delay))
      {
        /* entprellt */
        state = S1;
        changed = memcmp (in, out, sizeof *in) != 0;
      };
      break;
  };

  if (state != prev_state)
  {
    dbg (PSTR("debounced S:%u->%u\r\n"), prev_state, state);
    prev_state = state;
  };

  return changed;
}


void kbdscan (void *s,
              void (*mak) (void *s, int8_t idx),
              void (*brk) (void *s, int8_t idx),
              void (*dbg) (const __flash char *fmt, ...),
              const KeyboardTiming *timing)
{
  enum S
  {
    S0 = 0,
    S1,
    S2,
    S3
  };
  static uint8_t state = S0, prev_state = S0;
  static KbdState kbd_state, prev_kbd_state;
  static int8_t last_key_code = -1;
  static long repeat_start;

  read_keys (&kbd_state);
  if (debounced (&kbd_state, &prev_kbd_state, nodbg, timing->debounce))
  {
    for (int8_t row = -1; ++row < ROWS;)
    {
      const uint8_t curr_row_state = kbd_state.s[row];
      const uint8_t prev_row_state = prev_kbd_state.s[row];
      if (curr_row_state != prev_row_state)
      {
        for (int8_t column = -1; ++column < COLUMNS;)
        {
          const uint8_t key_mask = 1 << column;
          const uint8_t key_is_pressed = curr_row_state & key_mask;
          const uint8_t key_was_pressed = prev_row_state & key_mask;
          if (!key_is_pressed && key_was_pressed)
          {
            const int8_t key_code = row*COLUMNS+column;
            brk (s, key_code);
            if (key_code == last_key_code)
            {
              last_key_code = -1;
              state = S0;
            }
          }
          else if (key_is_pressed && !key_was_pressed)
          {
            last_key_code = row*COLUMNS+column;
            state = S1;
          }
        }
      }
    };
    prev_kbd_state = kbd_state;
  };

  switch (state)
  {
    default:
      state = S0;
      break;

    case S0:
      break;

    case S1:
      mak (s, last_key_code);
      if (timing->initial_delay)
      {
        mark (&repeat_start);
        state = S2;
      }
      else
      {
        state = S0;
      };
      break;

    case S2:
      if (is_elapsed (&repeat_start, timing->initial_delay))
      {
        mak (s, last_key_code);
        if (timing->delay)
        {
          mark (&repeat_start);
          state = S3;
        }
        else
        {
          state = S0;
        }
      };
      break;

    case S3:
      if (is_elapsed (&repeat_start, timing->delay))
      {
        mak (s, last_key_code);
        mark (&repeat_start);
      };
      break;
  };

  if (state != prev_state)
  {
    dbg (PSTR("kbdscan S:%u->%u\r\n"), prev_state, state);
    prev_state = state;
  }
}
