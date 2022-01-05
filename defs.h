/*
 * $Header: /home/playground/src/atmega32/op2/defs.h,v 0e70a97349fc 2022/01/02 14:30:24 nkoch $
 */


#ifndef _DEFS_H
#define _DEFS_H


#define PORT_KBDIN              PORTA
#define PIN_KBDIN               PINA
#define DDR_KBDIN               DDRA
#define MASK_KBDIN              (_BV(PA0) | _BV(PA1) | _BV(PA2) | _BV(PA3) | _BV(PA4) | _BV(PA5) | _BV(PA6) | _BV(PA7))

#define PORT_SWITCHES           PORTB
#define PIN_SWITCHES            PINB
#define DDR_SWITCHES            DDRB
#define MASK_SWITCHES           (_BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB3) | _BV(PB4))

#define PORT_KBDOUT             PORTC
#define PIN_KBDOUT              PINC
#define DDR_KBDOUT              DDRC
#define MASK_KBDOUT             (_BV(PC0) | _BV(PC1) | _BV(PC2) | _BV(PC3) | _BV(PC4))

#define PORT_CLK                PORTD
#define PIN_CLK                 PIND
#define DDR_CLK                 DDRD
#define MASK_CLK                _BV(PD3)

#define PORT_DTA                PORTD
#define PIN_DTA                 PIND
#define DDR_DTA                 DDRD
#define MASK_DTA                _BV(PD2)

#define UNUSED_C                (_BV(PC5) | _BV(PC6))

#define PORT_DEBUG              PORTD
#define DDR_DEBUG               DDRD
#define MASK_DEBUG              (_BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7))
#define SHIFT_DEBUG             4

#define PORT_INTR               PORTC
#define DDR_INTR                DDRC
#define MASK_INTR               _BV(PC7)

#define ROWS                    16
#define COLUMNS                 8

#define LO      false
#define HI      true


#endif
