/* usrP2vLib.c - P2V Runtime Configution Code */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,31jan97,jmb  Added support for timer array
01c,12jun96,jmb  Added p2vMessage to list of function pointers because
                 routines using I/O are moved to a separated file 
		 for scalability.
01b,03may96,jmb  fixed a comment and turned off "unsupported" messages.
01a,02may96,jmb  created
*/

#include "vxWorks.h"
#include "p2vLib.h"
#include "configP2v.h"
#ifdef DEBUG
#include "logLib.h"
#include "taskLib.h"
#endif

P2V_TABLE_NODE p2vFixQArray[P2V_MAX_FIX_Q];
int p2vMaxFixQ = P2V_MAX_FIX_Q;

P2V_TABLE_NODE p2vVarQArray[P2V_MAX_VAR_Q];
int p2vMaxVarQ = P2V_MAX_VAR_Q;

P2V_TABLE_NODE p2vSemArray[P2V_MAX_SEM];
int p2vMaxSem = P2V_MAX_SEM;

P2V_TABLE_NODE p2vPartArray[P2V_MAX_PART];
int p2vMaxPart = P2V_MAX_PART;

P2V_TABLE_NODE p2vRgnArray[P2V_MAX_RGN];
int p2vMaxRgn = P2V_MAX_RGN;

#ifdef P2V_USE_TIMER_ARRAY
P2V_TIMER_NODE p2vTimerArray[P2V_MAX_TIMERS];
int p2vMaxTimers = P2V_MAX_TIMERS;
#endif

int p2vStackBump = P2V_STACK_BUMP;

BOOL p2vStrictMode = P2V_STRICT_MODE;

#ifdef INCLUDE_P2VLIB_ALL
FUNCPTR p2vFuncArray [] =
   {
   (FUNCPTR) t_start,
   (FUNCPTR) ev_send,
   (FUNCPTR) sm_ident,
   (FUNCPTR) rn_ident,
   (FUNCPTR) pt_ident,
   (FUNCPTR) q_ident,
   (FUNCPTR) tm_set,
   (FUNCPTR) as_catch,
   (FUNCPTR) p2vTblAdd,
   (FUNCPTR) p2vMessage
   };
#endif

/*****************************************************************************
*
* p2vUnsupported - error message routine for unsupported features.
*
* This routine is called by the library when an unsupported option
* or argument will cause an error code to be returned.  In DEBUG mode, this
* routine calls logMsg, to print the cause of the error, then suspends
* the task.  If DEBUG is not defined, this routine does nothing.
* You can modify this routine to handle errors in some other way, but
* remember to keep the functional interface as is.
*
* INCLUDE FILES: p2vLib.h
*
* RETURNS:  
* <void>
*/
void p2vUnsupported 
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

