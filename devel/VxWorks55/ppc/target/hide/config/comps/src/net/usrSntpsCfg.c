/* usrSntpsCfg.c - Initialization routine for the SNTP server */

/* Copyright 1992 - 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,26mar02,vvv  check for errors (SPR #73503)
01b,26oct01,vvv  fixed to allow SNTP server to start when target not booted 
		 from network device (SPR #70115)
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette contains the init routine for the INCLUDE_SNTPS 
component. It initializes the SNTP server.

NOMANUAL
*/


STATUS usrSntpsInit ()
    {
    char	devName [MAX_FILENAME_LENGTH];	/* device name */

    /*
     * For now, set server to listen on primary interface.
     * Eventually, need to allow for multiple network devices.
     */

    devName [0] = EOS;
    sprintf(devName, "%s%d", pDevName, sysBootParams.unitNum);
    if (sntpsInit (devName, SNTPS_MODE, SNTPS_DSTADDR, SNTPS_INTERVAL, 
		   SNTP_PORT, SNTPS_TIME_HOOK) == ERROR)
        printf ("Unable to start SNTP server; errno = 0x%x\n", errno);
    }
