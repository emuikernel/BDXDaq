/* usrSerial.c - serial device configuration file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,24feb99,pr   added control for PC_CONSOLE in IO initialization (SPR#23075)
01b,11oct97,ms   use itos instead of sprintf to break dependecy on fioLib
01a,09oct97,ms   taken from usrConfig.c
*/

/*
DESCRIPTION

User configurable serial device intialization.
*/

#define	TY_NAME_BASE	"/tyCo/"

/******************************************************************************
*
* itos - given an integer, return a string representing it.
*/

static char * itos (int val)
    {
    static char str [20];
    char strtmp [20];
    str [0] = '\0';

    if (val == 0)
	return "0";

    while (val != 0)
	{
	strcpy (strtmp, str);
	str[0] = '0' + (val % 10);
	str[1] = '\0';
	strcat (str, strtmp);
	val = val - (val %= 10);
	}

    return str;
    }

/******************************************************************************
*
* usrSerialInit - initialize the serial ports
*/

STATUS usrSerialInit (void)
    {
    char tyName [20];
    int ix;

    if (NUM_TTY > 0)
	{
	ttyDrv();				/* install console driver */

	for (ix = 0; ix < NUM_TTY; ix++)	/* create serial devices */
	    {
#if     (defined(INCLUDE_WDB) && defined(INCLUDE_WDB_COMM_SERIAL))
	    if (ix == WDB_TTY_CHANNEL)		/* don't use WDBs channel */
		continue;
#endif
	    tyName[0] = '\0';
	    strcat (tyName, TY_NAME_BASE);
	    strcat (tyName, itos (ix));
	    (void) ttyDevCreate (tyName, sysSerialChanGet(ix), 512, 512);

#if  (!(defined(INCLUDE_PC_CONSOLE)))

	    if (ix == CONSOLE_TTY)		/* init the tty console */
		{
		consoleFd = open (tyName, O_RDWR, 0);
		(void) ioctl (consoleFd, FIOBAUDRATE, CONSOLE_BAUD_RATE);
		(void) ioctl (consoleFd, FIOSETOPTIONS, OPT_TERMINAL);
		}
#endif /* INCLUDE_PC_CONSOLE */

	    }
	}

    ioGlobalStdSet (STD_IN,  consoleFd);
    ioGlobalStdSet (STD_OUT, consoleFd);
    ioGlobalStdSet (STD_ERR, consoleFd);

    return (OK);
    }
