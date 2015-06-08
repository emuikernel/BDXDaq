/* usrScript.c - startup script module */

/* Copyright 1992-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,08nov01,cmf  fix undeclared params
01c,22oct01,cmf  add support for tftp for SPR 34476
01b,18nov96,dbt  ANSIfied function headers (SPR #7427.)
		 Replaced variable named new with newFdIn for C++ compatibility.
01a,18sep92,jcf  written.
*/

/*
DESCRIPTION
This file is used to execute a startup script of VxWorks shell commands.
This file is included by usrConfig.c.

SEE ALSO: usrExtra.c

NOMANUAL
*/

/******************************************************************************
*
* usrStartupScript - make shell read initial startup script file
*
* This routine reads a startup script before the VxWorks
* shell comes up interactively.
*
* NOMANUAL
*/

void usrStartupScript 
    (
    char *fileName
    )
    {
    int old;
    int newStdIn;
	int errFd;
	BOOT_PARAMS params;

    if ((fileName == NULL) || (fileName [0] == EOS))
        return;
   
    #ifdef INCLUDE_TFTP_CLIENT 

    /* if tftp defined get boot params to check boot method*/
    bootStringToStruct (BOOT_LINE_ADRS, &params);
    sysFlags = params.flags;

    if (sysFlags & SYSFLG_TFTP)		/* if tftp used for booting*/
        {                               /* use tftpXfer to open descriptor */

	    if (tftpXfer (params.hostName, 0, fileName, "get",\
               "binary", &newStdIn, &errFd) == ERROR)
	        {
    	    return;
	        }
	    }

	else 
 	    {
	    newStdIn = open (fileName, O_RDONLY, 0);
	    }

    #else

    newStdIn = open (fileName, O_RDONLY, 0);

    #endif


    if (newStdIn != ERROR)
        {
        printf ("Executing startup script %s ...\n", fileName);
        taskDelay (sysClkRateGet () / 2);

        old = ioGlobalStdGet (STD_IN);  /* save original std in */
        ioGlobalStdSet (STD_IN, newStdIn);   /* set std in to script */
        shellInit (SHELL_STACK_SIZE, FALSE);    /* execute commands */

        /* wait for shell to finish */
        do
            taskDelay (sysClkRateGet ());
        while (taskNameToId (shellTaskName) != ERROR);

        close (newStdIn);
        ioGlobalStdSet (STD_IN, old);   /* restore original std in */

        printf ("\nDone executing startup script %s\n", fileName);
        taskDelay (sysClkRateGet () / 2);
        }
    else
        printf ("Unable to open startup script %s\n", fileName);
    }

