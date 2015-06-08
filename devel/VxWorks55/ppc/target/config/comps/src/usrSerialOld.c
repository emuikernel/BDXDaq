/* usrSerialOld.c - old (pre-tornado) serial device configuration file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,24feb99,pr   added control for PC_CONSOLE in IO initialization (SPR#23075)
01a,03jun98,ms   written
*/

/*
DESCRIPTION
User configurable old (pre-tornado) serial device intialization.
*/

/******************************************************************************
*
* usrSerialOldInit - initialize the serial ports
*/

STATUS usrSerialOldInit (void)
    {
    char tyName [20];
    int ix;

    if (NUM_TTY > 0)
	{
	tyCoDrv ();				/* install console driver */

	for (ix = 0; ix < NUM_TTY; ix++)	/* create serial devices */
	    {
	    sprintf (tyName, "%s%d", "/tyCo/", ix);

	    (void) tyCoDevCreate (tyName, ix, 512, 512);

#if  (!(defined(INCLUDE_PC_CONSOLE)))
	    if (ix == CONSOLE_TTY)
		consoleFd = open (tyName, O_RDWR, 0);
#endif /* INCLUDE_PC_CONSOLE */
	    }

	/* set baud rate */

	(void) ioctl (consoleFd, FIOBAUDRATE, CONSOLE_BAUD_RATE);
	(void) ioctl (consoleFd, FIOSETOPTIONS, OPT_TERMINAL);
	}

    ioGlobalStdSet (STD_IN,  consoleFd);
    ioGlobalStdSet (STD_OUT, consoleFd);
    ioGlobalStdSet (STD_ERR, consoleFd);

    return (OK);
    }
