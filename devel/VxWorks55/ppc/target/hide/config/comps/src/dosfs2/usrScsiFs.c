/* usrScsiFs.c -- SCSI filesystem initialization */

/* Copyright 2003 Wind River Systems, Inc. */

/* 
modification history
--------------------
01a,20jan03,tor	written
*/

/*
DESCRIPTION
This file is used to configure and initialize a VxWorks filesystem
based on a SCSI device.
This file is included by the prjConfig.c configuration file created
by the Project Manager.


SEE ALSO: usrExtra.c

NOMANUAL
*/


/* includes */

#include "vxWorks.h"
#include "string.h"
#include "dosFsLib.h"
#include "dcacheCbio.h"
#include "dpartCbio.h"
#include "usrFdiskPartLib.h"
#include "drv/hdisk/ataDrv.h"
#include "scsiLib.h"

#ifdef INCLUDE_CDROMFS
#include "cdromFsLib.h"
#endif

/* defines */

#ifndef SCSI_CACHE_SIZE
#define SCSI_CACHE_SIZE 0x0
#endif /* SCSI_CACHE_SIZE */

#define SCSIFS_ERR_RETURN(msg)	\
			{ \
			printErr(msg); \
			return(ERROR); \
			}

/* typedefs */

/* globals */

IMPORT SCSI_CTRL * pSysScsiCtrl;	/* default SCSI_CTRL */
IMPORT CBIO_DEV_ID cbioWrapBlkDev(BLK_DEV * pBlkDev);

/* locals */

/* forward declarations */

STATUS usrScsiFsConfig
    (
    int		scsiFsDevBusId,
    int		scsiFsDevLun,
    int		scsiFsDevReqSenseLen,
    int		scsiFsDevType,
    BOOL	scsiFsDevRemovable,
    int		scsiFsDevNumBlocks,
    int		scsiFsDevBlkSize,
    int		scsiFsOffset,
    char *	scsiFsMountPoints
    );

/*******************************************************************************
*
* usrScsiFsConfig - Configure a dosFs 2 filesystem on SCSI device
*
* This routine mounts a DOS filesystem from a SCSI hard disk.
* Parameters:
*
* .IP <SCSI_FS_DEV_BUS_ID>
* the SCSI BUS ID of the device to be mounted
* .IP <SCSI_FS_DEV_LUN>
* the SCSI Logical Unit Number of the device to be mounted
* .IP <SCSI_FS_DEV_REQ_SENSE_LEN>
* the size of the device's REQUEST SENSE frame
* .IP <SCSI_FS_DEV_TYPE>
* the type of drive
* .IP <SCSI_FS_DEV_REMOVABLE>
* whether or not the drive is removable
* .IP <SCSI_FS_DEV_NUM_BLOCKS>
* the number of blocks on the device
* .IP <SCSI_FS_DEV_BLK_SIZE>
* the size of a single block on the device
* .IP <SCSI_FS_OFFSET>
* the offset to the beginning of the filesystem
* .IP <SCSI_FS_MOUNT_POINT>
* the name(s) of the filesystem mount points
*
* The SCSI_FS_MOUNT_POINT argument is a string which specifies
* the names of mount points for the disk partitions.  The string
* consists of a series of comma-separated mount points.  Any
* spaces in the string will be included in the filesystem names,
* so care should be taken not to include them.  There may be up
* to PART_MAX_ENTRIES (24) partitions specified.
* 
* Optionally, the string may terminate with a specification of the
* type of filesystem, enclosed in parentheses.  Currently, only dos
* and cdrom filesystem types are supported.  Cdrom filesystem supports
* only one partition.
*
* EXAMPLE:
*    usrScsiFsConfig (2,0,0,0,FALSE,0,512,0,"/scsi2a,/scsi2b(dos)");
*
*  RETURNS: none
*
*  ERRNO: not set
*/

STATUS usrScsiFsConfig
    (
    int		scsiFsDevBusId,
    int		scsiFsDevLun,
    int		scsiFsDevReqSenseLen,
    int		scsiFsDevType,
    BOOL	scsiFsDevRemovable,
    int		scsiFsDevNumBlocks,
    int		scsiFsDevBlkSize,
    int		scsiFsOffset,
    char *	scsiFsMountPoints
    )
    {
    SCSI_CTRL *		pScsiCtrl;
    SCSI_PHYS_DEV *	pScsiPhysDev;
    BLK_DEV *		pBlkDev;
    CBIO_DEV_ID		cbio;
    CBIO_DEV_ID		masterCbio;
    char *		devNames[PART_MAX_ENTRIES];
    char *		devNamesCopy;
    char *		freePtr;
    char *		sysType;
    char *		pTmpType;
    int			pn;
    int			ix;
    int			numPart = 0;
    STATUS		statVal;

    /* sanity checks */

    if ( scsiFsDevBusId < SCSI_MIN_BUS_ID || scsiFsDevBusId > SCSI_MAX_BUS_ID )
	SCSIFS_ERR_RETURN("usrScsiFsConfig: Illegal BUS ID\n");
    if ( scsiFsDevLun < SCSI_MIN_LUN || scsiFsDevLun > SCSI_MAX_LUN )
	SCSIFS_ERR_RETURN("usrScsiFsConfig: Illegal LUN\n");
    /* NOTE: numBlocks specifies maximum of 1Terrabyte disk size */
    if ( scsiFsDevNumBlocks < 0 )
	SCSIFS_ERR_RETURN("usrScsiFsConfig: Invalid number of blocks\n");
    if ( scsiFsDevBlkSize < 64 )
	SCSIFS_ERR_RETURN("usrScsiFsConfig: Illegal block size\n");
    if ( scsiFsOffset < 0 )
	SCSIFS_ERR_RETURN("usrScsiFsConfig: Illegal (negative) offset\n");
    if ( NULL == scsiFsMountPoints || EOS == *scsiFsMountPoints )
	SCSIFS_ERR_RETURN("usrScsiFsConfig: No Mount point specified\n");

    /* verify SCSI initialized & present */
    if ( NULL == pSysScsiCtrl )
	SCSIFS_ERR_RETURN("usrScsiFsConfig: SCSI Subsystem not initialized\n");

    /* 
     * make private copy of the devNames, SPR#70337 
     * strlen does not count EOS, so we add 1 to malloc.
     */

    devNamesCopy = malloc (1 + (int) (strlen (scsiFsMountPoints)));   
    if (NULL == devNamesCopy)
        SCSIFS_ERR_RETURN ("usrScsiFsConfig: malloc returned NULL\n");

    /* save string to safe place for free() */
    freePtr = devNamesCopy;

    /* copy string, include EOS */
    strcpy(devNamesCopy, scsiFsMountPoints);

    /* check for file system spec, default is "dos" */
    sysType = index(devNamesCopy, '(');
    if ( NULL != sysType )
	{
	*sysType = EOS;
	sysType++;
	pTmpType = index( sysType, ')' );
	if ( NULL != pTmpType )
	    {
	    *pTmpType = EOS;
	    }
	}
    else
	{
	sysType = "dos";
	}

    /* Parse the partition device name string */
    for ( numPart = 0 ; numPart < PART_MAX_ENTRIES ; numPart++ )
	{
	if ( EOS == *devNamesCopy )
	    break;

	pTmpType = devNamesCopy;
	devNames[numPart] = devNamesCopy;
	pTmpType = index(pTmpType, ',');
	if ( NULL == pTmpType )
	    {
	    numPart++;
	    break;
	    }
	*pTmpType = EOS;
	devNamesCopy = pTmpType + 1;
	}

    /* probe SCSI bus */
    scsiAutoConfig(pSysScsiCtrl);

#ifdef SYS_SCSI_CONFIG
    /* run BSP configuration */
    sysScsiConfig();
#endif SYS_SCSI_CONFIG

    /* create physical device for the entire disk */
    pScsiPhysDev = scsiPhysDevCreate( pSysScsiCtrl,
		scsiFsDevBusId, scsiFsDevLun,
    		scsiFsDevReqSenseLen, scsiFsDevType,
   	 	scsiFsDevRemovable, scsiFsDevNumBlocks,
    		scsiFsDevBlkSize);
    if ( NULL == pScsiPhysDev )
	{
	if ( NULL != pSysScsiCtrl )
	    {
	    /* check if scsiAutoConfig() created physical device already */
	    for ( ix = 0 ; ix < SCSI_MAX_PHYS_DEVS ; ix++ )
		{
		pScsiPhysDev = pSysScsiCtrl->physDevArr[ix];
		if ( NULL != pScsiPhysDev )
		    {
		    /* The SCSI_PHYS_DEV struct stores the LUN of the dev,
		     * but not the ID.  We verify what we're able. */

		    if ( pScsiPhysDev->scsiDevLUN == scsiFsDevLun )
		        break;
		    }
		}
	    }
	}
    if ( NULL == pScsiPhysDev )
	SCSIFS_ERR_RETURN("usrScsiFsConfig: Can't create SCSI_PHYS_DEV\n");

    /* create block device for the entire disk */
    pBlkDev = scsiBlkDevCreate(pScsiPhysDev,
		scsiFsDevNumBlocks, scsiFsOffset);
    if ( NULL == pBlkDev )
	SCSIFS_ERR_RETURN("usrScsiFsConfig: Can't create BLK_DEV\n");

    if ( strcmp (sysType, "dos" ) == 0 )
	{
	/* create disk cache for entire drive */
	cbio = dcacheDevCreate((CBIO_DEV_ID)pBlkDev, NULL, SCSI_CACHE_SIZE, freePtr);
	if ( NULL == cbio )
	    {
	    /* insufficient memory, will try without cache */
	    printErr("WARNING: usrScsiFsConfig: Failed to create"
			" %d bytes of disk cache,"
			" SCSI disk %s configured without cache\n",
			SCSI_CACHE_SIZE, scsiFsMountPoints );
	    cbio = cbioWrapBlkDev(pBlkDev);
	    if ( NULL == cbio )
		SCSIFS_ERR_RETURN("usrScsiFsConfig: Can't create CBIO\n");
	    }

	/* create partition manager */
	masterCbio = dpartDevCreate (cbio, numPart, usrFdiskPartRead);
	if ( NULL == masterCbio )
	    SCSIFS_ERR_RETURN("usrScsiFsConfig: Can't create partition manager\n");

	/* Create a DosFs device for each partition required */

	for ( pn = 0 ; pn < numPart ; pn++ )
	    {
	    statVal = dosFsDevCreate (devNames[pn], dpartPartGet (masterCbio, pn),
			    NUM_DOSFS_FILES, NONE);
	    if ( ERROR == statVal )
		{
		printErr("usrScsiFsConfig: Error creating"
			    " dosFs device %s, errno=0x%x\n",
			    devNames[pn], errno);
		free(freePtr);
		return(ERROR);
		}
	    }
	}
#ifdef INCLUDE_CDROMFS

    else if (strcmp (sysType, "cdrom") == 0)
        {

        if (cdromFsDevCreate (devNames[0], pBlkDev) == NULL)
            {
            printErr ("usrScsiFsConfig: Error creating"
			" cdromFs device %s, errno=%x\n",
                     	devName[0], errno);

            free (freePtr);

            return (ERROR);
            }
        }
#endif

    else
	{
	printErr ("usrScsiFsConfig: Unknown on un-included filesystem type: \"%s\"\n", sysType);
	free(freePtr);
	return(ERROR);
	}

    free (freePtr);
    return(OK);
    }

