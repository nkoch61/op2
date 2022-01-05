/*
 * $Header: /home/playground/src/atmega32/op2/cmdint.h,v c58c54c433e5 2021/12/24 16:35:10 nkoch $
 */


#ifndef _CMDINT_H
#define _CMDINT_H


#include <stdint.h>
#include <stdbool.h>


struct CmdIntCallback
{
  uint8_t (*getc_f) (void);
  void (*putc_f) (uint8_t c);
  void (*bs_f) (void);
  void (*crlf_f) (void);
  void (*cancel_f) (void);
  void (*prompt_f) (void);
  int8_t (*interp_f) (int8_t argc, char **argv);
  int8_t (*line_f) (void);
};
typedef struct CmdIntCallback CmdIntCallback_t;


extern int8_t cmdint_getline (const CmdIntCallback_t *callback,
                             char *ptr,
                             char *bptr,
                             uint8_t len);
extern void cmdint (const CmdIntCallback_t *callback);


#endif
