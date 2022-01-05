/*
 * $Header: /home/playground/src/atmega32/op2/typematic.c,v ec5d23a8d2e5 2022/01/02 20:01:14 nkoch $
 */


#include "kbdscan.h"




void typematic_to_timing (uint8_t typematic, KeyboardTiming *timing)
{
  static const long initial_delays[] =
  {
     250,
     500,
     750,
    1000,
  };
  static const long delays[] =
  {
     33, /* 30.0 cps */
     37, /* 26.7 cps */
     42, /* 24.0 cps */
     46, /* 21.8 cps */
     48, /* 20.7 cps */
     54, /* 18.5 cps */
     58, /* 17.1 cps */
     63, /* 16.0 cps */
     67, /* 15.0 cps */
     75, /* 13.3 cps */
     83, /* 12.0 cps */
     92, /* 10.9 cps */
    100, /* 10.0 cps */
    107, /*  9.2 cps */
    116, /*  8.6 cps */
    125, /*  8.0 cps */
    133, /*  7.5 cps */
    149, /*  6.7 cps */
    167, /*  6.0 cps */
    182, /*  5.5 cps */
    200, /*  5.0 cps */
    217, /*  4.6 cps */
    233, /*  4.3 cps */
    250, /*  4.0 cps */
    270, /*  3.7 cps */
    303, /*  3.3 cps */
    333, /*  3.0 cps */
    370, /*  2.7 cps */
    400, /*  2.5 cps */
    435, /*  2.3 cps */
    476, /*  2.1 cps */
    500, /*  2.0 cps */
  };
  timing->debounce = 5;
  timing->initial_delay = initial_delays[(typematic >> 5) & 0x3];
  timing->delay = delays[typematic & 0x1f];
}


void default_timing (KeyboardTiming *timing)
{
  timing->debounce = 5;
  timing->initial_delay = 500;
  timing->delay = 92;
}
