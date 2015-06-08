/* usrWdbTask.c - configuration file for the WDB task agent */

/* Copyright 1997-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,26nov01,jhw  Moved wdbInfo back into WIND_TCB.
01i,20oct01,jhw  Moved wdbExitHook out of WIND_TCB.
01h,29may01,pch  Fix compiler warning
01g,18apr01,kab  Added Altivec support
01f,16nov00,zl   added DSP support.
01e,07apr99,cym  changing vxTaskRegsGet to match with src/config.
		 (SPR #26387)
01d,15feb99,dbt  remove task creation hook only if it has been previously
                 installed (SPR #24195)
01c,18may98,dbt  integrated latest WDB stuff
01b,09oct97,ms   use _func_ioTaskStdSet to break binding to iosLib
01a,21jul97,ms   taken from 01y of usrWdb.c. moved some macros to configAll.h
*/

/*
DESCRIPTION

This library configures and initializes the WDB task agent.
*/

/* defines */

#define MILLION		1000000

/* globals */

FUNCPTR			wdbTaskFpRegsGet;
FUNCPTR			wdbTaskFpRegsSet;
FUNCPTR			wdbTaskDspRegsGet;
FUNCPTR			wdbTaskDspRegsSet;
FUNCPTR			wdbTaskAvRegsGet;
FUNCPTR			wdbTaskAvRegsSet;

/* externals */

extern WDB_RT_IF 	wdbRtIf;
extern VOIDFUNCPTR	wdbTaskRestartHook;
extern BOOL		wdbIsInitialized;

/* locals */

static VOIDFUNCPTR      wdbCreateHook = NULL;   /* WDB task create hook */

/* forward declarations */

static void wdbTaskRtIfInit ();

/******************************************************************************
*
* wdbTaskRestart - restart the WDB task
*/

static void wdbTaskRestart (void)
    {
    static int  restartCnt;
    extern int  wdbTaskId;

    /*
     * if the exception is in the agent task, restart the agent
     * after a delay.
     */

    if (((int)taskIdCurrent == wdbTaskId) && (restartCnt < WDB_MAX_RESTARTS))
        {
        restartCnt++;
        if (_func_logMsg != NULL)
            _func_logMsg ("WDB exception. restarting agent in %d seconds...\n",
                WDB_RESTART_TIME, 0,0,0,0,0);
        taskDelay (sysClkRateGet() * WDB_RESTART_TIME);
        taskRestart (0);
        }
    }

/******************************************************************************
*
* wdbTaskModeInit - configure and initialize the WDB task mode agent.
*
*/

STATUS wdbTaskModeInit (void)
    {
    if (!wdbIsInitialized)	/* test if WDB is initialized */
	return (ERROR);

    wdbTaskRtIfInit ();

    if (wdbTaskInit (WDB_TASK_PRIORITY,
		WDB_TASK_OPTIONS, NULL, WDB_STACK_SIZE) == ERROR)
	return (ERROR);

    wdbModeSet (WDB_MODE_TASK);

    wdbTaskRestartHook = wdbTaskRestart;

    return (OK);
    }

/******************************************************************************
*
* vxTaskCreate - WDB callout to create a task (and leave suspended).
*/

static int vxTaskCreate
    (
    char *   name,       /* name of new task (stored at pStackBase)   */
    int      priority,   /* priority of new task                      */
    int      options,    /* task option word                          */
    caddr_t  stackBase,  /* base of stack. ignored by VxWorks	      */
    int      stackSize,  /* size (bytes) of stack needed plus name    */
    caddr_t  entryPt,    /* entry point of new task                   */
    int      arg[10],	 /* 1st of 10 req'd task args to pass to func */
    int      fdIn,	 /* fd for input redirection		      */
    int      fdOut,	 /* fd for output redirection		      */
    int      fdErr	 /* fd for error output redirection	      */
    )
    {
    int tid;

    if (stackSize == 0)
	stackSize = WDB_SPAWN_STACK_SIZE;

    tid = taskCreat (name, priority, options, stackSize, (int (*)())entryPt,
		arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6],
		arg[7], arg[8], arg[9]);

    if (tid == 0)		/* create failed */
	return (ERROR);

    if (_func_ioTaskStdSet != NULL)
	{
	if (fdIn != 0)
	    _func_ioTaskStdSet (tid, 0, fdIn);
	if (fdOut != 0)
	    _func_ioTaskStdSet (tid, 1, fdOut);
	if (fdErr != 0)
	    _func_ioTaskStdSet (tid, 2, fdErr);
	}

    return (tid);
    }


/******************************************************************************
*
* vxTaskResume - WDB callout to resume a suspended task.
*/

static STATUS vxTaskResume
    (
    WDB_CTX *	pContext
    )
    {
    if (pContext->contextType != WDB_CTX_TASK)
	return (ERROR);

    return (taskResume (pContext->contextId));
    }

/******************************************************************************
*
* vxTaskSuspend - WDB callout to suspend a task.
*/

static STATUS vxTaskSuspend
    (
    WDB_CTX *	pContext
    )
    {
    if (pContext->contextType != WDB_CTX_TASK)
	return (ERROR);

    return (taskSuspend (pContext->contextId));
    }

/******************************************************************************
*
* vxTaskDelete - WDB callout to delete a task.
*/

static STATUS vxTaskDelete
    (
    WDB_CTX *	pContext
    )
    {
    if (pContext->contextType != WDB_CTX_TASK)
	return (ERROR);

    return (taskDelete (pContext->contextId));
    }

/******************************************************************************
*
* vxTaskRegsSet - WDB callout to get a task register set.
*/

static STATUS vxTaskRegsSet
    (
    WDB_CTX *	 pContext,
    WDB_REG_SET_TYPE regSetType,
    char *	 pRegSet
    )
    {
    if (pContext->contextType != WDB_CTX_TASK)
	return (ERROR);

    switch (regSetType)
	{
	case WDB_REG_SET_IU:
	    return (taskRegsSet (pContext->contextId, (REG_SET *)pRegSet));

	case WDB_REG_SET_FPU:
	    if (wdbTaskFpRegsSet == NULL)
		return (ERROR);
	    return (wdbTaskFpRegsSet (pContext, pRegSet));

	case WDB_REG_SET_DSP:
	    if (wdbTaskDspRegsSet == NULL)
		return (ERROR);
	    return (wdbTaskDspRegsSet (pContext, pRegSet));

	case WDB_REG_SET_AV:
	    if (wdbTaskAvRegsSet == NULL)
		return (ERROR);
	    return (wdbTaskAvRegsSet (pContext, pRegSet));

	default:
	    return (ERROR);
	}
    }

/******************************************************************************
*
* vxTaskRegsGet - WDB callout to get a tasks register set.
*
* This routine is not reentrant, but it it only called by one thread (the
* WDB agent).
*/

static STATUS vxTaskRegsGet
    (
    WDB_CTX *		pContext,
    WDB_REG_SET_TYPE 	regSetType,
    char **		ppRegSet
    )
    {
    WIND_TCB * pTcb;
    if (pContext->contextType != WDB_CTX_TASK)
	return (ERROR);

    pTcb = taskTcb (pContext->contextId);
    if (pTcb == NULL)
	return (ERROR);

    switch (regSetType)
	{
	case WDB_REG_SET_IU:
	    {
	    REG_SET dummy;
	    /* DO NOT REMOVE THIS CALL TO taskRegsGet (SPR #26387) */
	    taskRegsGet (pContext->contextId,&dummy);
	    *ppRegSet = (char *) &pTcb->regs;
	    return (OK);
	    }

	case WDB_REG_SET_FPU:
	    if (wdbTaskFpRegsGet == NULL)
		return (ERROR);
	    return (wdbTaskFpRegsGet (pContext, ppRegSet));

	case WDB_REG_SET_DSP:
	    if (wdbTaskDspRegsGet == NULL)
		return (ERROR);
	    return (wdbTaskDspRegsGet (pContext, ppRegSet));

	case WDB_REG_SET_AV:
	    if (wdbTaskAvRegsGet == NULL)
		return (ERROR);
	    return (wdbTaskAvRegsGet (pContext, ppRegSet));

	default:
	    return (ERROR);
	}
    }

/******************************************************************************
*
* vxSemCreate - create a SEMAPHORE
*/

static void * vxSemCreate (void)
    {
    return ((void *)semBCreate (0, 0));
    }

/******************************************************************************
*
* vxSemGive - give a semaphore
*/

static STATUS vxSemGive
    (
    void * semId
    )
    {
    return (semGive ((SEM_ID)semId));
    }

/******************************************************************************
*
* vxSemTake - take a semaphore
*/

static STATUS vxSemTake
    (
    void *		semId,
    struct timeval *	tv
    )
    {
    return (semTake ((SEM_ID) semId, 
	(tv == NULL ? WAIT_FOREVER :
	tv->tv_sec * sysClkRateGet() +
	(tv->tv_usec * sysClkRateGet()) / MILLION)));
    }

/******************************************************************************
*
* __wdbTaskDeleteHook -
*/

static int __wdbTaskDeleteHook
    (
    WIND_TCB *pTcb
    )
    {
    WDB_CTX     ctx;
    void        (*hook)();

    hook = (pTcb->wdbInfo).wdbExitHook;

    if (hook != NULL)
        {
        ctx.contextType = WDB_CTX_TASK;
        ctx.contextId   = (UINT32)pTcb;
        (*hook) (ctx, pTcb->exitCode, pTcb->errorStatus);
        }

    return (OK);
    }

/******************************************************************************
*
* vxTaskDeleteHookAdd - task-specific delete hook (one per task).
*
* currently only one hook per task.
*/

static STATUS vxTaskDeleteHookAdd
    (
    UINT32      tid,
    void        (*hook)()
    )
    {
    static BOOL initialized = FALSE;

    if (taskIdVerify ((int)tid) == ERROR)
        return (ERROR);

    (taskTcb (tid)->wdbInfo).wdbExitHook = hook;

    if (!initialized)
        {
        taskDeleteHookAdd (__wdbTaskDeleteHook);
        initialized = TRUE;
        }

    return (OK);
    }

/******************************************************************************
*
* __wdbTaskCreateHook - task create hook
*
* This hook is called each time a task is created.
*
* RETURNS : OK always
*/ 

static int __wdbTaskCreateHook
    (
    WIND_TCB *	pTcb
    )
    {
    WDB_CTX	createdCtx;
    WDB_CTX	creationCtx;

    if (wdbCreateHook != NULL)
	{
	/* fill createdCtx structure */

	createdCtx.contextType	= WDB_CTX_TASK;
	createdCtx.contextId	= (UINT32)pTcb;

	/* fill creationCtx structure */

	creationCtx.contextType	= WDB_CTX_TASK;
	creationCtx.contextId	= (UINT32)taskIdCurrent;

	(*wdbCreateHook) (&createdCtx, &creationCtx);
	}

    return (OK);
    }

/******************************************************************************
*
* vxTaskCreateHookAdd - install WDB task create hook.
*
* This routine installs or remove the WDB task create hook. 
*
* RETURNS : OK always.
*/ 

static STATUS vxTaskCreateHookAdd
    (
    void	(*hook)()
    )
    {
    static BOOL initialized = FALSE;

    wdbCreateHook = hook;

    if ((hook == NULL) && initialized)	/* remove task creation hook */
	{
	taskCreateHookDelete (__wdbTaskCreateHook);
	initialized = FALSE;
	}
    else if (!initialized)		/* install task creation hook */
	{
	taskCreateHookAdd (__wdbTaskCreateHook);
	initialized = TRUE;
	}

    return (OK);
    }

/******************************************************************************
*
* wdbTaskRtIfInit - Initialize pointers to the VxWorks routines.
*/

static void wdbTaskRtIfInit ()
    {
    wdbRtIf.taskCreate	= vxTaskCreate;
    wdbRtIf.taskResume	= vxTaskResume;
    wdbRtIf.taskSuspend	= vxTaskSuspend;
    wdbRtIf.taskDelete	= vxTaskDelete;

    wdbRtIf.taskLock	= (VOIDFUNCPTR) taskLock;
    wdbRtIf.taskUnlock	= (VOIDFUNCPTR) taskUnlock;

    wdbRtIf.taskRegsSet	= vxTaskRegsSet;
    wdbRtIf.taskRegsGet = vxTaskRegsGet;

    wdbRtIf.malloc	= malloc;
    wdbRtIf.free	= free;

    wdbRtIf.semCreate	= vxSemCreate;
    wdbRtIf.semGive	= vxSemGive;
    wdbRtIf.semTake	= vxSemTake;

    wdbRtIf.taskDeleteHookAdd	= vxTaskDeleteHookAdd;
    wdbRtIf.taskSwitchHookAdd	= (STATUS (*)())taskSwitchHookAdd;
    wdbRtIf.taskCreateHookAdd	= (STATUS (*)())vxTaskCreateHookAdd;
    }
