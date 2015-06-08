/* selectLib.h - select header file */

/* Copyright 1984-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
02d,18sep01,aeg  added selWakeupListTerm(); moved private definitions to
		 private/selectLibP.h; added S_selectLib_NO_SELECT_CONTEXT
		 and S_selectLib_WIDTH_OUT_OF_RANGE error codes.
02c,22sep92,rrr  added support for c++
02b,19aug92,smb  changed systime.h to sys/times.h
02a,04jul92,jcf  cleaned up.
01k,26may92,rrr  the tree shuffle
01j,04oct91,rrr  passed through the ansification filter
		  -removed comma from end of enum
		  -changed includes to have absolute path from h/
		  -changed VOID to void
		  -changed copyright notice
01i,25oct90,dnw  changed utime.h to systime.h.
01h,19oct90,shl  added #include of "utime.h".
01g,05oct90,shl  added ANSI function prototypes.
                 made #endif ANSI style.
                 added copyright notice.
01f,07aug90,shl  added function declarations comment.
01e,01aug90,jcf  clean up.
01d,13jul90,rdc  added original read and write fd_sets to wakeup node struct.
01c,30apr90,gae  added HIDDEN comments to internal data structures.
01b,20mar90,jcf  cleanup.
01a,26jan90,rdc  written
*/

#ifndef __INCselectLibh
#define __INCselectLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "sys/types.h"
#include "sys/times.h"

/* error codes */

#define S_selectLib_NO_SELECT_SUPPORT_IN_DRIVER  (M_selectLib | 1)
#define S_selectLib_NO_SELECT_CONTEXT		 (M_selectLib | 2)
#define S_selectLib_WIDTH_OUT_OF_RANGE		 (M_selectLib | 3)

typedef enum
    {
    SELREAD,
    SELWRITE
    } SELECT_TYPE;

#include "private/selectLibP.h"

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern SELECT_TYPE  selWakeupType 	(SEL_WAKEUP_NODE *pWakeupNode);
extern STATUS 	    selNodeAdd 	      	(SEL_WAKEUP_LIST *pWakeupList,
			    		 SEL_WAKEUP_NODE *pWakeupNode);
extern STATUS 	    selNodeDelete 	(SEL_WAKEUP_LIST *pWakeupList,
			       		 SEL_WAKEUP_NODE *pWakeupNode);
extern int 	    selWakeupListLen 	(SEL_WAKEUP_LIST *pWakeupList);
extern void 	    selWakeup 		(SEL_WAKEUP_NODE *pWakeupNode);
extern void 	    selWakeupAll 	(SEL_WAKEUP_LIST *pWakeupList, 
					 SELECT_TYPE type);
extern void 	    selWakeupListInit 	(SEL_WAKEUP_LIST *pWakeupList);
extern void 	    selWakeupListTerm 	(SEL_WAKEUP_LIST *pWakeupList);
extern int	    select 		(int width, fd_set *pReadFds, 
					 fd_set *pWriteFds, fd_set *pExceptFds,
					 struct timeval *pTimeOut);

#else	/* __STDC__ */

extern SELECT_TYPE  selWakeupType 	();
extern STATUS 	    selNodeAdd 	      	();
extern STATUS 	    selNodeDelete 	();
extern int 	    selWakeupListLen 	();
extern void 	    selWakeup 		();
extern void 	    selWakeupAll 	();
extern void 	    selWakeupListInit 	();
extern void 	    selWakeupListTerm 	();
extern int	    select 		();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCselectLibh */
