/* usrNfs.c - Support for NFS */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,20dec01,vvv  added alloca support
01b,06nov01,vvv  made max. path length configurable (SPR #63551)
01a,25feb99,nps  written
*/

/*
DESCRIPTION
This file is used to include support for network devices using the BSD
interface. The file contents are included in the project configuration 
file when INCLUDE_NFS is defined. This file performs the necessary
configuration - namely sets the Unix authorisations as specified by
the user via the project facility.

NOMANUAL
*/

#ifdef __GNUC__
# ifndef alloca
#  define alloca __builtin_alloca
# endif
#endif

const int nfsMaxPath = NFS_MAXPATH + 1;

/******************************************************************************
*
* usrNfsInit - initialize NFS
*
* This routine initializes NFS and configures it using the following parameters
* defined by the user via the project facility.
*
*	- NFS_USER_ID
*	- NFS_GROUP_ID
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/

STATUS usrNfsInit (void)
    {
    char *devName;

    if (nfsMaxPath < 1)
	{
	printf ("Error initializing NFS, invalid NFS_MAXPATH\n");
	return (ERROR);
	}

    if ((devName = (char *) alloca (nfsMaxPath)) == NULL)
	{
	printf ("Error initializing NFS, out of memory\n");
	return (ERROR);
	}

    /*
     * The following values are the default values used in NFS.
     * They can be reset here if necessary.
     *
     *     nfsMaxMsgLen   = 8192        message size (decrease only)
     *     nfsTimeoutSec  = 5           timeout seconds
     *     nfsTimeoutUSec = 0           timeout microseconds
     */

#if (CPU==SIMHPPA)
    nfsMaxMsgLen   = 512;
#endif

    nfsAuthUnixSet (sysBootParams.hostName,
        NFS_USER_ID,
        NFS_GROUP_ID,
        0,
        (int *) 0);

    if (nfsDrv () == ERROR)     /* initialize nfs driver */
        printf ("Error initializing NFS, errno = %#x\n", errno);
    else
        {
        /* if the boot pathname starts with a device name, e.g. an nfs mount,
         * then set the current directory to that device
         */

        (void) iosDevFind (sysBootParams.bootFile, &pAddrString);
        if (pAddrString != sysBootParams.bootFile)
            {
            devName[0] = EOS;
            strncat (devName, sysBootParams.bootFile, pAddrString - sysBootParams.bootFile);
            ioDefPathSet (devName);
            }
        }

    return OK;
    }


