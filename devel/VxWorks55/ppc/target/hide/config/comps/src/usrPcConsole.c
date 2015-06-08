/* usrPcConsole.c - PC console configuration file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,03jun98,ms   written
*/

/*
DESCRIPTION

User configurable PC console intialization.
*/

#ifndef	N_VIRTUAL_CONSOLES
#define	N_VIRTUAL_CONSOLES	1
#endif

#ifndef	PC_CONSOLE
#define	PC_CONSOLE		0
#endif

/******************************************************************************
*
* usrPcConsoleInit - initialize the PC console
*/

STATUS usrPcConsoleInit (void)
    {
    int ix;
    char tyName[30];

    pcConDrv ();
    for (ix = 0; ix < N_VIRTUAL_CONSOLES; ix++)
	{
	sprintf (tyName, "%s%d", "/pcConsole/", ix);
	(void) pcConDevCreate (tyName,ix, 512, 512);
	if (ix == PC_CONSOLE)		/* init the console device */
	    {
	    consoleFd = open (tyName, O_RDWR, 0);
	    (void) ioctl (consoleFd, FIOBAUDRATE, CONSOLE_BAUD_RATE);
	    (void) ioctl (consoleFd, FIOSETOPTIONS, OPT_TERMINAL);
	    }
	}
    ioGlobalStdSet (STD_IN,  consoleFd);
    ioGlobalStdSet (STD_OUT, consoleFd);
    ioGlobalStdSet (STD_ERR, consoleFd);

    return (OK);
    }
