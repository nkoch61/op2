/*
 * $Header: /home/playground/src/atmega32/op2/common.h,v c58c54c433e5 2021/12/24 16:35:10 nkoch $
 */


#ifndef _COMMON_H
#define _COMMON_H


#define CAT(a,b)                a##b
#define XCAT(a,b)               CAT (a,b)
#define STR(x)                  #x
#define XSTR(x)                 STR(x)
#define ASIZE(a)                (sizeof (a) / sizeof ((a)[0]))
#define LENGTH(a)               (ASIZE (a))
#define FIELD(t, f)             (((t*)0)->f)
#define FIELD_SIZEOF(t, f)      (sizeof(FIELD(t, f)))

#define RNDDIV(x, y)    (((x) + ((y) >> 1)) / (y))


#define FSTR(x)         (const __flash char []) {x}


#endif
