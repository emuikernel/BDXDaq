/* t_Lib.c - task control routines */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,04feb97,jmb  Added support for a timer array.
                 More bugfixing in t_restart -- task was unable to
		 restart itself.  Also fixed bug in patch 01n -- must manipulate
		 signal field in TCB directly (not using sigprocmask), when
		 setting sigmask for another task.
01n,20jan97,jmb  Get rid of ASEnable and use sigprocmask.
		 Also, fix bug in t_restart ... task was losing TCB extension.
01m,10oct96,jmb  change macro for selecting TCB extension connection method
01l,09jul96,jmb  add currentMode to TCB extension
01k,14jun96,jmb  comment clean-up
01j,13jun96,jmb  moved vmexTaskSpawn to vmexShow.
01i,10may96,jmb  bugfix in task mode in t_start.  Also, fix t_start so that
		 a task can't be started twice.
01h,10may96,jmb  change vmexTaskSpawn to 4 args and added strict-mode global.
01g,29apr96,jmb  bug in t_suspend...need special case for tid=0, also
		 for t_resume.
01f,22apr96,jmb  moved TCB extension to a task variable, added delete hook
01e,10apr96,jmb  added VMEX_STRICT stuff and fixed modified t_restart to
                 avoid stack reinitialization
01d,10apr96,cym  patched t_restart to restore reserved TCB fields.
01c,10apr96,jmb  modified mode flag handling in t_start and t_restart
                 fixed signal send logic in t_mode
01b,08apr96,jmb  added restoration of initial mode and priority to t_restart
01a,20mar96,cym  created
*/

#include "vxWorks.h"
#include "taskLib.h"
#include "taskVarLib.h"
#include "taskHookLib.h"
#include "vmexLib.h"
#include "errnoLib.h"
#include "taskArchLib.h"
#include "sigLib.h"
#include "wdLib.h"
#include "semLib.h"
#include "private/kernelLibP.h"
#include "private/taskLibP.h"
#include "intLib.h"

/* externals */

IMPORT int vmexStackBump;
IMPORT BOOL vmexStrictMode;
#ifdef VMEX_USE_TIMER_ARRAY
IMPORT int vmexMaxTimers;
IMPORT VMEX_TIMER_NODE vmexTimerArray[];
#endif

/* static forward declarations */

LOCAL STATUS dummyTask(void);
LOCAL void vmexCleanup(WIND_TCB *);

#ifdef TASKVAR_TCB_EXTENSION
LOCAL VMEX_SHADOW_TCB * pTcbExtension;
#endif
LOCAL BOOL vmexDeleteHookInitialized = FALSE;

#ifdef VMEX_USE_TIMER_ARRAY
LOCAL BOOL vmexTimerArrayInitialized = FALSE;
LOCAL void initializeTimerArray();
#endif

LOCAL BOOL sigmaskInitialized = FALSE;
LOCAL sigset_t sigusr1Mask;
LOCAL sigset_t oldMask;

#define MIN_SUPERVISOR_STACK 	128
#define MAX_VMEX_PRIORITY    	255
#define MAX_INITIAL_VMEX_PRIORITY    239
#define PRIORITY_SHIFT    	255

/*****************************************************************************
*
* dummyTask - dummy entry point for t_create
*
* This is a dummy entry point for the suspended task created by t_create.
* It should never be executed.  When the task is started with t_start, this 
* entry point will be replaced.
*
* INCLUDE FILES: vxWorks.h
*
* RETURNS:  ERROR if this routine is executed
*/

LOCAL STATUS dummyTask()
    {
    return (ERROR);
    }

/*****************************************************************************
*
* vmexTcb - get the TCB extension for the given task.
*
* This function is analogous to taskTcb, but returns a pointer to the vmex 
* "shadow" of the task, i.e. the TCB extension.  The TCB extension is where
* where the notepad registers, events, ASR, and other entities reside.
*
* INCLUDE FILES: vmexLib.h 
*
* RETURNS: - a pointer to the TCB, or NULL if the tid is invalid or
*            if an extension has not been installed.
*/

VMEX_SHADOW_TCB *vmexTcb
    (
    UINT32 tid                    /*  vxWorks task id */
    )
    {
#ifndef TASKVAR_TCB_EXTENSION
    WIND_TCB *tcb; 			/* the task's vxWorks tcb */

    if ((tcb = taskTcb (tid)) == NULL)  /* get the TCB */
	return (NULL);

    if( tcb->reserved1 != VMEX_COOKIE)  /* extension installed? */
	return (NULL);

    return ((VMEX_SHADOW_TCB *) tcb->reserved2);
#else
    int rvalue;

    rvalue = taskVarGet (tid, (int *) &pTcbExtension);
    if (rvalue == ERROR)
	return ((VMEX_SHADOW_TCB *) NULL);
    else
	return ((VMEX_SHADOW_TCB *) rvalue);
#endif
    }

/*****************************************************************************
*
* t_create -  task creation routine
*
* Creates a suspended task in vxWorks.
*
* INCLUDE FILES: vxWorks.h vmexLib.h taskLib.h private/taskLibP.h
*
* RETURNS:
* EOK if successful.
* ERR_TINYSTK if <superstk> is too small (strict mode only).
* ERR_PRIOR if <priority> is out of range.
* ERR_NOSTK if not enough memory to allocated requested stack space.
* ERROR for a failure in taskCreat or vmexTCBInit.
*
*/

UINT32 t_create
    (
    NAME_T (name),			/* four-byte name of the task */
    UINT32 priority,		/* priority(0 is low, 255 is high) */
    UINT32 superstk,		/* supervisor stack size */
    UINT32 userstk,		/* user stack size */
    UINT32 flags,			/* option flags */
    UINT32 *tid			/* tcb returned by this function */
    )
    {
    int 	vxOptions = 0;		/* vxWorks task options */
    char 	strName[5];		/* name as a character string */

    /* convert the four byte character name to a proper string */

    *(int *) strName = NAME_TO_INT(name);
    strName[4] = EOS;

    /* VMEexec requires supervisor stack >= 128, but vxWorks doesn't
     * have any stacksize requirements...
     */
    if ( vmexStrictMode )
        if ( superstk < MIN_SUPERVISOR_STACK )   
            return(ERR_TINYSTK);

    /* check for stack and priority limits */

    if ((priority > MAX_VMEX_PRIORITY) || priority == 0 )   
        return (ERR_PRIOR);
#ifndef VMEEXEC
    if (vmexStrictMode)
        if ( priority > MAX_INITIAL_VMEX_PRIORITY )
            return (ERR_PRIOR);
#endif

    /* convert to vxWorks priority */

    priority = PRIORITY_SHIFT - priority;

    /* handle all creation flags */

    if (flags & T_FPU) 
	vxOptions = VX_FP_TASK;

    /* actually create the task */

    *tid = taskCreat (strName, (int) priority, vxOptions, 
	(int) (superstk + userstk + vmexStackBump), dummyTask,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    /* check for errrors from taskCreat */

    if (*tid == ERROR)
	{
	if (errno == S_memLib_NOT_ENOUGH_MEMORY) 
	    return (ERR_NOSTK);
        else
            return (ERROR);
	}

    if ( vmexTCBInit (*tid) == ERROR )    /* Add the TCB extension */
	return (ERROR);

    if (! sigmaskInitialized)
        {
        sigemptyset(&sigusr1Mask);
        sigaddset(&sigusr1Mask,SIGUSR1);
        }

    return (EOK);
    }

/*****************************************************************************
*
* t_start - task start routine 
*
* This routine starts a task at the given entry point with the given parameters
*
* INCLUDE FILES: vmexLib.h 
*
* RETURNS:
* EOK if successful.
* ERR_ACTIVE if the task is already started.
* ERR_OBJID if the task id is invalid.
* ERR_OBJDEL if the task is deleted before is can be activated
* ERROR if the task failed to start for some other reason.
*/

UINT32 t_start
    (
    UINT32 tid,			/* task identifier */
    UINT32 mode,		/* initial task mode */
    void (*entrypt)(),		/* entry point for the task */
    UINT32 args[4]		/* initial arguments of task */
    )
    {
    WIND_TCB *		pTcb;		/* the task's control block */
    VMEX_SHADOW_TCB 	*pShadow;	/* the TCB extension */ 
    UINT32 		vxArgs[10];     /* larger arg array for vxWorks */
    unsigned int 	i; 		/* index variable */

    /* check that the task has been created */

    if ((pTcb = taskTcb (tid)) == NULL )   
	return (ERR_OBJID);

    /* and is a valid vmex task */

    if ((pShadow = vmexTcb(tid)) == NULL ) 
	return (ERR_OBJID);

    /* Next two if blocks guard against restarting an already started task */
    
    if (taskIsSuspended(tid))
	taskLock();				/* TASKLOCK */
    else
	return (ERR_ACTIVE);

    if (pTcb->entry != dummyTask)
	{
	taskUnlock();				/* TASK UNLOCK */
	return (ERR_ACTIVE);
	}

    pTcb->entry = (FUNCPTR) entrypt;   		/* install new entry point */

    taskUnlock();				/* TASK UNLOCK */

    /* Handle initial mode ...
     * Since round robin is global in vxWorks, the user's input value
     * for the round robin bit is ignored if round robin is enabled.
     * If global round robin is not on and the user requests time slicing
     * they'll get normal preemption mode instead.  Tasks will not be 
     * spawned locked if global round robin is on.
     * The only condition in which a task will be spawned locked is
     * if global round robin is off and the user requested an
     * unpreemptible task with time-slicing turned off.
     */

    if (roundRobinOn)
	{
	mode |= T_TSLICE;		/* time-slicing on */
	mode &= ~T_NOPREEMPT;		/* task still pre-emptible */
	}
    else
	{
	if (!(mode & T_TSLICE) && (mode & T_NOPREEMPT))
	    pTcb->lockCnt = 1; 		/* task will be locked when spawned */
        else
	    {
	    mode &= ~T_NOPREEMPT;	/* task is pre-emptible */
	    mode &= ~T_TSLICE;		/* time-slicing off */
	    }
        }

    if(mode & T_NOASR)            /* ASRs and SIGUSR1 enabled by default */
        pTcb->pSignalInfo->sigt_blocked = sigusr1Mask;

    mode |= T_SUPV;               /* No user mode in vxWorks */

    pShadow->initialMode = mode;   /* Done with mode ! */
    pShadow->currentMode = mode;   /* Done with mode ! */

    if ( args == NULL )
	vxArgs[0] = vxArgs[1] = vxArgs[2] = vxArgs[3] = 0;
    else
	for ( i = 0; i < 4; i++ )
	    vxArgs[i] = args[i];

    vxArgs[4] = vxArgs[5] = vxArgs[6] = vxArgs[7] = vxArgs[8] = vxArgs[9] = 0;

    taskArgsSet (pTcb, pTcb->pStackBase,(int *) vxArgs); /* set the task args */

    if (taskActivate (tid) == ERROR)             /* ready to run */
	{
	if (errno == S_objLib_OBJ_ID_ERROR ) 
	    return (ERR_OBJDEL);
        return (ERROR);
	}

    return (EOK);
    }

/*****************************************************************************
*
* t_mode - task mode set routine
*
* This function enables/disables preemption (like vxWorks' taskLock/taskUnlock).
* It also enables/disables the asynchronous signal routine(ASR).
* It does not lock interrupts -- the interrupt level mask in the mode
* word is ignored.  To lock/unlock interrupts, use the vxWorks routines
* key=intLock() and intUnlock(key).
*
* INCLUDE FILES: vxWorks.h vmexLib.h taskLib.h sigLib.h errnoLib.h
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if the task id is invalid.
* ERROR if a failure in sigLib.c
*/

UINT32 t_mode
    (
    UINT32 mask,		/* specifies which mode bits to change */
    UINT32 newmode,	/* new task mode to go to */
    UINT32 *oldmode	/* return old mode here */
    )
    {
    WIND_TCB *	pTcb;			/* the task's control block */
    VMEX_SHADOW_TCB *	pShadow;			/* the TCB extension */ 

    /* get the TCB extension */

    if ((pShadow = vmexTcb(0)) == NULL) 
	return (ERR_OBJID);

    *oldmode = pShadow->currentMode;

    /* a mask of zero means just return the currrent mode, don't change it */

    if (mask == 0) 
	return (EOK);

    pTcb = taskIdCurrent;

    if(mask & T_NOPREEMPT) 
	{
	if(newmode & T_NOPREEMPT)
	    {
	    /* to prevent preemption, do a taskLock() */
	    if (pTcb->lockCnt == 0)
	        taskLock ();			/* TASKLOCK */
            pShadow->currentMode |= T_NOPREEMPT;
	    }
        else 
	    {
	    /* to resume preemption, taskUnlock until the lockCnt is 0 */
	    if (pTcb->lockCnt > 0)
		{
		pTcb->lockCnt = 1;
		taskUnlock ();			/* TASK UNLOCK */
		}
            pShadow->currentMode &= ~T_NOPREEMPT;
	    }
        }

    if (mask & T_NOASR)       /* see if we're twiddling ASR's */
	if (newmode & T_NOASR)
	    {
            /*
	     * If old mode was ASR-enabled and we're
	     * not in an ASR, block SIGUSR1.
	     */
	    pShadow->currentMode |= T_NOASR;
	    if (! (pShadow->asrBusy || (*oldmode & T_NOASR)))
	        sigprocmask(SIG_BLOCK,&sigusr1Mask,&oldMask);
	    }
        else
	    {
            /*
             * If old mode was ASR-disabled and we're
	     * not in an ASR, unblock SIGUSR1.
	     */
	    pShadow->currentMode &= ~T_NOASR;
	    if ((! pShadow->asrBusy) && (*oldmode & T_NOASR))
	        sigprocmask(SIG_UNBLOCK,&sigusr1Mask,&oldMask);
	    }

    return (EOK); 
    }

/*****************************************************************************
*
* t_restart -  task restart routine.
*
* Restart a task with its original priority and mode.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if the task id is invalid.
* ERR_OBJDEL if the task is deleted by another task while in t_restart.
* ERROR if an unknown failure in taskRestart.
*/

UINT32 t_restart
    (
    UINT32 tid,			/* task ID of task to restart */
    UINT32 args[4]		/* new arguments for task */
    )
    {
    VMEX_TIMER_NODE 	*pTimer;	/* used to delete the tasks timers */
    WIND_TCB 		*pTcb;		/* the task's control block */
    VMEX_SHADOW_TCB 	*pShadow;			/* the TCB extension */ 
    UINT32 		tmp1;
#ifndef TASKVAR_TCB_EXTENSION
    UINT32 		tmp2;
#endif
    UINT32 		i;
    UINT32 		vxArgs[10];     /* larger arg array for vxWorks */
    UINT32 		lock;
    struct sigtcb 	*pSigTcb;

    /* get the task's TCB, and check for validity */

    if  (tid == 0)
	tid = taskIdSelf();

    if( (pTcb = taskTcb(tid)) == NULL) 
   	return (ERR_OBJID);

    /* get the TCB extension and make sure it's a vmex task */

    if( (pShadow = vmexTcb(tid)) == NULL ) 
	return (ERR_OBJID);

    if (tid == taskIdSelf())
	{
	/* self restart not possible because we have we have no context
	 * from which to sucessfully terminate or reinitialize ourself.
	 * We defer all self restarts to the exception task.
	 * To self restart within a critical section, we must TASK_UNSAFE ()
	 * until the safeCnt == 0.  Otherwise, the excTask would block
	 * forever trying to terminate us.
	 */

	while (pTcb->safeCnt > 0)	/* make task unsafe */
	    TASK_UNSAFE ();

#if	CPU==SIMSPARCSUNOS || CPU==SIMSPARCSOLARIS
	bzero ((char *) &pTcb->excInfo, sizeof (EXC_INFO));
#endif	/* CPU==SIMSPARCSUNOS || CPU==SIMSPARCSOLARIS */

	taskSpawn ("t_restart", 0, 0,
		   6000, t_restart, (int) tid,
		   0, 0, 0, 0, 0, 0, 0, 0, 0);
	FOREVER
	    taskSuspend (0);			/* wait for restart */
	}

    /* The following loop allows the target task to run until
     * it gives up all critical resources and can be suspended
     * without causing an interlock.  The criteria for suspending
     * the task here are the same as deleting a task -- both the
     * safeCnt and the lockCnt must be 0.  Note:  this loop does
     * things which should never appear in user code, namely entering
     * kernel mode and directly manipulating task queues.
     */

    lock = intLock();
    while ((pTcb->safeCnt > 0) ||
	   ((pTcb->status == WIND_READY) && (pTcb->lockCnt > 0)))
	{
	kernelState = TRUE;				/* KERNEL ENTER */

	intUnlock (lock);				/* UNLOCK INTERRUPTS */

	if (pTcb == taskIdCurrent)
	    {

	    pTcb->safeCnt = 0;				/* unprotect */
	    pTcb->lockCnt = 0;				/* unlock */

	    if (Q_FIRST (&pTcb->safetyQHead) != NULL)	/* flush safe queue */
                windPendQFlush (&pTcb->safetyQHead);

	    windExit ();				/* KERNEL EXIT */
	    }
	else						/* wait to destroy */
	    {
	    if (windPendQPut (&pTcb->safetyQHead, WAIT_FOREVER) != OK)
		{
		windExit ();				/* KERNEL EXIT */
		return (ERROR);
		}

	    switch (windExit())
		{

		case ERROR :				/* timed out */
		    return (ERROR);

		default :				/* we were flushed */
		    break;
		}
	     }

        /* Make sure target task didn't exit */

	lock = intLock ();				/* LOCK INTERRTUPTS */

	if (TASK_ID_VERIFY (pTcb) != OK)
	    {
	    intUnlock (lock);				/* UNLOCK INTERRUPTS */
	    return (ERR_OBJDEL);
	    }
	}

    /* Suspend the task being restarted (this already happened above 
     * if the task is restarting itself.
     */

    taskSuspend (tid);

    TASK_SAFE ();                                       /* protect deleter */

    /* At this point target task is suspended and can't be deleted
     * by anyone else.  If task is restarting itself, it is safe from
     * deletion.
     */

#ifndef TASKVAR_TCB_EXTENSION
    /* Save the pointers to the TCB extension, since the VxWorks TCB will
     * be wiped clean by taskRestart.
     */

    tmp1 = pTcb->reserved1;
    tmp2 = pTcb->reserved2;

    /* Set reserved field to NULL because delete hook runs when a
     * task is restarted -- and we don't want that!
     */
    pTcb->reserved1 = NULL;
    pTcb->reserved2 = NULL;
#else
    tmp1 = (UINT32) pTcb->pTaskVar;
    pTcb->pTaskVar = NULL;
#endif

    /* Reuse signal info structure in the task stack */

    pSigTcb = pTcb->pSignalInfo;

    intUnlock(lock);  				/* INT UNLOCK */

    /* Disarm all the task's timers.
     * Need to taskLock because we don't want timers expiring and
     * trying to update linked list while we are.
     */
    
    taskLock ();				/* TASKLOCK */
#ifdef VMEX_USE_TIMER_ARRAY
    {
    int i;
    if (pShadow->timerCount > 0)
       for (i=0; i < vmexMaxTimers; i++)
	   if (vmexTimerArray[i].tid == tid)
	       {
	       wdDelete (vmexTimerArray[i].timer);
	       vmexTimerArray[i].tid = NULL;
	       }
    pShadow->timerCount = 0;
    }
#else
    while (pShadow->pTimerList != NULL)
        {
	pTimer = pShadow->pTimerList;
	wdDelete (pTimer->timer);
	pShadow->pTimerList = pTimer->next;
	pTimer->tid = NULL;
	free (pTimer);
	}
    pShadow->pTimerList = NULL;
#endif
    taskUnlock ();				/* TASK UNLOCK */

    /* set the task's new arguments */

    if ( args == NULL )
	vxArgs[0] = vxArgs[1] = vxArgs[2] = vxArgs[3] = 0;
    else
        for ( i = 0; i < 4; i++ )
	    vxArgs[i] = args[i];

    taskArgsSet (pTcb, pTcb->pStackBase, (int *) vxArgs);

    /* Restore original priority. */

    taskPrioritySet (tid, pShadow->initialPriority);

    /* Restore original mode. */

    pShadow->currentMode = pShadow->initialMode;

    /*
     *  Note:  vxWorks' taskRestart keeps the current stack, rather than
     *  reducing the stacksize to the original. 
     *  To keep the outermost stack frame intact, must modify
     *  the vxWorks option word -- otherwise the stack would be
     *  reinitialized to 0xee's
     */

    pTcb->options |= VX_NO_STACK_FILL;

    /* don't let restarted task run till its TCB is fixed */

    taskLock ();   				/* TASK LOCK */

    if (taskRestart (tid) == ERROR)
	{
	taskUnlock ();				/* TASK UNLOCK */
	if (errno == S_objLib_OBJ_DELETED) 
	    return (ERR_OBJDEL);
	else
	    return (ERROR);
	}

#ifndef TASKVAR_TCB_EXTENSION
    /* Reconnect TCB extension */

    pTcb->reserved1 = tmp1;
    pTcb->reserved2 = tmp2;
#else
    pTcb->pTaskVar = (struct taskVar *) tmp1;
#endif

    /* reinitialize signal info */

    bzero ((void *) pSigTcb, sizeof (struct sigtcb));
    for (i = 0; i <= _NSIGS; i++)
        pSigTcb->sigt_qhead[i].sigq_next = pSigTcb->sigt_qhead[i].sigq_prev             = &pSigTcb->sigt_qhead[i];

    if (pShadow->currentMode & T_NOASR)
        pSigTcb->sigt_blocked = sigusr1Mask;

    pTcb->pSignalInfo = pSigTcb;
    /*
     * restarting a task clears all pending signals and events and resets 
     * the signal handler.
     */

    pShadow->events = 0;
    pShadow->ASEvents = 0;
    pShadow->asrBusy = FALSE;
    pShadow->pASR = NULL;

    /* Lock the new task if initial mode is locked */

    if(pShadow->currentMode & T_NOPREEMPT) 
	{
        if ((tid == 0) || (tid == taskIdSelf()))
           pTcb->lockCnt = 2;
	else
           pTcb->lockCnt = 1;
        }
    else
	pTcb->lockCnt = 0;

    taskUnlock ();   				/* TASK UNLOCK */
    taskUnsafe ();   				/* TASK UNSAFE */

    return (EOK);
    }

/*****************************************************************************
*
* t_ident -  task identification routine.
*
* Look up the task ID of the named task.
*
* INCLUDE FILES: vmexLib.h 
*
* RETURNS:
* EOK if successful.
* ERR_OBJNF if name not found.
* ERROR if an unknown failure in taskNameToId.
*/

UINT32 t_ident
    (
    NAME_T (name),			/* task's four-byte name */
    UINT32 node,			/* resident node of task (for MP) */
    UINT32 *tid			/* tid returned by this function */
    )
    {
    char strName[5];			/* task name in string form */

    /* convert the four-byte name into a proper string */
      
    *(int *) strName = NAME_TO_INT(name);
    strName[4] = EOS;

    /* a name of 0 means the calling task. */

    if (name == 0)
	*tid = taskIdSelf ();
    else
	{
        *tid = taskNameToId (strName);
	if (*tid == ERROR)
	    {
	    if (errno == S_taskLib_NAME_NOT_FOUND)
		return (ERR_OBJNF);
            else
	        return (ERROR);
	    }
	}

    return (EOK);
    }

/*****************************************************************************
*
* t_suspend -  task suspension routine.
*
* Suspends the specified task.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if task id invalid (not a vmex task)
* ERR_SUSP if task already suspended (strict mode only)
* ERR_OBJDEL if task id invalid in taskDelete
* ERROR if some other failure in taskDelete
*/

UINT32 t_suspend
    (
    UINT32 tid			/* task ID of task to suspend */
    )
    {
    /* vmex and vxWorks task Id is the same */
    if ( tid != 0 )
	{
        if (TASK_ID_VERIFY (tid) != OK ) 
	    return (ERR_OBJID);

    /* Suspending an already suspended task is an error in VMEexec,
     * but not in vxWorks
     */
         if (vmexStrictMode)
            if (taskIsSuspended (tid)) 
	        return (ERR_SUSP);
        }

    /* suspend it... */

    if (taskSuspend(tid) == ERROR)
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJDEL);
        else
	    return (ERROR);
	}

    return (EOK);
    }

/*****************************************************************************
*
* t_delete -  task deletion routine.
*
* This routine deletes the specified task.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if task id invalid (not a vmex task)
* ERR_OBJDEL if task id invalid in taskDelete
* ERROR if some other failure in taskDelete
*/

UINT32 t_delete
    (
    UINT32 tid				/* task ID of task to delete */
    )
    {
    /* verify the task Id as a vmex task */

    if (vmexTcb(tid) == NULL) 
	return (ERR_OBJID);

    if (taskDelete (tid) == ERROR)	/* delete the task */
	{
	if ( errno== S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJDEL);
        else
	    return (ERROR);
	}

    return (EOK);
    }

/*****************************************************************************
*
* t_resume - task resume routine.
*
* This routine causes a suspended task to resume execution.
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if task id invalid (not a vmex task)
* ERR_NOTSUSP if target task is not suspended (strict mode only)
* ERR_OBJDEL if task id invalid in taskDelete
* ERROR if some other failure in taskDelete
*/

UINT32 t_resume
    (
    UINT32 tid			/* task ID of task to resume */
    )
    {
    /* Verify that tid is a valid vxWorks task id */

    if ( tid != 0 )
        if (TASK_ID_VERIFY (tid) != OK) 
	    return (ERR_OBJID);

    /* resuming a non-suspended task is an error in VMEexec */

    if (vmexStrictMode)
        if( ! taskIsSuspended (tid)) 
	    return (ERR_NOTSUSP);

    if (taskResume (tid) == ERROR)
	{
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJDEL);
        else
	    return (ERROR);
	}

    return (EOK); 
    }

/*****************************************************************************
*
* t_setpri - priority get/set routine.  
*
* This routine sets the priority of the specified task, and returns the old 
* priority through an output parameter.  
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if task id invalid (not a vmex task)
* ERR_PRIOR if new priority is out of range 
* ERR_OBJDEL if task id invalid in taskPrioritySet
* ERROR if some other failure in taskLib.c
*/

UINT32 t_setpri
    (
    UINT32 tid,			/* id of target task */
    UINT32 newprio,		/* new priority to go to */ 
    UINT32 *oldprio		/* task's old priority */
    )
    {

    /* get the task's old priority */

    if ( taskPriorityGet((int) tid, (int *) oldprio) == ERROR )
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJID);
        else
	    return (ERROR);

    /* Change the old priority to VMEexec convention */

    *oldprio = PRIORITY_SHIFT - *oldprio;

    /* New priority of zero means just get the old priority */

    if (newprio == 0) 
	return (EOK);

    /* change the new priority to vxWorks convention */

    newprio = PRIORITY_SHIFT - newprio;

    /* and set the task's priority */

    if (taskPrioritySet ((int) tid, (int) newprio) == ERROR)
	{
	if (errno == S_taskLib_ILLEGAL_PRIORITY) 
	    return (ERR_PRIOR);
	if (errno == S_objLib_OBJ_ID_ERROR) 
	    return (ERR_OBJDEL);
	return (ERROR);
	}

    return (EOK);
    }

/*****************************************************************************
*
* t_setreg -  routine to set a notepad register.
*
* This routine sets a notepad "register" in the task's tcb extension.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if task id invalid (not a vmex task)
* ERR_REGNUM if register number is out of range.
*/

UINT32 t_setreg
    (
    UINT32 tid,			/* task ID of target task */
    UINT32 regnum,		/* number of register to set */
    UINT32 regvalue		/* value to set register to */
    )
    {
    VMEX_SHADOW_TCB *pShadow;			/* the tcb extension */ 

    /* Check that the register number is in range (0-15) */ 

    if (regnum > 15 ) 
	return (ERR_REGNUM);

    /* verify the task as a vmex task */

    if ((pShadow = vmexTcb (tid)) == NULL ) 
        return (ERR_OBJID);

    /* set the specified notepad register in the tcb extension */

    pShadow->notepads[regnum] = regvalue;

    return (EOK);
    }

/*****************************************************************************
*
* t_getreg - routine to get a notepad register.
*
* this routine gets a notepad register from the task's tcb extension.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS: 
* EOK if successful.
* ERR_OBJID if task id invalid (not a vmex task)
* ERR_REGNUM if register number is out of range.
*/

UINT32 t_getreg
    (
    UINT32 tid,			/* task ID to get the register from */
    UINT32 regnum,		/* number of register to get */
    UINT32 *regvalue		/* value of register returned here */
    )
    {
    VMEX_SHADOW_TCB *pShadow;			/* the tcb extension */ 

    /* check that the register number is in range (0-15) */

    if (regnum > 15) 
	return (ERR_REGNUM);

    /* verify the task as a vmex task */

    if( (pShadow = vmexTcb (tid)) == NULL) 
	return (ERR_OBJID);

    /* get the specified notepad value */

    *regvalue = pShadow->notepads[regnum];

    return(EOK);
    }

#ifdef VMEX_USE_TIMER_ARRAY
/*****************************************************************************
*
* initializeTimerArray 
*
* Nullify the tid field of each timer node.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS: 
* <void>
*/
LOCAL void initializeTimerArray
   (
   )
   {
   int i;

   for (i = 0; i < vmexMaxTimers; i++)
      vmexTimerArray[i].tid = NULL;

   vmexTimerArrayInitialized = TRUE;
   }
#endif

/*******************************************************************************
*
* sigTcbPut - put the pointer to the signal tcb in another task's TCB.
*
* Get the pointer to the signal tcb.  If there is no signal tcb, try and
* allocate it.
*
*/

static struct sigtcb *sigTcbPut 
    (
    WIND_TCB *pTcb
    )
    {
    struct sigtcb *pSigTcb;
    int ix;

    if (INT_RESTRICT () != OK)
	return (NULL);			/* errno was set */

    if (pTcb->pSignalInfo != NULL)
	return (pTcb->pSignalInfo);

    pSigTcb = (struct sigtcb *) taskStackAllot((int) pTcb,
					       sizeof (struct sigtcb));
    if (pSigTcb == NULL)
	{
	errnoSet (ENOMEM);
	return (NULL);
	}

    pTcb->pSignalInfo = pSigTcb;
    bzero ((void *) pSigTcb, sizeof (struct sigtcb));

    for (ix = 0; ix <= _NSIGS; ix++)
	pSigTcb->sigt_qhead[ix].sigq_next = pSigTcb->sigt_qhead[ix].sigq_prev =
		&pSigTcb->sigt_qhead[ix];

    return (pSigTcb);
    }

/*****************************************************************************
*
* vmexTCBInit - routine to initialize a task's tcb extension.
*
* This routine creates a tcb extension, initializes it, and adds it to the 
* task's tcb.  It also identifies the task as a vmex task.
*
* INCLUDE FILES: vmexLib.h
*
* RETURNS: 
* EOK if successful
* ERROR if unable to malloc TCB extension or create event semaphore
*/

STATUS vmexTCBInit
    (
    int tid				/* task ID to add extension to */
    )
    {
    WIND_TCB *	pTcb;			/* the task's control block (TCB) */
    VMEX_SHADOW_TCB *	pShadow;	/* the TCB extension */ 

    pTcb = taskTcb(tid);         	/*  Get the TCB */

    /* Allocate an extension */

    if ((pShadow = malloc (sizeof (VMEX_SHADOW_TCB))) == NULL ) 
	return (ERROR);  

    /* Reset events, signals, timers, and  create event semaphore */

    pShadow->events = 0;
    pShadow->eventWait = semBCreate (SEM_Q_FIFO, SEM_EMPTY);
    if ((STATUS) pShadow->eventWait == ERROR )
	return (ERROR);
    pShadow->asrBusy = FALSE;
#ifdef VMEX_USE_TIMER_ARRAY
    if (! vmexTimerArrayInitialized)
	initializeTimerArray();
    pShadow->timerCount = 0;
#else
    pShadow->pTimerList = NULL;
#endif

    /* Save the initial priority */

    pShadow->initialPriority = pTcb->priority;

    /* Hook up the TCB extension */

#ifndef TASKVAR_TCB_EXTENSION
    /* Add the vmex identifier */
    pTcb->reserved1 = VMEX_COOKIE;
    pTcb->reserved2 = (int) pShadow;
#else
    taskVarAdd (tid, (int *) &pTcbExtension);
    taskVarSet (tid, (int *) &pTcbExtension, (int) pShadow);
#endif

    /* Add a delete hook to clean up after a task if the task
     * is not killed by a t_delete.
     */
    if ( !vmexDeleteHookInitialized)
	{
        taskDeleteHookAdd ( (FUNCPTR) vmexCleanup);
	vmexDeleteHookInitialized = TRUE;
	}

    if (sigTcbPut (pTcb) == NULL)
        return (ERROR);

    return (EOK);
    }

/*****************************************************************************
*
* vmexTCBRemove - routine to remove the tcb extension from a task.
*
* this routine cleans up the task's tcb extension, and returns it to the 
* memory pool.
*
* INCLUDE FILES: vmexLib.h
*
* return - <void>
*/

void vmexTCBRemove
    (
    int tid				/* task ID to remove extension from */
    )
    {
    WIND_TCB *		pTcb;
    VMEX_SHADOW_TCB *	pShadow;	/* the TCB extension */ 
    VMEX_TIMER_NODE *	pTimer;

    taskLock();       				/* TASKLOCK */

    if ( (pShadow = vmexTcb (tid)) == NULL )
       {
       taskUnlock ();				/*TASK UNLOCK */
       return;
       }

    pTcb = taskTcb (tid);
#ifndef TASKVAR_TCB_EXTENSION
    pTcb->reserved2 = NULL;
    pTcb->reserved1 = NULL;
#else
    (void) taskVarDelete (tid, (int *) &pTcbExtension);
#endif

    /*  Delete all the armed timers */

#ifdef VMEX_USE_TIMER_ARRAY
    {
    int i;
    if (pShadow->timerCount > 0)
       for (i=0; i < vmexMaxTimers; i++)
	   if (vmexTimerArray[i].tid == tid)
	       {
               wdDelete (vmexTimerArray[i].timer);
	       vmexTimerArray[i].tid = NULL;
	       }
    }
#else
    while ((pTimer = pShadow->pTimerList) != NULL )           
	{
	pTimer->tid = NULL;
        wdDelete (pTimer->timer);
	pShadow->pTimerList = pTimer->next;
        (void) free (pTimer);
	}
#endif

    taskUnlock ();				/*TASK UNLOCK */

    semDelete (pShadow->eventWait);      /* free the event semaphore */
    free (pShadow);
    }

LOCAL void vmexCleanup
    (
    WIND_TCB * pTcb
    )
    {

    if (vmexTcb ((UINT32) pTcb) == NULL) 
	return;

    vmexTCBRemove ((UINT32) pTcb); 	/* clean up the TCB extension */
    }
