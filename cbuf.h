/*
 * $Header: /home/playground/src/atmega32/op2/cbuf.h,v fb3150a8993c 2021/08/21 18:14:20 nkoch $
 */


#include "common.h"
#include <stdbool.h>
#include <stdint.h>


#ifndef CBUF_ID
#error
#endif
#ifndef CBUF_LEN
#error
#endif
#if CBUF_LEN > 256
#error
#endif
#ifndef CBUF_TYPE
#error
#endif


struct XCAT(CBUF_ID,CBuf)
{
  CBUF_TYPE data[CBUF_LEN];
  volatile uint8_t put, get, overrun;
};


static uint8_t XCAT(CBUF_ID,used) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  return (cbuf->put - cbuf->get + CBUF_LEN) % CBUF_LEN;
}

static uint8_t XCAT(CBUF_ID,avail) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  return CBUF_LEN - XCAT(CBUF_ID,used) (cbuf) - 1;
}

static bool XCAT(CBUF_ID,empty) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  return cbuf->get == cbuf->put;
}

static bool XCAT(CBUF_ID,full) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  return (cbuf->get == 0 && cbuf->put == CBUF_LEN -1)
         ||
         (cbuf->put == cbuf->get - 1);
}

static CBUF_TYPE XCAT(CBUF_ID,peek) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  return cbuf->data[cbuf->get];
}

static void XCAT(CBUF_ID,less) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  cbuf->get = (cbuf->get + 1) % CBUF_LEN;
}

static CBUF_TYPE XCAT(CBUF_ID,get) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  const CBUF_TYPE v = XCAT(CBUF_ID,peek) (cbuf);
  XCAT(CBUF_ID,less) (cbuf);
  return v;
}

static void XCAT(CBUF_ID,poke) (struct XCAT(CBUF_ID,CBuf) *cbuf, CBUF_TYPE v)
{
  cbuf->data[cbuf->put] = v;
}

static void XCAT(CBUF_ID,more) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  cbuf->put = (cbuf->put + 1) % CBUF_LEN;
}

static void XCAT(CBUF_ID,put) (struct XCAT(CBUF_ID,CBuf) *cbuf, CBUF_TYPE v)
{
  XCAT(CBUF_ID,poke) (cbuf, v);
  XCAT(CBUF_ID,more) (cbuf);
}

static void XCAT(CBUF_ID,clear) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  cbuf->put = cbuf->get = cbuf->put % CBUF_LEN;
}

static void XCAT(CBUF_ID,init) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  cbuf->put = cbuf->get = cbuf->overrun = 0;
}

static void XCAT(CBUF_ID,set_overrun) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  ++cbuf->overrun;
}

static bool XCAT(CBUF_ID,get_overrun) (struct XCAT(CBUF_ID,CBuf) *cbuf)
{
  const bool overrun = cbuf->overrun > 0;

  if (overrun)
  {
    --cbuf->overrun;
    return true;
  };
  return false;
}
