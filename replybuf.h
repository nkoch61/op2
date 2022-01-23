/*
 * $Header: /home/playground/src/atmega32/op2/replybuf.h,v 2822ec43b4ec 2022/01/06 19:33:35 nkoch $
 */


#ifndef _REPLYBUF_H
#define _REPLYBUF_H

#undef  CBUF_ID
#undef  CBUF_LEN
#undef  CBUF_TYPE
#define CBUF_ID         reply_
#define CBUF_LEN        4
#define CBUF_TYPE       uint8_t
#include "cbuf.h"


#endif
