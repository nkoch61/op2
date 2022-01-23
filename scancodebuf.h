/*
 * $Header: /home/playground/src/atmega32/op2/scancodebuf.h,v 2822ec43b4ec 2022/01/06 19:33:35 nkoch $
 */


#ifndef _SCANCODEBUF_H
#define _SCANCODEBUF_H


#include "scancode.h"


#undef  CBUF_ID
#undef  CBUF_LEN
#undef  CBUF_TYPE
#define CBUF_ID         sc_
#define CBUF_LEN        20
#define CBUF_TYPE       Scancode
#include "cbuf.h"


#endif
