/* usrPassFs.c - VxSim pass-through filesystem initialization */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,03jun99,ms   written
*/

/*
DESCRIPTION

User configurable pass-thorough filesystem intialization for VxSim.
*/

/******************************************************************************
*
* usrPassFsInit - initialize pass-through filesystem
*/ 

STATUS usrPassFsInit (void)
    {
    extern STATUS passFsInit ();
    extern void *passFsDevInit ();
    char passName [256];
    extern char vxsim_hostname[];
    extern char vxsim_cwd[];

    if (passFsInit (1) != OK)
	return (ERROR);

    sprintf (passName, "%s:", vxsim_hostname);
    if (passFsDevInit (passName) == NULL)
	{
	printf ("passFsDevInit failed for <%s>\n", passName);
	return (ERROR);
	}

    sprintf (passName, "%s:%s", vxsim_hostname, vxsim_cwd);
    ioDefPathSet (passName);

    return (OK);
    }
