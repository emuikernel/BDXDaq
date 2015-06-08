/* usrWdbSys.c - configuration file for the system mode WDB agent */

/* Copyright 1997-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,03jun98,dbt	 check if WDB core is initialized before initializing 
		 system mode. 
		 Set mode to system mode only if the task mode agent is not
		 available.
01a,21jul97,ms   taken from 01y of usrWdb.c
*/

/*
DESCRIPTION

This library configures and initializes the system mode WDB agent.
*/

/* externals */

extern BOOL		wdbIsInitialized;

/* locals */

static uint_t	wdbExternStackArray [WDB_STACK_SIZE/sizeof(uint_t)];

/******************************************************************************
*
* wdbSysModeInit - configure and initialize the system mode WDB agent.
*
*/

STATUS wdbSysModeInit (void)
    {
    caddr_t	pExternStack;

    if (!wdbIsInitialized)
	return (ERROR);

#if	_STACK_DIR == _STACK_GROWS_DOWN
    pExternStack = (caddr_t)&wdbExternStackArray
				[WDB_STACK_SIZE/sizeof(uint_t)];
    pExternStack = (caddr_t)STACK_ROUND_DOWN (pExternStack);
#else	/* _STACK_DIR == _STACK_GROWS_UP */
    pExternStack = (caddr_t)wdbExternStackArray;
    pExternStack = (caddr_t)STACK_ROUND_UP (pExternStack);
#endif	/* _STACK_DIR == _STACK_GROWS_DOWN */

    if (wdbExternInit (pExternStack) != OK)
	return (ERROR);

    if (!wdbRunsTasking())
	wdbModeSet (WDB_MODE_EXTERN);

    return (OK);
    }

#ifdef	INCLUDE_KERNEL
/******************************************************************************
*
* wdbSp - spawn a task with default params
*/ 

void wdbSp
    (
    int (*func)(),
    int arg0,
    int arg1,
    int arg2,
    int arg3,
    int arg4
    )
    {
    taskSpawn (NULL, WDB_SPAWN_PRI, WDB_SPAWN_OPTS, WDB_SPAWN_STACK_SIZE, func,
			arg0, arg1, arg2, arg3, arg4, 0, 0, 0, 0, 0);
    }
#endif	/* INCLUDE_KERNEL */
