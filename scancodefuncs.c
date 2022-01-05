/* scancodefuncs.c */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "scancodefuncs.h"
#include "kbdscan.h"
#include "keytable.h"
#include "common.h"


enum PrefixKeyState
{
  NO_PREFIX = 0x00,
  SHIFT     = 0x01,
  CTRL      = 0x02,
  ALT       = 0x04,
  ALTGR     = 0x08
};


static void send_mak (ScancodeContext *c, uint16_t mak)
{
  if (!sc_full (c->scancodes))
  {
    Scancode sc;
    if (mak & 0x100)
    {
      sc.flags = Mak | E0;
    }
    else
    {
      sc.flags = Mak;
    };
    sc.code = mak & 0xff;
    sc_put (c->scancodes, sc);
  }
}


static void send_brk (ScancodeContext *c, uint16_t mak)
{
  if (mak && !sc_full (c->scancodes))
  {
    Scancode sc;
    if (mak & 0x100)
    {
      sc.flags = Brk | E0;
    }
    else
    {
      sc.flags = Brk;
    };
    sc.code = mak & 0xff;
    sc_put (c->scancodes, sc);
  }
}


static void send1_prefix (ScancodeContext *c, uint8_t pks, uint8_t mask, uint16_t mak)
{
  if (!(pks & mask) && (c->s.pks_sent & mask))
  {
    send_brk (c, mak);
    c->s.pks_sent &= ~mask;
  }
  else if ((pks & mask) && !(c->s.pks_sent & mask))
  {
    send_mak (c, mak);
    c->s.pks_sent |= mask;
  }
}


static void send_prefix (ScancodeContext *c, uint8_t pks)
{
  if (pks == c->s.pks_sent)
  {
    return;
  };

  send1_prefix (c, pks, ALTGR, '\x11'|0x100);
  send1_prefix (c, pks, ALT,   '\x11');
  send1_prefix (c, pks, CTRL,  '\x14');
  send1_prefix (c, pks, SHIFT, '\x12');
}


static void send_prefix_mak (ScancodeContext *c, uint8_t pks, uint16_t mak)
{
  send_prefix (c, pks);
  send_mak (c, mak);
}


static void mak_special (ScancodeContext *c, const __flash KeyTableEntry *k)
{
  switch (k->m)
  {
    case SPECIAL_CTRL:
      c->s.pks_pressed |= CTRL;
      send_prefix (c, c->s.pks_pressed);
      break;

    case SPECIAL_SHIFT:
      c->s.pks_pressed |= SHIFT;
      send_prefix (c, c->s.pks_pressed);
      break;

    case SPECIAL_REP:
      c->s.pks_pressed |= ALT;
      send_prefix (c, c->s.pks_pressed);
      break;

    case SPECIAL_3:
      if (c->s.pks_pressed & SHIFT)
      {
        send_prefix_mak (c, c->s.pks_pressed & ~SHIFT, c->s.mak_3 = '\x5d');            /* "#" */
      }
      else
      {
        send_prefix_mak (c, c->s.pks_pressed, c->s.mak_3 = '\x26');                     /* "3" */
      };
      break;

    case SPECIAL_7:
      switch (c->s.pks_pressed & (ALT|ALTGR|SHIFT))
      {
        case ALTGR|SHIFT:
          send_prefix_mak (c, c->s.pks_pressed & ~(ALTGR|SHIFT), c->s.mak_7 = '\x55');  /* "´" */
          break;

        case ALT|SHIFT:
          send_prefix_mak (c, c->s.pks_pressed & ~ALT, c->s.mak_7 = '\x55');            /* "`" */
          break;

        case SHIFT:
          send_prefix_mak (c, c->s.pks_pressed, c->s.mak_7 = '\x5d');                   /* "'" */
          break;

        case ALT|ALTGR:
          break;

        default:
          send_prefix_mak (c, c->s.pks_pressed, c->s.mak_7 = '\x3d');                   /* "7" */
          break;
      };
      break;

    case SPECIAL_0:
      if (c->s.pks_pressed & SHIFT)
      {
        send_prefix_mak (c, (c->s.pks_pressed & ~SHIFT) | ALTGR, c->s.mak_0 = '\x15');  /* "@" */
      }
      else
      {
        send_prefix_mak (c, c->s.pks_pressed, c->s.mak_0 = '\x45');                     /* "0" */
      };
      break;

    case SPECIAL_SZ:
      {
        const uint8_t pks = c->s.pks_pressed ^ c->s.alt_prog_mask;
        if (pks & ALT)
        {
          send_prefix_mak (c, pks & ~ALT, c->s.mak_sz = '\x4e');                        /* "ß" */
        }
        else if (pks & SHIFT)
        {
          send_prefix_mak (c, pks & ~SHIFT, c->s.mak_sz = '\x0e');                      /* "^" */
        }
        else
        {
          send_prefix_mak (c, pks | ALTGR, c->s.mak_sz = '\x5b');                       /* "~" */
        }
      };
      break;

    case SPECIAL_UL:
      if (c->s.pks_pressed & SHIFT)
      {
        send_prefix_mak (c, c->s.pks_pressed, c->s.mak_ul = '\x45');                    /* "=" */
      }
      else
      {
        send_prefix_mak (c, c->s.pks_pressed | SHIFT, c->s.mak_ul = '\x4a');            /* "_" */
      };
      break;

    case SPECIAL_DEL:
      if (c->s.pks_pressed & SHIFT)
      {
        send_prefix_mak (c, c->s.pks_pressed & ~SHIFT, c->s.mak_del = '\x71'|0x100);    /* DEL */
      }
      else
      {
        send_prefix_mak (c, c->s.pks_pressed, c->s.mak_del = '\x66');                   /* BACKSPACE */
      };
      break;

    case SPECIAL_RES:
      send_prefix_mak (c, c->s.pks_pressed, c->s.mak_res = '\x70'|0x100);               /* INS */
      break;

    case SPECIAL_COMMA:
      if (c->s.pks_pressed & SHIFT)
      {
        send_prefix_mak (c, c->s.pks_pressed & ~SHIFT, c->s.mak_comma = '\x61');        /* "<" */
      }
      else
      {
        send_prefix_mak (c, c->s.pks_pressed, c->s.mak_comma = '\x41');                 /* "," */
      };
      break;

    case SPECIAL_PERIOD:
      if (c->s.pks_pressed & SHIFT)
      {
        send_prefix_mak (c, c->s.pks_pressed, c->s.mak_period = '\x61');                /* ">" */
      }
      else
      {
        send_prefix_mak (c, c->s.pks_pressed, c->s.mak_period = '\x49');                /* "." */
      };
      break;

    case SPECIAL_SLASH:
      if (c->s.pks_pressed & SHIFT)
      {
        send_prefix_mak (c, c->s.pks_pressed | SHIFT, c->s.mak_slash = '\x4e');         /* "?" */
      }
      else
      {
        send_prefix_mak (c, c->s.pks_pressed | SHIFT, c->s.mak_slash = '\x3d');         /* "/" */
      };
      break;

    case SPECIAL_COLON:
      if (c->s.pks_pressed & SHIFT)
      {
        send_prefix_mak (c, c->s.pks_pressed | SHIFT, c->s.mak_colon = '\x5b');         /* "*" */
      }
      else
      {
        send_prefix_mak (c, c->s.pks_pressed | SHIFT, c->s.mak_colon = '\x49');         /* ":" */
      };
      break;

    case SPECIAL_PLUS:
      if (c->s.pks_pressed & SHIFT)
      {
        send_prefix_mak (c, c->s.pks_pressed | SHIFT, c->s.mak_plus = '\x41');          /* ";" */
      }
      else
      {
        send_prefix_mak (c, c->s.pks_pressed, c->s.mak_plus = '\x5b');                  /* "+" */
      };
      break;

    case SPECIAL_AE:
      {
        const uint8_t pks = c->s.pks_pressed ^ c->s.alt_prog_mask;
        if (pks & ALT)
        {
          send_prefix_mak (c, pks & ~ALT, c->s.mak_ae = '\x52');                        /* "Ä" */
        }
        else if (pks & SHIFT)
        {
          send_prefix_mak (c, (pks & ~SHIFT) | ALTGR, c->s.mak_ae = '\x3e');            /* "[" */
        }
        else
        {
          send_prefix_mak (c, pks | ALTGR, c->s.mak_ae = '\x3d');                       /* "{" */
        }
      };
      break;

    case SPECIAL_OE:
      {
        const uint8_t pks = c->s.pks_pressed ^ c->s.alt_prog_mask;
        if (pks & ALT)
        {
          send_prefix_mak (c, pks & ~ALT, c->s.mak_oe = '\x4c');                        /* "Ö" */
        }
        else
        {
          send_prefix_mak (c, pks | ALTGR, c->s.mak_oe = '\x4e');                       /* "\" */
        }
      };
      break;

    case SPECIAL_UE:
      {
        const uint8_t pks = c->s.pks_pressed ^ c->s.alt_prog_mask;
        if (pks & ALT)
        {
          send_prefix_mak (c, pks & ~ALT, c->s.mak_ue = '\x54');                        /* "Ü" */
        }
        else if (pks & SHIFT)
        {
          send_prefix_mak (c, (pks & ~SHIFT) | ALTGR, c->s.mak_ue = '\x46');            /* "]" */
        }
        else
        {
          send_prefix_mak (c, pks | ALTGR, c->s.mak_ue = '\x45');                       /* "}" */
        }
      };
      break;

    case SPECIAL_PROG:
      switch (c->s.pks_pressed)
      {
        case NO_PREFIX:
        case ALT:
          c->s.alt_prog_mask = c->s.pks_pressed & ALT;
          break;

        default:
          break;
      };
      break;

    case SPECIAL_00:
      if (!(c->s.pks_pressed & SHIFT))
      {
        send_prefix_mak (c, c->s.pks_pressed, c->s.mak_00 = '\x70');
        send_brk (c, '\x70');
        send_mak (c, '\x70');
      };
      break;

    case SPECIAL_LF:
      c->s.pks_pressed |= ALTGR;
      send_prefix (c, c->s.pks_pressed);
      break;

    case SPECIAL_LOCK:
      break;

    case SPECIAL_NUMMINUS:
      {
        const uint8_t pks = c->s.pks_pressed ^ c->s.alt_prog_mask;
        if (pks & ALT)
        {
          send_prefix_mak (c, pks & ~ALT, c->s.mak_numminus = '\x7b');                  /* "-" */
        }
        else
        {
          send_prefix_mak (c, pks | ALTGR, c->s.mak_numminus = '\x7d'|0x100);           /* PGUP */
        }
      };
      break;

    case SPECIAL_NUMPLUS:
      {
        const uint8_t pks = c->s.pks_pressed ^ c->s.alt_prog_mask;
        if (pks & ALT)
        {
          send_prefix_mak (c, pks & ~ALT, c->s.mak_numplus = '\x79');                   /* "+" */
        }
        else
        {
          send_prefix_mak (c, pks | ALTGR, c->s.mak_numplus = '\x7a'|0x100);            /* PGDN */
        }
      };
      break;

    case SPECIAL_NUMSTAR:
      {
        const uint8_t pks = c->s.pks_pressed ^ c->s.alt_prog_mask;
        if (pks & ALT)
        {
          send_prefix_mak (c, pks & ~ALT, c->s.mak_numstar = '\x7c');                   /* "*" */
        }
        else
        {
          send_prefix_mak (c, pks | ALTGR, c->s.mak_numstar = '\x6c'|0x100);            /* HOME */
        }
      };
      break;

    case SPECIAL_NUMSLASH:
      {
        const uint8_t pks = c->s.pks_pressed ^ c->s.alt_prog_mask;
        if (pks & ALT)
        {
          send_prefix_mak (c, pks & ~ALT, c->s.mak_numslash = '\x4a'|0x100);            /* "./." */
        }
        else
        {
          send_prefix_mak (c, pks | ALTGR, c->s.mak_numslash = '\x69'|0x100);           /* END */
        }
      };
      break;

    default:
      break;
  }
}


static void brk_special (ScancodeContext *c, const __flash KeyTableEntry *k)
{
  switch (k->m)
  {
    case SPECIAL_CTRL:
      c->s.pks_pressed &= ~CTRL;
      break;

    case SPECIAL_SHIFT:
      c->s.pks_pressed &= ~SHIFT;
      break;

    case SPECIAL_REP:
      c->s.pks_pressed &= ~ALT;
      break;

    case SPECIAL_3:
      send_brk (c, c->s.mak_3);
      c->s.mak_3 = '\0';
      break;

    case SPECIAL_7:
      send_brk (c, c->s.mak_7);
      c->s.mak_7 = '\0';
      break;

    case SPECIAL_0:
      send_brk (c, c->s.mak_0);
      c->s.mak_0 = '\0';
      break;

    case SPECIAL_SZ:
      send_brk (c, c->s.mak_sz);
      c->s.mak_sz = '\0';
      break;

    case SPECIAL_UL:
      send_brk (c, c->s.mak_ul);
      c->s.mak_ul = '\0';
      break;

    case SPECIAL_DEL:
      send_brk (c, c->s.mak_del);
      c->s.mak_del = '\0';
      break;

    case SPECIAL_RES:
      send_brk (c, c->s.mak_res);
      c->s.mak_res = '\0';
      break;

    case SPECIAL_COMMA:
      send_brk (c, c->s.mak_comma);
      c->s.mak_comma = '\0';
      break;

    case SPECIAL_PERIOD:
      send_brk (c, c->s.mak_period);
      c->s.mak_period = '\0';
      break;

    case SPECIAL_SLASH:
      send_brk (c, c->s.mak_slash);
      c->s.mak_slash = '\0';
      break;

    case SPECIAL_COLON:
      send_brk (c, c->s.mak_colon);
      c->s.mak_colon = '\0';
      break;

    case SPECIAL_PLUS:
      send_brk (c, c->s.mak_plus);
      c->s.mak_plus = '\0';
      break;

    case SPECIAL_AE:
      send_brk (c, c->s.mak_ae);
      c->s.mak_ae = '\0';
      break;

    case SPECIAL_OE:
      send_brk (c, c->s.mak_oe);
      c->s.mak_oe = '\0';
      break;

    case SPECIAL_UE:
      send_brk (c, c->s.mak_ue);
      c->s.mak_ue = '\0';
      break;

    case SPECIAL_PROG:
      break;

    case SPECIAL_00:
      send_brk (c, c->s.mak_00);
      c->s.mak_00 = '\0';
      break;

    case SPECIAL_LF:
      c->s.pks_pressed &= ~ALTGR;
      break;

    case SPECIAL_LOCK:
      break;

    case SPECIAL_NUMMINUS:
      send_brk (c, c->s.mak_numminus);
      c->s.mak_numminus = '\0';
      break;

    case SPECIAL_NUMPLUS:
      send_brk (c, c->s.mak_numplus);
      c->s.mak_numplus = '\0';
      break;

    case SPECIAL_NUMSTAR:
      send_brk (c, c->s.mak_numstar);
      c->s.mak_numstar = '\0';
      break;

    case SPECIAL_NUMSLASH:
      send_brk (c, c->s.mak_numslash);
      c->s.mak_numslash = '\0';
      break;

    default:
      break;
  }
}


static void mak_internal (ScancodeContext *c, const __flash KeyTableEntry *k)
{
  c->dbg (PSTR("mak_internal %p\r\n"), k);
  if (k->f & SPECIAL)
  {
    mak_special (c, k);
  }
  else
  {
    send_prefix_mak (c, c->s.pks_pressed, k->m | ((k->f & E0_PREFIX) ? 0x100 : 0));
  }
}


static void brk_internal (ScancodeContext *c, const __flash KeyTableEntry *k)
{
  c->dbg (PSTR("brk_internal %p\r\n"), k);
  if (k->f & SPECIAL)
  {
    brk_special (c, k);
  }
  else
  {
    send_brk (c, k->m | ((k->f & E0_PREFIX) ? 0x100 : 0));
  };
  send_prefix (c, c->s.pks_pressed);
}


static void mak (void *s, int8_t idx)
{
  ScancodeContext *c = s;

  if (idx < 0 || idx > LENGTH(keytable))
  {
    c->dbg (PSTR("mak: %d out of range\r\n"), idx);
  };

  mak_internal (c, &keytable[idx]);
}


static void brk (void *s, int8_t idx)
{
  ScancodeContext *c = s;

  if (idx < 0 || idx > LENGTH(keytable))
  {
    c->dbg (PSTR("brk: %d out of range\r\n"), idx);
  };

  brk_internal (c, &keytable[idx]);
}


void next_scancode (ScancodeContext *c)
{
  c->scan (c, mak, brk, c->dbg, c->timing);
}


void initstate_scancode (ScancodeContext *c)
{
  memset (&c->s, 0, sizeof c->s);
}


void initif_scancode (ScancodeContext *c,
                      void (*dbg) (const __flash char *fmt, ...),
                      void (*scan) (void *s,
                                    void (*mak) (void *s, int8_t idx),
                                    void (*brk) (void *s, int8_t idx),
                                    void (*dbg) (const __flash char *fmt, ...),
                                    const KeyboardTiming *timing),
                      struct sc_CBuf *scancodes,
                      const KeyboardTiming *timing)
{
  c->dbg = dbg;
  c->scan = scan;
  c->scancodes = scancodes;
  c->timing = timing;
}
