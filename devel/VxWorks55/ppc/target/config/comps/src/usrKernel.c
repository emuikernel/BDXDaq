/* usrKernel.c - wind kernel initialization */

/* Copyright 1992-1999 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,20jan99,jpd  added INITIAL_MEM_ALLOCATION.
01e,18sep95,ism  imported conditional include of wvLib.h from bootConfig.c
01d,28mar95,kkk  moved kernel defines to configAll.h
01c,07dec93,smb  configuration change for windview
01b,10nov92,jcf  configuration change for MicroWorks.
01a,18sep92,jcf  written.
*/

/*
DESCRIPTION
This file is used to configure and initialize the Wind kernel.  This file is
included by usrConfig.c.

NOMANUAL
*/

#ifdef	INCLUDE_WDB
#define	FREE_MEM_START_ADRS (FREE_RAM_ADRS + WDB_POOL_SIZE)
#else	/* ! INCLUDE_WDB */
#define	FREE_MEM_START_ADRS FREE_RAM_ADRS
#endif

#ifdef  INCLUDE_INITIAL_MEM_ALLOCATION
#define MEM_POOL_START (char *) \
	    (ROUND_UP(FREE_MEM_START_ADRS, (INITIAL_MEM_ALIGNMENT)) + \
	    (INITIAL_MEM_SIZE))
#else   /* INCLUDE_INITIAL_MEM_ALLOCATION */
#define MEM_POOL_START (char *) FREE_MEM_START_ADRS
#endif  /* INCLUDE_INITIAL_MEM_ALLOCATION */
/* global variables */

#ifdef	INCLUDE_CONSTANT_RDY_Q
BMAP_LIST	readyQBMap;		/* bit mapped ready queue list array */
#endif	/* INCLUDE_CONSTANT_RDY_Q */

/*******************************************************************************
*
* usrKernelInit - configure kernel data structures
*
* NOMANUAL
*/

void usrKernelInit (void)

    {
    classLibInit ();			/* initialize class (must be first) */
    taskLibInit ();			/* initialize task object */

    /* configure the kernel queues */

#ifdef	INCLUDE_CONSTANT_RDY_Q
    qInit (&readyQHead, Q_PRI_BMAP, (int)&readyQBMap, 256); /* fixed ready q */
#else
    qInit (&readyQHead, Q_PRI_LIST);	/* simple priority ready q */
#endif	/* !INCLUDE_CONSTANT_RDY_Q */

    qInit (&activeQHead, Q_FIFO); 	/* FIFO queue for active q */
    qInit (&tickQHead, Q_PRI_LIST); 	/* simple priority semaphore q*/

    workQInit ();			/* queue for deferred work */


    /* start the kernel specifying usrRoot as the root task */

    kernelInit ((FUNCPTR) usrRoot, ROOT_STACK_SIZE, MEM_POOL_START,
                sysMemTop (), ISR_STACK_SIZE, INT_LOCK_LEVEL);
    }

