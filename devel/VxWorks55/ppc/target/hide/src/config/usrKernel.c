/* usrKernel.c - wind kernel initialization */

/* Copyright 1992-2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,30nov01,bwa  Changed INCLUDE_EVENTS to INCLUDE_VXEVENTS (SPR #71920).
01i,10sep01,bwa  Added eventLibInit().
01h,12feb98,pr   added trgShowInit routine 
01g,27jan98,cth  changed INCLUDE_INSTRUMENTATION to INCLUDE_WINDVIEW, removed
		 include of wvLib.h
01f,25jan98,pr   added triggering support
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

#ifndef  __INCusrKernelc
#define  __INCusrKernelc 


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
#ifdef  INCLUDE_WINDVIEW
    wvLibInit ();			/* initialize windview first, really */
#endif	/* INCLUDE_WINDVIEW */
    classLibInit ();			/* initialize class (must be first) */
    taskLibInit ();			/* initialize task object */

#ifdef	INCLUDE_TASK_HOOKS
    taskHookInit ();			/* initialize task hook package */
#ifdef	INCLUDE_SHOW_ROUTINES
    taskHookShowInit ();		/* task hook show routine */
#endif	/* INCLUDE_SHOW_ROUTINES */
#endif	/* INCLUDE_TASK_HOOKS */

#ifdef	INCLUDE_SEM_BINARY
    semBLibInit ();			/* initialize binary semaphore */
#endif	/* INCLUDE_SEM_BINARY */

#ifdef	INCLUDE_SEM_MUTEX
    semMLibInit ();			/* initialize mutex semaphore */
#endif	/* INCLUDE_SEM_MUTEX */

#ifdef	INCLUDE_SEM_COUNTING
    semCLibInit ();			/* initialize counting semaphore */
#endif	/* INCLUDE_SEM_COUNTING */

#ifdef        INCLUDE_VXEVENTS
    eventLibInit ();
#endif        /* INCLUDE_VXEVENTS */

#ifdef	INCLUDE_SEM_OLD
    semOLibInit ();			/* initialize VxWorks 4.0 semaphores */
#endif	/* INCLUDE_SEM_OLD */

#ifdef	INCLUDE_WATCHDOGS
    wdLibInit ();			/* initialize watchdog */
#ifdef	INCLUDE_SHOW_ROUTINES
    wdShowInit ();			/* watchdog object show routine */
#endif	/* INCLUDE_SHOW_ROUTINES */
#endif	/* INCLUDE_WATCHDOGS */

#ifdef	INCLUDE_MSG_Q
    msgQLibInit ();			/* initialize message queues */
#ifdef	INCLUDE_SHOW_ROUTINES
    msgQShowInit ();			/* message queue object show routine */
#endif	/* INCLUDE_SHOW_ROUTINES */
#endif	/* INCLUDE_MSG_Q */

#ifdef	INCLUDE_SHOW_ROUTINES
    classShowInit ();			/* class object show routine */
    taskShowInit ();			/* task object show routine */
    semShowInit ();			/* semaphore object show routine */
#endif	/* INCLUDE_SHOW_ROUTINES */

    /* configure the kernel queues */

#ifdef	INCLUDE_CONSTANT_RDY_Q
    qInit (&readyQHead, Q_PRI_BMAP, (int)&readyQBMap, 256); /* fixed ready q */
#else
    qInit (&readyQHead, Q_PRI_LIST);	/* simple priority ready q */
#endif	/* !INCLUDE_CONSTANT_RDY_Q */

    qInit (&activeQHead, Q_FIFO); 	/* FIFO queue for active q */
    qInit (&tickQHead, Q_PRI_LIST); 	/* simple priority semaphore q*/

    workQInit ();			/* queue for deferred work */

#ifdef INCLUDE_TRIGGERING               /* initialize event triggering library */
    trgLibInit ();
#ifdef	INCLUDE_SHOW_ROUTINES
    trgShowInit ();			/* semaphore object show routine */
#endif	/* INCLUDE_SHOW_ROUTINES */

#endif /* INCLUDE_TRIGGERING */

    }

#endif /* __INCusrKernelc */
