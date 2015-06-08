/* tm_Lib.c -  timer routines */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,04feb97,jmb  Add code to support timer array.
01g,12jul96,jmb  Bug in month translation
01f,08jul96,jmb  Correct time structure for VMEexec
01e,24jun96,jmb  comment clean up
01d,16may96,jmb  bugfixes for VMEEXEC
01c,05apr96,jmb  fixed tm_delete and changed intLocks to taskLocks
01b,04apr96,jmb  added tm_ functions that use calendar time and
		 added comments
01a,26mar96,cym  created
*/

#include "vxWorks.h"
#include "taskLib.h"     
#include "vmexLib.h"
#include "excLib.h"
#include "sysLib.h"     
#include "time.h"     
#include "memLib.h"     
#include "wdLib.h"     
#include "tickLib.h"     

/* externals */
#ifdef VMEX_USE_TIMER_ARRAY
IMPORT int vmexMaxTimers;
IMPORT VMEX_TIMER_NODE vmexTimerArray[];
#endif

/* locals */

BOOL vmexTimeReady = FALSE;

/* forward static functions */

LOCAL int timeToDelay (struct time_ds *, UINT32 *);
LOCAL int timeRangeCheck (struct time_ds *);
LOCAL int timeToTm (struct time_ds *, struct tm *);
#ifndef VMEEXEC
LOCAL int timeToVMETime (UINT32, UINT32, UINT32, 
    struct time_ds *);
LOCAL int VMETimeToTime (struct time_ds *, UINT32 *, UINT32 *,
    UINT32 *);
#endif
LOCAL void timerWrapper (UINT32);
LOCAL void timerTaskWrapper (UINT32);
#ifdef VMEX_USE_TIMER_ARRAY
LOCAL UINT32 getTimerArrayElement (VMEX_SHADOW_TCB *,VMEX_TIMER_NODE * *, UINT32 *);
#else
LOCAL UINT32 makeTimerNode (VMEX_SHADOW_TCB *,VMEX_TIMER_NODE * *, UINT32 *);
#endif
LOCAL UINT32 vmexTimerArm (UINT32, UINT32, UINT32 *, UINT32);



/*****************************************************************************
*
* tm_tick - clock tick announce function
*
* this function causes a system clock tick, it is usually called by the 
* system clock's ISR.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK 
* 
*/
UINT32 tm_tick()
    {
    tickAnnounce ();
    return (EOK);
    }

/**********************************************************************
*
* tm_wkafter - wake up calling task after a given number of ticks
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK 
*
*/
UINT32 tm_wkafter
    (
    UINT32 ticks    /* delay in ticks */
    )
    {
    (void) taskDelay (ticks);
    return (EOK);
    }


/**********************************************************************
*
* timerTaskWrapper - send event to a task, then either rearm or delete
*                    the expired timer.
*
* This routine is run at task level by timerWrapper.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* <void>
*
*/
LOCAL void timerTaskWrapper
    (
    UINT32 timer    		/* timer id  */
    )
    {
    VMEX_TIMER_NODE * pTimer;
    VMEX_SHADOW_TCB * pShadow;
    UINT32		tid;
    
    pTimer = (VMEX_TIMER_NODE *) timer;
    tid = pTimer->tid;

    /* ev_send will fail if target task is deleted...since target task
     * owns the timer, it will be cleaned up when that task dies.
     */
    if ( ev_send (tid, pTimer->event) != EOK )  
	return;                 

    taskLock();			/* TASK LOCK */

    if (pTimer->tid == tid)
	{
        if (pTimer->delay != 0)
	    {          
	    /*  rearm the timer */

            (void) wdStart (pTimer->timer, pTimer->delay, 
	        (FUNCPTR) timerWrapper, (int) pTimer);
            taskUnlock();
	    }
        else
	    {
	    /*
	     *  Remove the timer node from the linked list in the TCB extension,
	     *  delete the watchdog, and free the node.
	     *  Even though we're running in the exception task, it's
	     *  safe to taskLock because someone may decide (probably unwisely)
	     *  to run an application at priority 0.
	     */

	    pTimer->tid = NULL;		/* free timer node */
#ifdef VMEX_USE_TIMER_ARRAY
	    pShadow = vmexTcb (pTimer->tid);
	    pShadow->timerCount -= 1;
#else
	    if (pTimer->next != NULL) 
	        pTimer->next->previous = pTimer->previous;

	    if (pTimer->previous != NULL) 
	        pTimer->previous->next = pTimer->next;
            else
	        {
	        pShadow = vmexTcb (pTimer->tid);
	        pShadow->pTimerList = pTimer->next;
	        }
#endif
            taskUnlock();			/* TASK UNLOCK */

	    (void) wdDelete (pTimer->timer);
#ifndef VMEX_USE_TIMER_ARRAY
	    (void) free (pTimer);
#endif
            }
	}
    }

/**********************************************************************
*
* timerWrapper - queue timerTaskWrapper for task level execution.
*
* This routine executes upon expiration of a timer armed by tm_evafter
* or tm_evevery.  It is called by a watchdog timer and runs at interrupt
* level.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* <void>
*
*/
LOCAL void timerWrapper
    (
    UINT32 timer    /* timer id (really a pointer to timer node ) */
    )
    {
    /* excJobAdd queues a function to run at task level, priority 0 */

    (void) excJobAdd (timerTaskWrapper, timer, 0, 0, 0, 0, 0);
    }

#ifndef VMEX_USE_TIMER_ARRAY
/**********************************************************************
*
* makeTimerNode - make a timer node
*
* This routine allocates a timer node and adds it to the calling task's
* TCB.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* ERROR if malloc failure
* EOK otherwise
*
*/
LOCAL UINT32 makeTimerNode 
    (
    VMEX_SHADOW_TCB * pShadow,	/* pointer to TCB extension */
    VMEX_TIMER_NODE * *pNode,	/* pointer to timer node returned */
    UINT32 * rc			/* return code -- not used */
    )
    {
    VMEX_TIMER_NODE * pTimer;
    /*  Allocate a timer node */

    pTimer = (VMEX_TIMER_NODE *) malloc (sizeof (VMEX_TIMER_NODE));   

    if (pTimer == NULL)
	return (ERROR);

    pTimer->tid = taskIdSelf();
    pTimer->previous = NULL;

    /*  Add the new timer node to head of linked list in TCB extension */

    taskLock();                                   /* TASK LOCK */

    pTimer->next = (void *) pShadow->pTimerList;
    pShadow->pTimerList = pTimer;
    if(pTimer->next != NULL)
        pTimer->next->previous = pTimer;

    taskUnlock();                                 /* TASK UNLOCK */

    *pNode = pTimer;
    return (EOK);
    }
#endif

#ifdef VMEX_USE_TIMER_ARRAY
/**********************************************************************
*
* getTimerArrayElement - get a timer node
*
* This routine gets a timer node from an array of timer nodes.
* Free nodes in the array are those with a NULL task id field.
* When a node is gotten, the tid field is set to the calling task's
* id and the timerCount in the TCB is incremented.  The timerCount
* is used to inform t_delete and t_restart that it is necessary to
* scan the timer array and delete all timers owned by the dying task.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK if successful
* ERROR if no elements available in timer array.
*
*/
LOCAL UINT32 getTimerArrayElement
    (
    VMEX_SHADOW_TCB * pShadow,	/* pointer to TCB extension */
    VMEX_TIMER_NODE * *pNode,	/* pointer to timer array element returned */
    UINT32 * rc			/* error code returned */
    )
    {
    int i;

    /*  Allocate a timer node */

    taskLock();				/* TASK LOCK */

    /* Do the equivalent of malloc'ing a timer node:
     * search the timer array for a free node
     * and grab it if available.
     */

    for (i = 0; i < vmexMaxTimers; i++)
       if (vmexTimerArray[i].tid == NULL)
	  break;

    if (i < vmexMaxTimers)
	{
        pShadow->timerCount += 1;
        vmexTimerArray[i].tid = taskIdSelf();
	}

    taskUnlock();			/* TASK UNLOCK */

    if (i == vmexMaxTimers)  /* No more timers available */
	{
        *rc = ERR_NOTIMERS;
	return (ERROR);
	}
    
    *pNode = &vmexTimerArray[i];

    *rc = EOK;

    return (EOK);
    }
#endif

/***********************************************************************
*
* vmexTimerArm - This routine implements tm_evevery and tm_evafter.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK if successful
* ERROR if a failure if called by an unrecognized task, memory
*       allocation failed, or problems with wdLib.c 
*
*/
LOCAL UINT32 vmexTimerArm
    (
    UINT32 ticks,  /* clock ticks to wait before sending event */
    UINT32 event,  /* event mask */
    UINT32 *tmid,   /* timer id returned */
    UINT32 periodic /* If true, timer is peridic, else timer is one-shot  */
    )
    {
    VMEX_TIMER_NODE * pTimer;
    VMEX_SHADOW_TCB * pShadow;
    int rc;

    /*
     * Make sure calling task has a valid TCB extension.
     */

    if( (pShadow = vmexTcb (0)) == NULL ) 
	return (ERROR);

    /*  Allocate a timer node */

#ifdef VMEX_USE_TIMER_ARRAY
    if (getTimerArrayElement (pShadow, &pTimer, &rc) == ERROR )
       return (rc);
    *tmid = pTimer - vmexTimerArray;   	/* tmid is array index */
#else
    if (makeTimerNode (pShadow, &pTimer, &rc) == ERROR )
       return (ERROR);
    *tmid = (UINT32) pTimer;		/* tmid is pointer to node */
#endif

    if (periodic)
        pTimer->delay = ticks;
    else
        pTimer->delay = 0;

    pTimer->event = event;

    /*  Create and start a watchdog timer  */

    if ( (pTimer->timer = wdCreate ()) == NULL )   
       goto watchdogFailure;

    if ( wdStart (pTimer->timer, ticks, (FUNCPTR) timerWrapper, (int) pTimer) 
	== ERROR)
       goto watchdogFailure;

    return (EOK);

    /* Should not get here!  Following code is executed only if
     * watchdog timer cannot be created or started.
     */

watchdogFailure:

    /*
     * Free allocated timer node and remove its ptr from TCB
     */

    taskLock();			/* TASK LOCK */

#ifdef VMEX_USE_TIMER_ARRAY

    pShadow->timerCount -= 1;
    pTimer->tid = NULL;

#else

    if (pTimer->next != NULL) 
        pTimer->next->previous = pTimer->previous;

    if (pTimer->previous != NULL) 
        pTimer->previous->next = pTimer->next;
    else
        pShadow->pTimerList = pTimer->next;
#endif

    taskUnlock();			/* TASK UNLOCK */

#ifndef VMEX_USE_TIMER_ARRAY
    free (pTimer);
#endif
    return (ERROR);
    }

/***********************************************************************
*
* tm_evafter - send an event to calling task when a timer expires
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK if successful
* ERROR if a failure if called by an unrecognized task, memory
*       allocation failed, or problems with wdLib.c 
* ERR_NOTIMERS if using timer array and no timers available
*
*/
UINT32 tm_evafter
    (
    UINT32 ticks,  /* clock ticks to wait before sending event */
    UINT32 event,  /* event mask */
    UINT32 *tmid   /* timer id returned */
    )
    {
    return (vmexTimerArm (ticks, event, tmid, FALSE));
    }

/***********************************************************************
*
* tm_evevery - send an event to calling task at periodic intervals
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK if successful
* ERROR if a failure if called by an unrecognized task, memory
*       allocation failed, or if a problem in wdLib.c
* ERR_NOTIMERS if using timer array and no timers available
*
*/
UINT32 tm_evevery
    (
    UINT32 ticks,  /*  clock ticks to wait before sending event */
    UINT32 event,  /* event mask */
    UINT32 *tmid   /* timer id returned */
    )
    {
    return (vmexTimerArm (ticks, event, tmid, TRUE));
    }
 
/***********************************************************************
*
* tm_cancel - disarm and delete a timer
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK if successful
* ERR_BADTMID if successful
* ERROR if a failure if called by an unrecognized task, memory
*       allocation failed, or watchdog could not be created.
*
*/
UINT32 tm_cancel
    (
    UINT32 tmid   /* timer id (really a ptr to a timer node) */
    )
    {
    VMEX_TIMER_NODE * 	pTimer;
    VMEX_SHADOW_TCB * pShadow;

    if( (pShadow = vmexTcb (0)) == NULL ) 
	return (ERROR);
        
#ifdef VMEX_USE_TIMER_ARRAY
    if (tmid > vmexMaxTimers)
        return (ERR_BADTMID);
    pTimer = &vmexTimerArray[tmid];
#else
    pTimer = (VMEX_TIMER_NODE *) tmid;
#endif


    /* Only task that created timer can delete it */

    if (pTimer->tid != taskIdSelf())
        return (ERR_BADTMID);
        
    if (wdCancel (pTimer->timer) == ERROR)
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_BADTMID);
        else
	    return (ERROR);
	}

    taskLock();			/* TASK LOCK */

    pTimer->tid = NULL;

#ifdef VMEX_USE_TIMER_ARRAY

    pShadow->timerCount -= 1;

#else

    if (pTimer->next != NULL) 
        pTimer->next->previous = pTimer->previous;

    if (pTimer->previous != NULL) 
        pTimer->previous->next = pTimer->next;
    else
        pShadow->pTimerList = pTimer->next;
#endif

    taskUnlock();			/* TASK UNLOCK */

#ifndef VMEX_USE_TIMER_ARRAY
    free (pTimer);
#endif
    return (EOK);
    }

/**********************************************************************
*
* tm_set - set the calendar time
*
* This routine sets the calendar time and sets the flag
* indicating that the time has been set.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* 0 if successful
* ERR_ILLTIME if input argument contains an invalid time.
* ERR_ILLDATE if input argument contains an invalid date.
* ERR_ILLTICKS if input tick count exceeds 1 second of real time.
* ERROR if a failure in clockLib.c or ansiTime.c
*
*/

UINT32 tm_set
    (
#ifdef VMEEXEC
    struct time_ds * pTimebuf    /* new time */
#else
    UINT32 date,          /* new year, month, date */
    UINT32 time,          /* new hour, minute, second */
    UINT32 ticks          /* new clock tick counter value */
#endif
    )
    {
    struct tm 		newTime;
    struct timespec 	newPosixTime;
    struct timespec 	clockPosixRes;
    int 		status;
#ifndef VMEEXEC
    struct time_ds 	timeBuf;
    struct time_ds * 	pTimebuf = &timeBuf;

    timeToVMETime (date, time, ticks, pTimebuf);
#endif

    /*  Check input values */

    status = timeRangeCheck (pTimebuf);

    if ( status != EOK ) 
	return (status);

    /* Convert timebuf values to ANSI "tm" format */

    timeToTm (pTimebuf, &newTime);

    /* Get calendar time in seconds */
      
    newPosixTime.tv_sec = mktime (&newTime);  

    if ( newPosixTime.tv_sec == ERROR )
	 return (ERROR);

    /* Convert number of ticks to microseconds */

    if ( clock_getres (CLOCK_REALTIME, &clockPosixRes) == ERROR )
        return (ERROR);

    newPosixTime.tv_nsec = (pTimebuf->ticks * clockPosixRes.tv_nsec);

    /*  Now set the time */

    if ( clock_settime (CLOCK_REALTIME, &newPosixTime) == ERROR )
	return (ERROR);

    vmexTimeReady = TRUE;
    return (EOK);
    }

/**********************************************************************
*
* tm_get - get the calendar time
*
* This routine gets the calendar time.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* 0 if successful
* ERR_NOTIME if time has not been set by an earlier tm_set call.
* ERROR if a failure in clockLib.c
*
*/

UINT32 tm_get
    (
#ifdef VMEEXEC
    struct time_ds * pTimebuf
#else
    UINT32 * date,
    UINT32 * time,
    UINT32 * ticks
#endif
    )
    {
    struct timespec 	currentPosixTime;
    struct timespec 	clockPosixRes;
    struct tm 		tmTime;
#ifndef VMEEXEC
    struct time_ds 	timeBuf;
    struct time_ds * 	pTimebuf = &timeBuf;
#endif

    /* Make sure time has been initialized since reboot. */
     
    if (vmexTimeReady == FALSE )
       return (ERR_NOTIME);

    /* Get current time in seconds. */

    if ( clock_gettime (CLOCK_REALTIME, &currentPosixTime) == ERROR)
	return (ERROR);

    /* Convert to broken-down (tm) form. */
       
    localtime_r ((time_t *) &currentPosixTime.tv_sec, &tmTime);
      
    /*  Translate to output format.  */
       
    pTimebuf->time.second = tmTime.tm_sec;
    pTimebuf->time.minute = tmTime.tm_min;
    pTimebuf->time.hour = tmTime.tm_hour;
    pTimebuf->date.day = tmTime.tm_mday;
    pTimebuf->date.month = tmTime.tm_mon + 1;
    pTimebuf->date.year = tmTime.tm_year + 1900;

    /* If there's a problem getting the clock resolution, just
     * return the number of ticks as 0 rather than raising an error.
     * Assume users don't really care about wall-clock time
     * at the tick level. 
     */

    if ( clock_getres (CLOCK_REALTIME, &clockPosixRes) != ERROR )
        pTimebuf->ticks = currentPosixTime.tv_nsec / clockPosixRes.tv_nsec;
    else
	pTimebuf->ticks = 0;

#ifndef VMEEXEC
    VMETimeToTime (pTimebuf, date, time, ticks);
#endif

    return (EOK);
    }

/***********************************************************************
*
* tm_wkwhen - block calling task until arrival of specific calendar time
*
* INCLUDE FILES:  vmexLib.h
*
* RETURNS:
* 0 if successful
* ERR_NOTIME if time/date never set by tm_set.
* ERR_ILLTIME if input argument contains an invalid time.
* ERR_ILLDATE if input argument contains an invalid date.
* ERR_ILLTICKS if input tick count exceeds 1 second of real time.
* ERR_TOOLATE if wakeup time is in the past.
* ERROR if a failure in clockLib.c or ansiTime.c
*
*/

UINT32 tm_wkwhen
    (
#ifdef VMEEXEC
    struct time_ds * timebuf   /* ptr to time info */
#else
    UINT32 date,         /* year, month, date */
    UINT32 time,         /* hour, minute, second */
    UINT32 ticks         /* clock ticks */
#endif
    )
    {
/*
*   The vxWorks implementation of waking up tasks at a specfic
*   time is to compute a delay and wake up the task after a
*   fixed number of ticks.  It is not sensitive to a clock reset.
*   Therefore, instead of calling vxWorks directly,
*   this routine will simply serve as a wrapper for tm_wkafter.
*/

    UINT32 tickDelay;
    int status;
#ifndef VMEEXEC
    struct time_ds timeBuf;
    struct time_ds * pTimebuf = &timeBuf;

    timeToVMETime (date, time, ticks, pTimebuf);
#else
    struct time_ds * pTimebuf = timebuf;
#endif

    status = timeToDelay (pTimebuf, &tickDelay);

    if ( status != EOK )
	return (status);

    /*  Call tm_wkafter to arm timer */

    return (tm_wkafter (tickDelay));
    }

/***********************************************************************
*
* tm_evwhen - send an event to a task when a specific calendar time arrives
*
* INCLUDE FILES: time.h 
*
* RETURNS:
* 0 if successful
* ERR_NOTIME if time/date never set by tm_set.
* ERR_ILLTIME if input argument contains an invalid time.
* ERR_ILLDATE if input argument contains an invalid date.
* ERR_ILLTICKS if input tick count exceeds 1 second of real time.
* ERR_TOOLATE if wakeup time is in the past.
* ERROR if a failure in clockLib.c or ansiTime.c
*
*/

UINT32 tm_evwhen
    (
#ifdef VMEEXEC
    struct time_ds * pTimebuf,         /* time info */
#else
    UINT32 date,          /* year, month, date */
    UINT32 time,          /* hour, minute, second */
    UINT32 ticks,         /* clock ticks */
#endif
    UINT32 event,         /* event bits to send */
    UINT32 *tmid          /* output timer id */
    )
    {
/*
*   The vxWorks implementation of waking up tasks at a specfic
*   time is to compute a delay and wake up the task after a
*   fixed number of ticks.  It is not sensitive to a clock reset.
*   Therefore, instead of calling vxWorks directly,
*   this routine will simply serve as a wrapper for tm_evwhen.
*/

    UINT32 	tickDelay;
    int 		status;
#ifndef VMEEXEC
    struct time_ds 	timeBuf;
    struct time_ds * 	pTimebuf = &timeBuf;

    timeToVMETime (date, time, ticks, pTimebuf );
#endif

    status = timeToDelay ( pTimebuf, &tickDelay);

    if ( status != EOK )
	return (status);

    return (vmexTimerArm (tickDelay, event, tmid, FALSE));
    }

/**********************************************************************
*
* timeToTm - convert a time_ds structure to an ANSI tm structure
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK
*
*/

LOCAL int timeToTm
    (
    struct time_ds * pTimebuf,   	/* input time info struct */
    struct tm * pOutputTime         	/* output time in ANSI tm format */
    )
    {
    pOutputTime->tm_sec = pTimebuf->time.second;
    pOutputTime->tm_min = pTimebuf->time.minute;
    pOutputTime->tm_hour = pTimebuf->time.hour;
    pOutputTime->tm_mday = pTimebuf->date.day;
    pOutputTime->tm_mon = pTimebuf->date.month - 1;
    pOutputTime->tm_year = pTimebuf->date.year - 1900;
    pOutputTime->tm_wday = 0;  		/* ignored */
    pOutputTime->tm_yday = 0;  		/* ignored */
    pOutputTime->tm_isdst = 1;
    return (EOK);
    }

/**********************************************************************
*
* timeRangeCheck - check a time_ds structure for valid entries
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK if successful
* ERR_ILLTIME if input argument contains an invalid time.
* ERR_ILLDATE if input argument contains an invalid date.
* ERR_ILLTICKS if input tick count exceeds 1 second of real time.
*
*/

LOCAL int timeRangeCheck
    (
    struct time_ds * pTimebuf
    )
    {
    if ( pTimebuf->time.hour > 23 )
       return (ERR_ILLTIME);

    if ( pTimebuf->time.minute > 59 )
       return (ERR_ILLTIME);

    if ( pTimebuf->time.second > 59 )
       return (ERR_ILLTIME);

    if ( pTimebuf->date.day > 31 )
       return (ERR_ILLDATE);

    if ( pTimebuf->date.month > 12 )
       return (ERR_ILLDATE);

    if ( pTimebuf->ticks > sysClkRateGet() - 1 )
       return (ERR_ILLTICKS);

    return (EOK);
    }

/**********************************************************************
*
* timeToDelay - compute number of ticks from current time to
*                   time specified in timebuf structure.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK if successful
* ERR_NOTIME if calendar time hasn't been set via tm_set.
* ERR_ILLTIME if input argument contains an invalid time.
* ERR_ILLDATE if input argument contains an invalid date.
* ERR_ILLTICKS if input tick count exceeds 1 second of real time.
* ERR_TOOLATE if current time is later than time in <timebuf>.
* ERROR if failure in clockLib.c or ansiTime.c.
*
*/

LOCAL int timeToDelay 
    ( 
    struct time_ds * pTimebuf,   /* input struct describing some future time */
    UINT32 * pTickDelay    /* output number of ticks from the present */
    )
    {
    struct timespec 	wakeupPosixTime;
    struct timespec 	currentPosixTime;
    struct timespec 	clockResPosix;
    struct tm 		timeTm;
    int 		sDelay; 
    int 		nanoDelay; 
    int 		status;

    if (vmexTimeReady == FALSE )
       return (ERR_NOTIME);

    /*  Check input values */

    status = timeRangeCheck (pTimebuf);

#ifdef VMEEXEC
    if (status != EOK && status != ERR_ILLTICKS) 
	return (status);
#else
    if ( status != EOK ) 
	return (status);
#endif

    /*  Convert time to seconds */

    timeToTm (pTimebuf, &timeTm);

    wakeupPosixTime.tv_sec = mktime (&timeTm);  

    if ( wakeupPosixTime.tv_sec == ERROR )
	 return (ERROR);

#ifdef VMEEXEC
    wakeupPosixTime.tv_nsec = 0;   /* ticks argument ignored */
#else
    if ( clock_getres (CLOCK_REALTIME, &clockResPosix) == 0 )
        wakeupPosixTime.tv_nsec = pTimebuf->ticks * clockResPosix.tv_nsec;
    else
	wakeupPosixTime.tv_nsec = 0;
#endif

    /* Get Current time */

    if ( clock_gettime (CLOCK_REALTIME, &currentPosixTime) == ERROR )
	return (ERROR);

    /* Make sure current time is less than wakeup time */

    if ( (int) wakeupPosixTime.tv_sec < (int) currentPosixTime.tv_sec )
	return (ERR_TOOLATE);

    if ( wakeupPosixTime.tv_sec == currentPosixTime.tv_sec )
        if ( (int) wakeupPosixTime.tv_nsec < (int) currentPosixTime.tv_nsec) 
	    return (ERR_TOOLATE);

    /* Compute the delay time in ticks */

    sDelay = sysClkRateGet() * 
	(wakeupPosixTime.tv_sec - currentPosixTime.tv_sec);

    if ( clockResPosix.tv_nsec == 0 ) 
	nanoDelay = 0;
    else
        nanoDelay = (wakeupPosixTime.tv_nsec - currentPosixTime.tv_nsec) /
	    clockResPosix.tv_nsec;

    *pTickDelay = sDelay + nanoDelay;

    /*  If absolute value of sDelay is less than the clock rate, then
     *  the nanoDelay can cause the total delay to go negative.
     *  The checking for valid delay is kind of messy because delay
     *  is an unsigned value, and 32-bit delays are valid.  
     */

    if ( sDelay < sysClkRateGet() && -sDelay << sysClkRateGet() )
        if ( (int) *pTickDelay <= 0 )
            return (ERR_TOOLATE);

    return (EOK);
    }

#ifndef VMEEXEC
/**********************************************************************
*
* timeToVMETime - convert time from ISI to Motorola format. 
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK
*
*/
LOCAL int timeToVMETime
    (
    UINT32 date,    /* input year, month, date */
    UINT32 time,    /* input hour, minute, second */
    UINT32 ticks,   /* input ticks */
    struct time_ds * pTimebuf   /* output time info struct */
    )
    {

    pTimebuf->date.year =  date >> 16;
    pTimebuf->date.month  = (date & 0x0000ff00) >> 8;
    pTimebuf->date.day  = (date & 0x000000ff);
    pTimebuf->time.hour =  time >> 16;
    pTimebuf->time.minute  = (time & 0x0000ff00) >> 8;
    pTimebuf->time.second  = (time & 0x000000ff);
    pTimebuf->ticks = ticks;
    return (EOK);
    }

/**********************************************************************
*
* VMETimeToTime - convert time from Motorola to ISI format. 
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS:
* EOK
*
*/
LOCAL int VMETimeToTime
    (
    struct time_ds * pTimebuf,  /* input structure with time info */
    UINT32 * pDate,       /* output year, month, date */
    UINT32 * pTime,       /* output hour, minute, second */
    UINT32 * pTicks       /* output ticks */
    )
    {
    *pDate = 0;
    *pDate |=  (pTimebuf->date.year << 16);
    *pDate |=  (pTimebuf->date.month << 8);
    *pDate |=   pTimebuf->date.day;
    *pTime = 0;
    *pTime |=  (pTimebuf->time.hour << 16);
    *pTime |=  (pTimebuf->time.minute << 8);
    *pTime |=   pTimebuf->time.second;
    *pTicks = pTimebuf->ticks;
    return (EOK);
    }
#endif
