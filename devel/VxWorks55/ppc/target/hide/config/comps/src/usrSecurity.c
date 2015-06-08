/* usrSecurity.c - shell login security initialization library library */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,03jun98,ms   written
*/

/*
DESCRIPTION
Initialize shell login password protection.
*/

/******************************************************************************
*
* usrSecurity - initialize the shell security
*/

void usrSecurity (void)
    {
    if ((sysFlags & SYSFLG_NO_SECURITY) == 0)
        {
        loginInit ();                           /* initialize login table */
        shellLoginInstall (loginPrompt, NULL);  /* install security program */

        /* add additional users here as required */

        loginUserAdd (LOGIN_USER_NAME, LOGIN_PASSWORD);
        }
    }

