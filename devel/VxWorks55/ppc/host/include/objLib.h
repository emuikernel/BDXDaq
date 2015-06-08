/* objLib.h - object management library header */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01p,25may98,fle  fixed bug in OBJ_VERIFY
01o,15mar95,p_m  changed #include "vxWorks.h" to #include "host.h".
01n,01mar95,pad  removed old style error codes.
01m,20jan95,jcf  made more portable.
01l,22sep92,rrr  added support for c++
01k,29jul92,jcf  added errno.h include.
01j,04jul92,jcf  cleaned up.
01i,26may92,rrr  the tree shuffle
01h,04dec91,rrr  some ansi cleanup.
01g,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed VOID to void
		  -changed copyright notice
01f,10jun91.del  added pragma for gnu960 alignment.
01e,05apr91,gae  added NOMANUAL to avoid fooling mangen.
01d,05oct90,shl  added ANSI function prototypes.
                 made #endif ANSI style.
		 added copyright notice.
01c,15jul90,dnw  changed objAlloc() from (char*) to (void*)
		 added objAllocExtra()
01b,26jun90,jcf  added objAlloc()/objFree().
		 added standard object status codes.
01a,10dec89,jcf  written.
*/

#ifndef __INCobjLibh
#define __INCobjLibh

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>

#include "host.h"


/* defines */

#define OBJ_VALID	0x12345678
#define OBJ_INVALID	0xdeadbeef


/* typedefs */

typedef struct obj_core				/* OBJ_CORE */
    {
    int valid;					/* validation field */
    } OBJ_CORE;					/* object core */

typedef struct obj_core  *OBJ_ID;		/* object id */


/*******************************************************************************
*
* OBJ_VERIFY - check the validity of an object pointer
*
* This macro verifies the existence of the specified object by comparing the
* object's classId with the expected class id.  If the object does not check
* out, errno is set with the appropriate invalid id status.
*
* RETURNS: OK or ERROR if invalid object id
*
* NOMANUAL
*/

#define OBJ_VERIFY(objId)						\
    (									\
    ( (objId) && ((OBJ_ID)(objId))->valid == OBJ_VALID) ?		\
	    OK								\
	:							   	\
	    (errno = WTX_ERR_OBJ_INVALID_OBJECT, ERROR)			\
    )

#ifdef __cplusplus
}
#endif

#endif /* __INCobjLibh */
