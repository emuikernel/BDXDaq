/* usrCplus.c - cplus configuration file */

/* Copyright 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,03oct01,fk   written
*/

/*
DESCRIPTION

Initialize global variable sysCplusEnable 
*/


IMPORT sysCplusEnable;

void usrCplusLibInit(void)
    {
    sysCplusEnable = TRUE;
    cplusLibInit();             /* Initialize cplus library */
    }
