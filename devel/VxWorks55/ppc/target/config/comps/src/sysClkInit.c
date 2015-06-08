/* sysClkInit.c - initialize the system clock */

/* Copyright 1984-1998 Wind River Systems, Inc. */ 

/*
modification history
--------------------
01a,21apr98,ms   take from usrConfig.c
*/ 

/*
DESCRIPTION
Initialize the system clock
*/  

/******************************************************************************
*
* usrClock - user-defined system clock interrupt routine.
*/

void usrClock (void)
    {
    tickAnnounce();
    }

/******************************************************************************
*
* sysClkInit - initialize the system clock
*/ 

void sysClkInit (void)
    {
    /* set up the system timer */

    sysClkConnect ((FUNCPTR) usrClock, 0);      /* connect clock ISR */
    sysClkRateSet (SYS_CLK_RATE);               /* set system clock rate */
    sysClkEnable ();                            /* start it */
    }

