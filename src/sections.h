#ifndef __ILARNSECTIONS_H__
#define __ILARNSECTIONS_H__

// section of code formerly before start.  more-or-less.   very full.
#define SEC_1 __attribute__ ((section ("sec_1")))
// section of code formerly after start (except main.c or so).  full.
#define SEC_2 __attribute__ ((section ("sec_2")))
// yet another section of code.  3 is pretty empty...
#define SEC_3 __attribute__ ((section ("sec_3")))
// yet another section of code.  less-empty.
#define SEC_4 __attribute__ ((section ("sec_4")))
// section of code formerly in the library
//#define SEC_L __attribute__ ((section ("sec_5")))
#define SEC_5 __attribute__ ((section ("sec_5")))

#endif
