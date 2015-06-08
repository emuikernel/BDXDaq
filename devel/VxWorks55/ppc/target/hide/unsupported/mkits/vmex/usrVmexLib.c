/* usrVmexLib.c - VMEX Runtime Configution Code */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,04feb97,jmb  Added timer array feature.
01c,12jun96,jmb  Added vmexMessage to list of function pointers because
                 routines using I/O are moved to a separated file 
		 for scalability.
01b,03may96,jmb  fixed a comment and turned off "unsupported" messages.
01a,02may96,jmb  created
*/

#include "vxWorks.h"
#include "vmexLib.h"
#include "configVmex.h"
#ifdef DEBUG
#include "logLib.h"
#include "taskLib.h"
#endif

VMEX_TABLE_NODE vmexFixQArray[VMEX_MAX_FIX_Q];
int vmexMaxFixQ = VMEX_MAX_FIX_Q;

VMEX_TABLE_NODE vmexVarQArray[VMEX_MAX_VAR_Q];
int vmexMaxVarQ = VMEX_MAX_VAR_Q;

VMEX_TABLE_NODE vmexSemArray[VMEX_MAX_SEM];
int vmexMaxSem = VMEX_MAX_SEM;

VMEX_TABLE_NODE vmexPartArray[VMEX_MAX_PART];
int vmexMaxPart = VMEX_MAX_PART;

VMEX_TABLE_NODE vmexRgnArray[VMEX_MAX_RGN];
int vmexMaxRgn = VMEX_MAX_RGN;

#ifdef VMEX_USE_TIMER_ARRAY
VMEX_TIMER_NODE vmexTimerArray[VMEX_MAX_TIMERS];
int vmexMaxTimers = VMEX_MAX_TIMERS;
#endif

int vmexStackBump = VMEX_STACK_BUMP;

BOOL vmexStrictMode = VMEX_STRICT_MODE;

#ifdef INCLUDE_VMEXLIB_ALL
FUNCPTR vmexFuncArray [] =
   {
   (FUNCPTR) t_start,
   (FUNCPTR) ev_send,
   (FUNCPTR) sm_ident,
   (FUNCPTR) rn_ident,
   (FUNCPTR) pt_ident,
   (FUNCPTR) q_ident,
   (FUNCPTR) tm_set,
   (FUNCPTR) as_catch,
   (FUNCPTR) vmexTblAdd,
   (FUNCPTR) vmexMessage
   };
#endif

/*****************************************************************************
*
* vmexUnsupported - error message routine for unsupported features.
*
* This routine is called by the library when an unsupported option
* or argument will cause an error code to be returned.  In DEBUG mode, this
* routine calls logMsg, to print the cause of the error, then suspends
* the task.  If DEBUG is not defined, this routine does nothing.
* You can modify this routine to handle errors in some other way, but
* remember to keep the functional interface as is.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:  
* <void>
*/
void vmexUnsupported 
    (
    char * fmt,
    int arg1,
    int arg2,
    int arg3,
    int arg4,
    int arg5,
    int arg6
    )
    {
#ifdef DEBUG
    logMsg (fmt, arg1, arg2, arg3, arg4, arg5, arg6);
    taskSuspend (0);
#endif
    }

