/* usrLoadSym.c - development symbol table loader */

/* Copyright 1992-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01l,24may02,wap  made previous fix conditional on INCLUDE_NET_INIT
01k,10may02,wap  Reduce task priority below tNetTask's before doing netLoad()
                 (SPR #76107)
01j,29oct01,jkf  added rip's SPR 62094 & 62122 fixes, made a symlink from
                 src/config/usrLoadSym.c to this version.  Changed DESC.
01i,15apr99,sn   fixed SPR 26726 (=SPR 22644):
                 prevent netSymTbl from ever firing linked C++ ctors
01h,03jun98,ms   modfied for config tool
01g,19nov97,hdn  added support for TFFS.
01f,18nov96,dbt  ANSIfied function headers (SPR #7427.)
01e,01nov96,hdn  added support for PCMCIA.
01d,23jul96,hdn  added support for ATA driver.
01c,25oct94,hdn  swapped 1st and 2nd parameter of usrFdConfig().
01b,13nov93,hdn  added support for floppy and IDE drive.
01a,18sep92,jcf  written.
*/

/*
DESCRIPTION
This file is used to load the development symbol table from an
external source into the target runtime.  Possible sources are
the network, or from a local disc (floppy, ata/ide, scsi, etc).
This file is included as a component of either usrConfig.c for
BSP directory builds, or from the project facility.

NOMANUAL
*/

#ifndef  __INCusrLoadSymc
#define  __INCusrLoadSymc

#ifdef  INCLUDE_NET_SYM_TBL

/********************************************************************************
* netLoadSymTbl - load system symbol table from network (or from SCSI)
*
* This routine creates a system symbol table and loads it across the network.
* The name of the symbol table file is constructed as:
*      <host>:<bootFile>.sym
*
* RETURNS: OK, or ERROR if symbol table was not loaded.
*
* NOMANUAL
*/

STATUS netLoadSymTbl ()

    {
    char symTblFile [PATH_MAX + 1];   /* name of symbol table file */
#ifdef INCLUDE_NET_INIT
    IMPORT int netTaskPriority;
    int oldTaskPriority;

    taskPriorityGet (0, &oldTaskPriority);
#endif /* INCLUDE_NET_INIT */

    if (strncmp (sysBootParams.bootDev, "scsi", 4) == 0)
	sprintf (symTblFile, "%s.sym", sysBootParams.bootFile);
#ifdef INCLUDE_IDE
    else if (strncmp (sysBootParams.bootDev, "ide", 3) == 0)
	{
        int drive = 0;
        int type = 0;

	if (strlen (sysBootParams.bootDev) == 3)
	    return (ERROR);
	else
	    sscanf (sysBootParams.bootDev, "%*3s%*c%d%*c%d", &drive, &type);

	if (usrIdeConfig (drive, sysBootParams.bootFile) != OK)
	    return (ERROR);

	sprintf (symTblFile, "%s.sym", sysBootParams.bootFile);
	}
#endif /* INCLUDE_IDE */
#ifdef INCLUDE_ATA
    else if (strncmp (sysBootParams.bootDev, "ata", 3) == 0)
        {
        int ctrl  = 0;
        int drive = 0;
        char tmp[BOOT_FILE_LEN];

        if (strlen (sysBootParams.bootDev) == 3)
            return (ERROR);
        else
            sscanf (sysBootParams.bootDev, "%*3s%*c%d%*c%d", &ctrl, &drive);

        /*
         * SPR 62094, 62122:  When dosFs2 happened, the file usrAta.c was
         * modified to take into account partitioned ata drives.  Part of that
         * revision was a change to the handling of the third parameter in the
         * call, from a single string to a comma delimited list of partition
         * names.  That change, however, was not reflected in this file.
         *
         * To correct, we need split off the filename from the device name
         * before passing it (the device name) to usrAtaConfig.
         */
        devSplit(sysBootParams.bootFile, tmp);

        if (usrAtaConfig (ctrl, drive, tmp) != OK)
            return (ERROR);

        sprintf (symTblFile, "%s.sym", sysBootParams.bootFile);
        }
#endif /* INCLUDE_ATA */
#ifdef INCLUDE_FD
    else if (strncmp (sysBootParams.bootDev, "fd", 2) == 0)
	{
        int drive = 0;
        int type = 0;

	if (strlen (sysBootParams.bootDev) == 2)
	    return (ERROR);
	else
	    sscanf (sysBootParams.bootDev, "%*2s%*c%d%*c%d", &drive, &type);

	if (usrFdConfig (drive, type, sysBootParams.bootFile) != OK)
	    return (ERROR);

	sprintf (symTblFile, "%s.sym", sysBootParams.bootFile);
	}
#endif /* INCLUDE_FD */
#ifdef INCLUDE_PCMCIA
    else if (strncmp (sysBootParams.bootDev, "pcmcia", 6) == 0)
	{
	int sock = NONE;

	if (strlen (sysBootParams.bootDev) == 6)
	    return (ERROR);
	else
	    sscanf (sysBootParams.bootDev, "%*6s%*c%d", &sock);

	/* we try a block device first then an ethernet device */

	if (usrPcmciaConfig (sock, sysBootParams.bootFile) != OK)
            sprintf (symTblFile, "%s:%s.sym", sysBootParams.hostName,
		     sysBootParams.bootFile);

	sprintf (symTblFile, "%s.sym", sysBootParams.bootFile);
	}
#endif /* INCLUDE_PCMCIA */
#ifdef INCLUDE_TFFS
    else if (strncmp (sysBootParams.bootDev, "tffs", 4) == 0)
	{
        int drive = 0;
        int removable = 0;

	if (strlen (sysBootParams.bootDev) == 4)
	    return (ERROR);
	else
	    sscanf (sysBootParams.bootDev, "%*4s%*c%d%*c%d", &drive, &removable);

	if (usrTffsConfig (drive, removable, sysBootParams.bootFile) != OK)
	    return (ERROR);

	sprintf (symTblFile, "%s.sym", sysBootParams.bootFile);
	}
#endif /* INCLUDE_TFFS */
    else
        {
#ifdef INCLUDE_NET_INIT
        taskPrioritySet (0, netTaskPriority + 1);
#endif /* INCLUDE_NET_INIT */

        sprintf (symTblFile, "%s:%s.sym", sysBootParams.hostName,
	         sysBootParams.bootFile);
        }

    printf ("Loading symbol table from %s ...", symTblFile);

    if (loadSymTbl (symTblFile) == ERROR)
	{
	taskDelay (sysClkRateGet () * 3);	/* wait 3 seconds */
#ifdef INCLUDE_NET_INIT
        taskPrioritySet (0, oldTaskPriority);
#endif /* INCLUDE_NET_INIT */
	return (ERROR);
	}

    printf ("done\n");
#ifdef INCLUDE_NET_INIT
    taskPrioritySet (0, oldTaskPriority);
#endif /* INCLUDE_NET_INIT */
    return (OK);
    }

/*******************************************************************************
*
* loadSymTbl - load system symbol table
*
* This routine loads the system symbol table.
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/

STATUS loadSymTbl 
    (
    char *symTblName
    )
    {
    char *loadAddr;
    int savedCplusXtorStrategy;
    int symfd = open (symTblName, O_RDONLY, 0);

    if (symfd == ERROR)
	{
	printf ("Error opening %s: status = 0x%x\n", symTblName, errno);
	return (ERROR);
	}

    /* load system symbol table */

    loadAddr = 0;		/* to prevent symbols from being relocated */

    /* SPR 22644 - loadModuleAt will fire any C++ ctors it finds
     * unless the ctors strategy is set to MANUAL. We must prevent
     * this since loadSymTbl should just "load the symbol table"! */

    /* save ctors strategy and temporarily set to MANUAL*/

    savedCplusXtorStrategy = cplusXtorStrategy;
    cplusXtorStrategy = MANUAL;

    if (loadModuleAt (symfd, HIDDEN_MODULE | ((sysFlags & SYSFLG_DEBUG)
					      ? LOAD_ALL_SYMBOLS
					      : LOAD_GLOBAL_SYMBOLS),
		      &loadAddr, &loadAddr, &loadAddr) == NULL)
	{  
	printf ("Error loading symbol table: status = 0x%x\n", errno);
	close (symfd);

	/* restore ctors strategy */  
	cplusXtorStrategy = savedCplusXtorStrategy;

	return (ERROR);
	}

    close (symfd);

    /* restore ctors strategy */  
    cplusXtorStrategy = savedCplusXtorStrategy;
  
    return (OK);
    }

#ifdef PRJ_BUILD
/******************************************************************************
*
* usrLoadSyms - load symbols
*/ 

void usrLoadSyms (void)
    {
    sysSymTbl = symTblCreate (SYM_TBL_HASH_SIZE_LOG2, TRUE, memSysPartId);

    netLoadSymTbl ();
    }

#endif /* PRJ_BUILD */

#endif  /* INCLUDE_NET_SYM_TBL */
#endif  /* __INCusrLoadSymc */
