/* usrTffs.c - TFFS initialization */

/* Copyright 1992-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,07dec01,nrv  fixed diab warnings
01f,01oct01,yp  merging in T3 version, removed refs to dosFs compatibility layer
01e,02feb99,yp   added function usrTffsLnConfig() to support DOS_OPT_LONGNAMES
01d,01apr98,hdn  moved tffsLoad() back to bootConfig.c.
01c,31dec97,yp   doc cleanup
01b,01dec97,hdn  added tffsLoad() to minimize change to bootConfig.c.
01a,07nov97,hdn  written.
*/

/*
DESCRIPTION
This file is included by bootConfig.c and usrConfig.c. The file contains
routines for configuring a TFFS Flash disk to be used as file system as
with dosFs. The routines are used by the boot process to find and load
vxWorks images. 

SEE ALSO: usrExtra.c

NOMANUAL
*/

#ifndef  __INCusrTffs
#define  __INCusrTffs

/* includes */
#include "dcacheCbio.h"
#include "tffs/tffsDrv.h"
#include "tffs/flsocket.h"

/* forward declarations */


/*******************************************************************************
*
* usrTffsConfig - mount the DOS file system on a TFFS Flash disk
*
* This routine mounts the vxWorks DOS file system on a TFFS Flash drive.
*
* The <drive> parameter is the drive number of the TFFS Flash drive;
* valid values are 0 through the number of socket interfaces in BSP.
*
* The <fileName> parameter is the mount point, e.g., `/tffs0/'.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO:
* .pG "I/O System, Local File Systems"
*/

STATUS usrTffsConfig
    (
    int     drive,	/* drive number of TFFS */
    int     removable,	/* 0 - nonremovable flash media */
    char *  fileName	/* mount point */
    )
    {
    int    dosFsCacheSizeDefault       = 128 * 1024 ;
    CBIO_DEV_ID pCbio;

    BLK_DEV * pBlkDev;
    char devName [BOOT_FILE_LEN];

    if ((UINT)drive >= noOfDrives)
	{
	printErr ("drive is out of range (0-%d).\n", noOfDrives - 1);
	return (ERROR);
	}

    /* create a block device spanning entire disk (non-distructive!) */

    if ((pBlkDev = tffsDevCreate (drive, removable)) == NULL)
	{
        printErr ("tffsDevCreate failed.\n");
        return (ERROR);
	}
	
    /* split off boot device from boot file */

    devSplit (fileName, devName);

    /* initialize the block device as a dosFs device named <devName> */

    /* Create e.g. 128 Kbytes disk cache */

    if ( (pCbio = dcacheDevCreate( (CBIO_DEV_ID) pBlkDev, NULL,
                dosFsCacheSizeDefault, devName)) == NULL )
        return (ERROR);

    if (dosFsDevCreate (devName, pCbio, 0, NONE) != OK)
        return (ERROR);

    return (OK);
    }

/*******************************************************************************
*
* usrTffsLnConfig - mount the DOS file system on a TFFS Flash disk
*
* This routine mounts the vxWorks DOS file system on a TFFS Flash drive with
* long file name support.
*
* OBSOLETE: Version II of dosFs does not need special treatment for supporting 
* long filenames. This routine is provided to support legacy code only and
* will become obsolete soon.
*
* The <drive> parameter is the drive number of the TFFS Flash drive;
* valid values are 0 through the number of socket interfaces in BSP.
*
* The <fileName> parameter is the mount point, e.g., `/tffs0/'.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO:
* .pG "I/O System, Local File Systems"
*
* NOMANUAL
*/

STATUS usrTffsLnConfig
    (
    int     drive,	/* drive number of TFFS */
    int     removable,	/* 0 - nonremovable flash media */
    char *  fileName	/* mount point */
    )
    {
    
    if (usrTffsConfig (drive, removable, fileName) != OK)
	return (ERROR);

    return (OK);
    }
#endif /* __INCusrTffs */
