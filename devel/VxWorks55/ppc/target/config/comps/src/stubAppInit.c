/* usrAppInit.c - stub application initialization routine */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,02jun98,ms   written
*/

/*
DESCRIPTION
Initialize user application code.
*/ 

/******************************************************************************
*
* usrAppInit - initialize the users application
*/ 

void usrAppInit (void)
    {
#ifdef	USER_APPL_INIT
	USER_APPL_INIT;		/* for backwards compatibility */
#endif

    /* add application specific code here */
    }

