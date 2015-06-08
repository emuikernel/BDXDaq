/* usrNtPassFs.c - VxSim pass-through filesystem initialization */

/* Copyright 2001-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,05apr02,hbh  Completed fix for SPR# 30368.
01a,07sep01,hbh  written based on usrPassFs.c file. Fixed SPR 30368.   
*/

/*
DESCRIPTION

User configurable pass-thorough filesystem intialization for VxSim.
*/

/******************************************************************************
*
* usrNtPassFsInit - initialize pass-through filesystem
*/ 

STATUS usrNtPassFsInit (void)
    {
    IMPORT STATUS	ntPassFsInit ();
    IMPORT void *	ntPassFsDevInit ();
    IMPORT char		fullExePath[];
    char		passName [MAX_FILENAME_LENGTH];
    char *		defPathEnd;

    if (ntPassFsInit (1) != OK)
	return (ERROR);

    if (ntPassFsDevInit ("host:") == NULL)
	{
	printf ("ntPassFsDevInit failed\n");
	return (ERROR);
	}

    sprintf (passName, "host:%s", fullExePath);

    /* Remove bootFile name at the end of the string */

    defPathEnd = strrchr (passName, '/');
    if (defPathEnd != NULL)
    	*defPathEnd = '\0';
    ioDefPathSet (passName);

    return (OK);
    }
