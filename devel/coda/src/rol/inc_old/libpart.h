
/* libpart.h - header file for the memory allocation system; keep all structure
               elements (for danode) aligned on 4-byte boundaries, this
               insures that malloc() will create event buffer nodes that are
               4-byte aligned */

/* Sergey: to use 64bit transfer must be 8-byte aligned ??? */

#ifndef  __INC_MEM_PART__
#define __INC_MEM_PART__

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define maximum(a,b) (a<b ? b : a)

/* Sergey: moved to 'crldefs.h'
extern char	*malloc();
*/

#include "mempart.h"

/* global data */


#endif
