/* usrDosDisk.c - DOS disk configuration file */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,02jun98,ms   written
*/

/*
DESCRIPTION

User configurable DOS disk intialization used by VxSim.
*/

/******************************************************************************
*
* usrDosDiskInit - initialize the DOS pass-through FS.
*/ 

STATUS usrDosDiskInit (void)
    {
    char unixName [80];
    extern void unixDrv ();
    extern void unixDiskInit ();
    extern char *u_progname;  /* home of executable */
    char *pLastSlash;

    unixDrv ();

    pLastSlash = strrchr (u_progname, '/');
    pLastSlash = (pLastSlash == NULL) ? u_progname : (pLastSlash + 1);
    sprintf (unixName, "/tmp/%s%d.dos", pLastSlash, sysProcNumGet());
    unixDiskInit (unixName, "A:", 0);

    return (OK);
    }
