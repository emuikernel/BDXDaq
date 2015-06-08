/* scsi2Lib.h - SCSI library header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,08may02,dat  cplusplus protection SPR 74987
01b,19aug95,jds  added comments 
01a,10oct94,jds  merged for SCSI1 and SCSI2 compatability
*/

#ifndef __INCscsiLibh
#define __INCscsiLibh

#ifdef __cplusplus
extern "C" {
#endif

typedef struct scsifunctbl
    {
    FUNCPTR scsiCtrlInit;		/* SCSI controller initialisation */
    FUNCPTR scsiBlkDevInit;		/* Initialise a block device */
    FUNCPTR scsiBlkDevCreate;		/* Create a block device */
    FUNCPTR scsiBlkDevShow;		/* Show list of block devices */ 
    FUNCPTR scsiPhaseNameGet;		/* Get the name of the SCSI phase */
    FUNCPTR scsiPhysDevCreate;		/* Create a SCSI physical device */
    FUNCPTR scsiPhysDevDelete;		/* Delete a SCSI physical device */
    FUNCPTR scsiPhysDevIdGet;		/* Get the SCSI ID of the device */
    FUNCPTR scsiAutoConfig;		/* Configure all devices on SCSI bus */
    FUNCPTR scsiShow;			/* Show all configured SCSI devices */
    FUNCPTR scsiBusReset;		/* Reset the SCSI bus */
    FUNCPTR scsiCmdBuild;		/* Build a SCSI CDB */
    FUNCPTR scsiTransact;		/* Transact an entire SCSI command */
    FUNCPTR scsiIoctl;			/* SCSI I/O control operations */
    FUNCPTR scsiFormatUnit;		/* Format a SCSI direct access dev */
    FUNCPTR scsiModeSelect;		/* Issue a MODE_SELECT command */
    FUNCPTR scsiModeSense;		/* Issue a MODE_SENSE command */
    FUNCPTR scsiReadCapacity;		/* Issue a READ CAPACITY command */
    FUNCPTR scsiRdSecs;			/* Read SCSI disk sectors */
    FUNCPTR scsiWrtSecs;		/* Write SCSI disk sectors */
    FUNCPTR scsiTestUnitRdy;		/* Issue a TEST_UNIT_READY command */
    FUNCPTR scsiInquiry;		/* Issue an INQUIRY command */
    FUNCPTR scsiReqSense;		/* Issue a REQUEST_SENSE command */
    } SCSI_FUNC_TBL;

#ifdef __cplusplus
}
#endif

/* The default is to include SCSI1 headers */

#ifndef INCLUDE_SCSI2

#include "scsi1Lib.h"

#else

#include "scsi2Lib.h"

#endif

#endif /* INCscsiLibh */
