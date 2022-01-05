/*
 * $Header: /home/playground/src/atmega32/op2/uart.c,v 33f135374e37 2022/01/04 15:29:52 nkoch $
 */


#include <inttypes.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include "common.h"
#include "switches.h"
#define CBUF_ID         u_
#define CBUF_LEN        UART_CBUF_LEN
#define CBUF_TYPE       uint8_t
#include "cbuf.h"
#include "uart.h"


static struct u_CBuf uart_rxd, uart_txd;


static void dummy_sleep (void)
{
}


static void dummy_event (uint8_t c)
{
}


void (*uart_sleep) (void) = dummy_sleep;
void (*uart_inevent) (uint8_t c) = dummy_event;


// UART Empfangsinterrupt
ISR (USART_RXC_vect)
{
  const uint8_t status = UCSRA;
  uint8_t c = UDR;

  if (status & (_BV(FE) | _BV(DOR) | _BV(PE)))
  {
    c = '~';
  };

  if (!u_full (&uart_rxd))
  {
    u_put (&uart_rxd, c);
  }
  else
  {
    u_set_overrun (&uart_rxd);
  };
  uart_inevent (c);
}


// UART Sendeinterrupt
ISR (USART_UDRE_vect)
{
  if (!u_empty (&uart_txd))
  {
    UDR = u_get (&uart_txd);
  }
  else
  {
    UCSRB &= ~ _BV(UDRIE);
  }
}


int uart_getc_nowait (void)
{
  if (!u_empty (&uart_rxd))
  {
    return u_get (&uart_rxd);
  };
  return -1;
}


uint8_t uart_getc (void)
{
  while (u_empty (&uart_rxd))
  {
    uart_sleep ();
  };
  return u_get (&uart_rxd);
}


static inline void uart_putc_ (uint8_t c)
{
  u_put (&uart_txd, c);
  UCSRB |= _BV(UDRIE);
}


bool uart_putc_nowait (uint8_t c)
{
  if (u_full (&uart_txd))
  {
    return false;
  };
  uart_putc_ (c);
  return true;
}


void uart_putc (uint8_t c)
{
  while (u_full (&uart_txd))
  {
    uart_sleep ();
  };
  uart_putc_ (c);
}


int uart_in_peek (void)
{
  return u_peek (&uart_rxd);
}


bool uart_in_empty ()
{
  return u_empty (&uart_rxd);
}


uint16_t uart_in_used ()
{
  return u_used (&uart_rxd);
}


uint16_t uart_out_avail ()
{
  return u_avail (&uart_rxd);
}


void uart_drain ()
{
  while (!u_empty (&uart_txd))
  {
    uart_sleep ();
  }
}


void uart_flush ()
{
  while (uart_getc_nowait () != -1)
  {
  }
}


static const uint8_t baudrates[][2] =
{
#define BAUD    19200
#include <util/setbaud.h>
#if USE2X
#error
#endif
  { UBRRH_VALUE, UBRRL_VALUE },
#undef BAUD

#define BAUD    9600
#include <util/setbaud.h>
#if USE2X
#error
#endif
  { UBRRH_VALUE, UBRRL_VALUE },
#undef BAUD

#define BAUD    4800
#include <util/setbaud.h>
#if USE2X
#error
#endif
  { UBRRH_VALUE, UBRRL_VALUE },
#undef BAUD

#define BAUD    2400
#include <util/setbaud.h>
#if USE2X
#error
#endif
  { UBRRH_VALUE, UBRRL_VALUE },
#undef BAUD
};


void uart_init (void)
{
  u_init (&uart_rxd);
  u_init (&uart_txd);

  const uint8_t idx = sw_baudrate () % LENGTH(baudrates);
  UBRRH = baudrates[idx][0];
  UBRRL = baudrates[idx][1];
  UCSRA &= ~_BV(U2X);

  // UART Receiver und Transmitter anschalten, Receive-Interrupt aktivieren
  // ASYNC, 8N1, asynchron
  UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
  UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);

  // Flush Receive-Buffer
  do
  {
    UDR;
  }
  while (UCSRA & _BV(RXC));

  // Rücksetzen von Receive und Transmit Complete-Flags
  UCSRA |= _BV(RXC) | _BV(TXC);
}


void uart_puts (const char *s)
{
  while (*s)
  {
    uart_putc (*s++);
  }
}


void uart_puts_P (const __flash char *s)
{
  char c;

  while ((c = *s++))
  {
    uart_putc (c);
  }
}


void uart_bs (void)
{
  uart_puts_P (PSTR ("\b \b"));
}


void uart_crlf (void)
{
  uart_puts_P (PSTR ("\r\n"));
}


void uart_putsln (const char *s)
{
  uart_puts (s);
  uart_crlf ();
}


void uart_putsln_P (const __flash char *s)
{
  uart_puts_P (s);
  uart_crlf ();
}


void uart_vsnprintf_P (const __flash char *fmt, va_list ap)
{
  char temp[128];
  vsnprintf_P (temp, sizeof temp, fmt, ap);
  uart_puts (temp);
}


void uart_printf_P (const __flash char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  uart_vsnprintf_P (fmt, ap);
  va_end (ap);
}
