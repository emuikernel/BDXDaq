/* as_Lib.c - Asynchronous Signal Routines. */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,21jan97,jmb  simplify ASR wrapper...get rid of loop and let POSIX
		 signal functions handle queuing.
01c,10apr96,jmb  removed initialization of ASEnable from as_catch
01b,09apr96,jmb  added asrBusy
01a,20mar96,cym  created
*/

#include "taskLib.h"
#include "vmexLib.h"
#include "intLib.h"
#include "sigLib.h"
#include "errnoLib.h"


LOCAL void ASRWrapper ( int dummy1, int dummy2, void * dummy3);

/*****************************************************************************
*                                         
*   as_return -- return from an ASR
*
*   This function is unnecessary.  It is implemented in the vmexLib.h
*   header file as:  #define as_return return(0)
*
*   When a function returns from an ASR,
*   control returns to ASRWrapper.  The wrapper is connected to the kernel as
*   a POSIX signal handler. The ASR doesn't need to worry about restoring
*   registers and it can be entered and exited using standard C.
*  
*   RETURNS:
*   <void>
*/

/*****************************************************************************
*
* ASRWrapper - Asynchronous signal handler wrapper routine 
*
* This is the function that gets called when a vmex task gets a
* signal (actually an event) from an as_send.
* It removes events from the TCB extension and passes them 
* to the user-specified signal handler.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* <void>
*/

LOCAL void ASRWrapper
    (
    int dummy1,				/* vxWorks passes signal # here */
    int dummy2,				/* signal code here */
    void *dummy3			/* and a sigContext structure here */
    )
    {
    WIND_TCB *	pTcb;			/* the control block of this task */
    VMEX_SHADOW_TCB *	pShadow;		/* TCB extension of the task */
    UINT32 	taskMode;		/* old, task level mode of the task */
    UINT32 	curASEvents;		/* signals being handled currently */
    int 	intKey;			/* key to unlock interrupts */

    /* get this tasks control block and TCB extension */

    pTcb = taskTcb (0);			
    pShadow = vmexTcb (0);

    if (pShadow->pASR == NULL)   /* make sure as_catch has been called */
	return;

    if (pShadow->ASEvents == 0)    /* Don't trigger on a 0 event */
        return;


    taskMode = pShadow->currentMode;    /* save the old task mode */

    /* go into the ASR mode */

    pShadow->currentMode = pShadow->ASMode;

    if(pShadow->ASMode & T_NOPREEMPT)
        {
        if (pTcb->lockCnt == 0) 
	    taskLock();				/* TASK LOCK*/
	}
    else
        while (pTcb->lockCnt) 
	    taskUnlock();			/* TASK UNLOCK */

    intKey = intLock();				/* LOCK INTERRUPTS */

    curASEvents = pShadow->ASEvents;     /* pick up the events */

    pShadow->ASEvents = 0;               /* clear pending events */
	   
    intUnlock (intKey);			/* UNLOCK INTERRUPTS */

    pShadow->asrBusy = TRUE;    /* Set flag so that t_mode calls in ASR
				   don't change sigmask. */

    (pShadow->pASR) (0, 0, 0, (curASEvents));   /* call the ASR */

    pShadow->asrBusy = FALSE;

    /* restore original mode */

    if (taskMode & T_NOPREEMPT)
	{
        if (pTcb->lockCnt == 0) 
	    taskLock();
        }
    else
        while(pTcb->lockCnt) 
	    taskUnlock();

    pShadow->ASMode = pShadow->currentMode;
    pShadow->currentMode = taskMode;
    return;
    }

/*****************************************************************************
*
* as_catch - attach an asynchronous signal handler routine
*
* INCLUDE FILES: vmexLib.h 
*
* RETURNS: 
* EOK if successful.
* ERROR if calling task is not a vmex task or if signal function fails.
*/

UINT32 as_catch
    (
    FUNCPTR pASR,			/* signal handler function to install */
    UINT32 mode			/* mode the signal handler runs in */
    )
    {
    VMEX_SHADOW_TCB *pShadow;			/* TCB extension */

    if ( (pShadow=vmexTcb (0)) == NULL )     /* get the TCB extension */
	return(ERROR);

    /* install ASR in the TCB, with all signals cleared */

    pShadow->pASR = pASR;
    pShadow->ASMode = mode;
    pShadow->ASEvents = 0;

    /* attach the "ASR Wrapper" function to POSIX signal SIGUSR1 */

    if ( signal (SIGUSR1, (VOIDFUNCPTR) ASRWrapper) == SIG_ERR)
	return (ERROR);

    return (EOK);
    }

/*****************************************************************************
*
* as_send - asynchronous signal send routine
*
* This function sends an asynchronous signal to the specified task.
* the signal gets OR'ed with other pending signals.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK if successful
* ERR_OBJID if <tid> is invalid
* ERR_NOASR if target task has not installed an ASR
* ERR_OBJDEL if signal was not sent due to invalid <tid>
* ERROR if some other failure in sigLib.c
*/

UINT32 as_send
    (
    UINT32 tid,			/* the task ID of the task to signal */
    UINT32 signal			/* the signal to be sent */
    )
    {
    int 		intKey;		/* key to unlock interrrupts */
    VMEX_SHADOW_TCB *	pShadow;	/* vmex TCB extension of task */

    /* get the task's TCB extension, and verify it is a vmex task */

    if ((pShadow = vmexTcb (tid)) == NULL) 
	return (ERR_OBJID);

    if (pShadow->pASR == NULL)   /* make sure target task has called as_catch */
	return (ERR_NOASR);

    if (signal == 0)
	return (EOK);    /* null signals don't trigger handler */

    intKey = intLock();   		 /* LOCK INTERRUPTS */

    pShadow->ASEvents |= signal;   /* OR the new signal into the mix */

    intUnlock(intKey); 			/* UNLOCK INTERRUPTS */ 

    /* 
     * Signal the target task.
     */

    if ( kill (tid, SIGUSR1) == ERROR )
        {
        if ( errno == EINVAL )
	    return (ERR_OBJDEL);
        else
	    return (ERROR);
        }

    return (EOK);
    }
