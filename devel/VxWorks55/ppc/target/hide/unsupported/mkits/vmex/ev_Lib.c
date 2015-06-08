/* ev_Lib.c -  event routines */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,04feb97,jmb  add initialization of timeLast
01d,29jan97,jmb  fix waitForever condition and skip tickGet() calls
		 where possible.
01c,13jun96,jmb  comment clean-up
01b,12apr96,jmb  converted args to unsigned
01a,20mar96,cym  created
*/

#include "vxWorks.h"
#include "vmexLib.h"
#include "tickLib.h"
#include "intLib.h"
#include "semLib.h"
#include "objLib.h"

/*****************************************************************************
*
* ev_asend -  asynchronous event send 
*
* This is a pSOS+m(r) function, and is currently unsupported on vxWorks. 
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* ERR_SSFN  
*/
UINT32 ev_asend
    (
    UINT32 tid,			/* ID of task to send to */
    UINT32 event			/* event to send */
    )
    {
    return (ERR_SSFN);
    }

/*****************************************************************************
*
* ev_receive -  event reception routine.
*
* This function waits on a block of (32) event bits in the TCB extension, for 
* any/all of a given set of events to be sent to the task through the 
* ev_send routine.  It can be used as a "select" for sempahores.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK if succesful
* ERR_TIMEOUT if requested events aren't received before <timeout> expires.
* ERR_NOEVS if requested events aren't received when the no-wait flag is set.
* ERROR if calling task not a vmex task or if some error (other than
* a timeout) occurred in semTake.
*/
UINT32 ev_receive
    (
    UINT32 eventin,		/* events to wait on */
					/* (0 means just get pending events */
    UINT32 flags,			/* option flags(wait/nowait, any/all)*/
    UINT32 timeout,		/* how long to wait for the events */
    UINT32 *eventout		/* which events were received */
    )
    {
    int 	timeLast = 0;		/* time value, to compute timeout */
    int 	mask = eventin;		/* which events to look for */
    int 	eventsCur;		/* */
    int 	eventMatch;		/* valid events matched on this loop */
    int 	intlvl;			/* key to unlock interrupts */
    int 	rvalue = EOK;
    BOOL 	done = FALSE;		/* have I received the needed events? */
    VMEX_SHADOW_TCB *	pShadow;		/* tcb extension(where events are kept*/
    int 	timeoutPrev;
    BOOL 	waitForever;

    /* get the tcb extension and make sure this is a vmex task */

    if( (pShadow = vmexTcb (0)) == NULL ) 
	return (ERROR);

    /* for eventin=0, return all pending events */

    if (mask == 0)
	{
	*eventout = pShadow->events;
	return (EOK);
	}

    /* Convert to vxWorks timeouts */

    if (timeout == 0) 
	timeout = WAIT_FOREVER;

    if (flags & EV_NOWAIT) 
	timeout = NO_WAIT;

    /* Need this variable because when timeout is used as a counter
    *  and it is decremented beyond 0, it could result in a -1, which is
    *  a vxWorks WAIT_FOREVER 
    */

    waitForever = (timeout == WAIT_FOREVER); 

    if ((timeout != WAIT_FOREVER) && (timeout != NO_WAIT))
        timeLast = tickGet();    		/* initialize timekeeper */

    /* keep going until the proper events are received, or we've timed out */
    do
        {
	/* get the currently pending events */

	eventsCur = pShadow->events;

	/* see which ones we are looking for */

	eventMatch = eventsCur & mask;

        /* see if we've matched any */

        if (eventMatch)
	    {
            /*
	    * we need to unpend the events to acknowledge we've received them
	    * lock interrupts while messing with the extension
	    */
            intlvl = intLock();			/* LOCK INTERRUPTS */

	    /* clear any events we've matched */

	    pShadow->events &= ~eventMatch;

	    intUnlock (intlvl);			/* UNLOCK INTERRUPTS */

	    /* stop looking for events we've received */

            mask = mask & ~eventMatch;

	    /* if the ANY flag is set, we're done */

	    if(flags & EV_ANY)
		done = TRUE;
            else
		{
		/* otherwise, keep looking if we haven't gotten them all */
		if (mask == 0)
		    done = TRUE;
                }
            }

	/*  update the timeout value if needed */

        if (!waitForever && (timeout != NO_WAIT))
	    {
	    timeoutPrev = timeout;
	    timeout = timeout + timeLast - (timeLast = tickGet());
	    if ( timeout==0 || ( (int) timeout < 0 && timeoutPrev > 0 ) )
		timeout = NO_WAIT;
	    }

	/* if we're not done, and there time on the clock */

        if(!done && (timeout != NO_WAIT))
	    {
	    /*
	    * wait for someone to signal a new event.
	    * (ev_send gives this semaphore)
	    */
	    if (semTake (pShadow->eventWait, timeout) == ERROR)
		{
		/* if the wait times out, give up */
		if (errno == S_objLib_OBJ_TIMEOUT)
		    rvalue = ERR_TIMEOUT;
                else
		    rvalue = ERROR;
		timeout = NO_WAIT;
		}
            }
    /* keep going until were done, or timed out */
    } while( !done && (timeout != NO_WAIT));

    /* in VMEexec, a failed NO_WAIT ev_receive returns NOEVS instead of TIMEOUT */
    if(flags & EV_NOWAIT) 
	if(!done)
	    rvalue = ERR_NOEVS;

    /* return all events which have been cleared from the mask(received) */

    *eventout = eventin & ~mask;

    return (rvalue);
    }

/*****************************************************************************
*
* ev_send -  event send routine 
*
* This routine sets some bits in the event word of a task's tcb extension.
* It also gives a semaphore to signal the task if it waiting to receive events.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK if successful
* ERR_OBJID if <tid> is invalid
*/
UINT32 ev_send
    (
    UINT32 tid,			/* ID of task to send events to */
    UINT32 event			/* events to send to the task */
    )
    {
    int 		intlvl;		/* key to unlock interrupts */
    VMEX_SHADOW_TCB *	pShadow;

    /* make sure the task we're sending to is a vmex task */

    if ((pShadow = vmexTcb (tid)) == NULL) 
	return (ERR_OBJID);

    /* lock interrupts while modofying the TCB */
    intlvl = intLock();			/* LOCK INTERRUPTS */

    /* OR in the events */

    pShadow->events |= event;
    intUnlock (intlvl);			/* UNLOCK INTERRUPTS */

    /* and signal the task that an event has been logged */

    semGive (pShadow->eventWait);

    return (EOK);
    }

