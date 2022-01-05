/* timer.c */


#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "timer.h"
#include "defs.h"


volatile long millisecs;


/**********************
 * Millisekundentimer *
 **********************/

long now (void)
{
  long ms;

  do
  {
    ms = millisecs;
  }
  while (ms != millisecs);
  return ms;
}


/***************
 * Zeitmessung *
 ***************/

void mark (long *since)
{
  *since = now ();
}


long elapsed (long *since)
{
  return now () - *since;
}


long elapsed_mark (long *since)
{
  const long n = now ();
  const long dif = n - *since;
  *since = n;
  return dif;
}


bool is_elapsed (long *since, long howlong)
{
  return elapsed (since) - howlong >= 0;
}


bool is_elapsed_mark (long *since, long howlong)
{
  const long n = now ();
  const long dif = n - *since;
  const bool is_elapsed = dif - howlong >= 0;
  if (is_elapsed)
  {
    *since = n;
  };
  return is_elapsed;
}


/********************
 * SLEEP/POWER DOWN *
 ********************/

void sleep (void)
{
  cli ();
  set_sleep_mode (SLEEP_MODE_IDLE);
  sleep_enable ();
  sei ();
  sleep_cpu ();
  sleep_disable ();
}


void sleep_until (const long ms)
{
  do
  {
    sleep ();
  }
  while ((now () - ms) < 0);
}


void sleep_for (const long ms)
{
  sleep_until (now () + ms);
}
