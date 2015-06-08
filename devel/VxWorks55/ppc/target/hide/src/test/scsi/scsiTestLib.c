/* scsiTest.c - Program to test SCSI library */

/* Copyright 1994-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,25sep96,dds  added documentation and corrected for wrs coding standards.
01a,25jan94,ccc  written.
*/

/* 
DESCRIPTION
These set of routines are written to test the functionality of the SCSI
library.
*/

#include "vxWorks.h"
#include "tickLib.h"
#include "taskLib.h"
#include "ioLib.h"
#include "memLib.h"
#include "usrLib.h"
#include "fppLib.h"
#include "scsiLib.h"

/* defines */

#define	FACTOR (0x800 * 512)
#define	TOTAL 0x1000000
#define	BLOCK (0x400 * 512)
#define	LINES (TOTAL / FACTOR)
#define	THIS_TASK 0

#define TEST_VERBOSE 							\
if (testVerbose)							\
    printf
    
IMPORT	errno;
VOID printf ();
VOID bzero ();
IMPORT STATUS scsiTargetOptionsGet ();
IMPORT STATUS scsiTargetOptionsSet ();
IMPORT STATUS scsiTestUnitRdy ();
IMPORT void logMsg ();
IMPORT STATUS scsiTapeModeSense ();
IMPORT STATUS scsiTapeModeSelect ();
IMPORT STATUS scsiReadBlockLimits ();
IMPORT STATUS scsiLoad ();
IMPORT STATUS scsiReserveUnit ();
IMPORT STATUS scsiRewind ();
IMPORT STATUS scsiErase ();
IMPORT STATUS scsiReleaseUnit ();
IMPORT STATUS scsiWrtTape ();
IMPORT STATUS scsiWrtFileMarks ();
IMPORT STATUS scsiSpace ();
IMPORT STATUS scsiRdTape ();
IMPORT STATUS scsiReserve ();
IMPORT STATUS scsiRelease ();
IMPORT STATUS scsiStartStopUnit ();


/* global variables */

BOOL testVerbose;

/* COMMON COMMANDS */

/******************************************************************************
*
* scsiTestTestUnitRdy - run TEST UNIT READY command
*
* This routine executes the TEST UNIT READY command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestTestUnitRdy
    (
    SCSI_PHYS_DEV *pScsiPhysDev
    )
    {
    if (scsiTestUnitRdy (pScsiPhysDev) == ERROR)
	{
	printf ("Failed TEST UNIT READY\n");
        return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass TEST UNIT READY\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestInquiry - run INQUIRY command
*
* This routine executes the INQUIRY command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestInquiry
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device */
    char *buffer,			/* ptr to data buffer          */
    int  bufLength			/* length of buffer in bytes   */
    )
    {
    int i;
    
    if (scsiInquiry (pScsiPhysDev, buffer, bufLength) == ERROR)
	{
	printf ("Failed INQUIRY\n");
        return (ERROR);
	}
    
    switch (buffer [0])
	{
	case 0x00:
	    TEST_VERBOSE ("\n\nDirect-access device (e.g. disk):\n");
	    break;
	case 0x01:
	    TEST_VERBOSE ("\n\nSequential-access device (e.g. tape):\n");
	    break;
	default:
	    TEST_VERBOSE ("\n\nDevice type = 0x%02x:\n", buffer [0]);
	    break;
	}
    
    switch (buffer [2] & 0x07)
	{
	case 0x00:
	    TEST_VERBOSE ("    Pre SCSI-1 standard\n");
	    break;
	case 0x01:
	    TEST_VERBOSE ("    SCSI-1 device\n");
	    break;
	case 0x02:
	    TEST_VERBOSE ("    SCSI-2 device\n");
	    break;
	default:
	    TEST_VERBOSE ("    Reserved version number\n");
	    break;
	}
    
    switch (buffer [7] & 0x60)
	{
	case 0x40:
	    TEST_VERBOSE ("    Supports 32 bit transfers\n");
	    break;
	case 0x20:
	    TEST_VERBOSE ("    Supports 16 bit transfers\n");
	    break;
	default:
	    TEST_VERBOSE ("    Supports 8 bit transfers\n");
	    break;
	}
    
    if ((buffer [7] & 0x02) == 0x02)
	TEST_VERBOSE ("    Supports command queuing\n");
    
    TEST_VERBOSE ("ID: ");
    for (i = 8; i < 16; i++)
	TEST_VERBOSE ("%c", buffer[i]);
    TEST_VERBOSE ("\n\n");
    
    TEST_VERBOSE ("\tPass INQUIRY\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestReqSense - runs REQUEST SENSE command
*
* This routine executes the REQUEST SENSE command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestReqSense
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device */
    char *buffer,			/* ptr to data buffer          */
    int  bufLength			/* length of buffer in bytes   */
    )
    {
    if (scsiReqSense (pScsiPhysDev, buffer, bufLength) == ERROR)
	{
	printf ("Failed REQUEST SENSE\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass REQUEST SENSE\n");
    return (OK);
    }


/* SEQUENTIAL COMMANDS */

#if FALSE

/******************************************************************************
*
* scsiTestTapeModeSense - runs tape MODE SENSE command
*
* This routine executes the tape MODE SENSE command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestTapeModeSense
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device     */
    int pageControl,                    /* value of the page control field */
    int pageCode,                       /* value of the page code field    */
    char *buffer,			/* ptr to data buffer              */
    int  bufLength			/* length of buffer in bytes       */
    )
    {
    if (scsiTapeModeSense (pScsiPhysDev, pageControl, pageCode, buffer,
			   bufLength) == ERROR)
        {
	printf ("Failed tape MODE SENSE\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape MODE SENSE\n");
    return (OK);
    }

/******************************************************************************
 *
 * scsiTestTapeModeSelect - runs tape MODE SELECT command
 *
 * This routine executes the tape MODE SELECT command to the specified
 * SCSI physical device.
 *
 * RETURNS: OK if passes, or ERROR if an error is encountered.
 */
STATUS scsiTestTapeModeSelect
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device      */
    int pageFormat,                     /* value of the page format bit     */
    int saveParams,                     /* value of the save parameters bit */
    char *buffer,			/* ptr to data buffer               */
    int  bufLength			/* length of buffer in bytes        */
    )
    {
    
    if (scsiTapeModeSelect (pScsiPhysDev, pageFormat, saveParams, buffer,
			    bufLength) == ERROR)
        {
	printf ("Failed tape MODE SELECT\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape MODE SELECT\n");
    return (OK);
    
    }

/******************************************************************************
*
* scsiTestReadBlockLimits - runs tape READ BLOCK LIMITS command
*
* This routine executes the tape READ BLOCK LIMITS command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestReadBlockLimits
    (
    SCSI_PHYS_DEV *pScsiPhysDev, /* ptr to SCSI physical device          */
    int    *pMaxBlockLength,	 /* where to return maximum block length */
    UINT16 *pMinBlockLength      /* where to return minimum block length */
    )
    {
    if (scsiReadBlockLimits (pScsiPhysDev, pMaxBlockLength, 
			     pMinBlockLength) == ERROR)
        {
	printf ("Failed tape READ BLOCK LIMITS\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape READ BLOCK LIMITS\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestLoad - runs tape LOAD command
*
* This routine executes the tape LOAD command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestLoad
    (
    SCSI_PHYS_DEV *pScsiPhysDev		/* ptr to SCSI physical device      */
    )
    {
    if (scsiLoad (pScsiPhysDev, TRUE) == ERROR)
        {
	printf ("Failed tape LOAD\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape LOAD\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestUnload - runs tape UNLOAD command
*
* This routine executes the tape UNLOAD command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestUnload
    (
    SCSI_PHYS_DEV *pScsiPhysDev		/* ptr to SCSI physical device      */
    )
    {
    if (scsiLoad (pScsiPhysDev, FALSE) == ERROR)
        {
	printf ("Failed tape UNLOAD\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape UNLOAD\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestReserveUnit - runs tape RESERVE UNIT command
*
* This routine executes the tape RESERVE UNIT command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestReserveUnit
    (
    SCSI_PHYS_DEV *pScsiPhysDev		/* ptr to SCSI physical device */
    )
    {
    if (scsiReserveUnit (pScsiPhysDev) == ERROR)
        {
	printf ("Failed tape RESERVE UNIT\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape RESERVE UNIT\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestReleaseUnit - runs tape RELEASE UNIT command
*
* This routine executes the tape RELEASE UNIT command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestReleaseUnit
    (
    SCSI_PHYS_DEV *pScsiPhysDev		/* ptr to SCSI physical device */
    )
    {
    if (scsiReleaseUnit (pScsiPhysDev) == ERROR)
        {
	printf ("Failed tape RELEASE UNIT\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape RELEASE UNIT\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestRewind - runs tape REWIND command
*
* This routine executes the tape REWIND command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestRewind
    (
    SCSI_PHYS_DEV *pScsiPhysDev		/* ptr to SCSI physical device */
    )
    {
    if (scsiRewind (pScsiPhysDev) == ERROR)
        {
	printf ("Failed tape REWIND\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape REWIND\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestErase - runs tape ERASE command
*
* This routine executes the tape ERASE command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestErase
    (
    SCSI_PHYS_DEV *pScsiPhysDev		/* ptr to SCSI physical device      */
    )
    {
    if (scsiErase (pScsiPhysDev, TRUE) == ERROR)
        {
	printf ("Failed tape ERASE\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape ERASE\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestWrtFileMarks - runs tape WRITE FILE MARKS command
*
* This routine executes the tape WRITE FILE MARKS command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestWrtFileMarks
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device   */
    int numMarks,                       /* number of file marks to write */
    BOOL shortMark                      /* TRUE to write short file mark */
    )
    {
    if (scsiWrtFileMarks (pScsiPhysDev, numMarks, shortMark) == ERROR)
        {
	printf ("Failed tape WRITE FILE MARKS\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape WRITE FILE MARKS\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestSpace - runs tape SPACE command
*
* This routine executes the tape SPACE command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestSpace
   (
    SCSI_PHYS_DEV *pScsiPhysDev,      /* ptr to SCSI physical device */
    int count,                        /* count for space command    */
    int spaceCode                     /* code for the type of space command */
    )
    {
    if (scsiSpace (pScsiPhysDev, count, spaceCode) == ERROR)
        {
	printf ("Failed tape SPACE\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape SPACE\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestWrtTapeVar - runs tape WRITE command for variable block size
*
* This routine executes the tape WRITE command to the specified
* SCSI physical device. Data is written using variable block size.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestWrtTapeVar
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device   */
    int numBytes,                       /* total bytes to be written     */
    char *buffer                        /* ptr to input data buffer      */
    )
    {
    if (scsiWrtTape (pScsiPhysDev, numBytes, buffer, FALSE) == ERROR)
        {
	printf ("Failed tape WRITE Var\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape WRITE Var\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestRdTapeVar - runs tape READ command for variable block size
*
* This routine executes the tape READ command to the specified
* SCSI physical device. Data is written using variable block size.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestRdTapeVar
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device   */
    int numBytes,                       /* total bytes to be written     */
    char *buffer                        /* ptr to input data buffer      */
    )
    {
    if (scsiRdTape (pScsiPhysDev, numBytes, buffer, FALSE) == ERROR)
        {
	printf ("Failed tape READ Var\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape READ Var\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestWrtTapeFixed - runs tape WRITE command for fixed block size
*
* This routine executes the tape WRITE command to the specified
* SCSI physical device. Data is written using fixed block size.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestWrtTapeFixed
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device   */
    int numBytes,                       /* total bytes to be written     */
    char *buffer                        /* ptr to input data buffer      */
    )
    {
    if (scsiWrtTape (pScsiPhysDev, numBytes, buffer, TRUE) == ERROR)
        {
	printf ("Failed tape WRITE Fixed\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape WRITE Fixed\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestRdTapeFixed - runs tape READ command for fixed block size
*
* This routine executes the tape READ command to the specified
* SCSI physical device. Data is written using fixed block size.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestRdTapeFixed
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device   */
    int numBytes,                       /* total bytes to be written     */
    char *buffer                        /* ptr to input data buffer      */
    )
    {
    if (scsiRdTape (pScsiPhysDev, numBytes, buffer, TRUE) == ERROR)
        {
	printf ("Failed tape READ Fixed\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass tape READ Fixed\n");
    return (OK);
    }

#endif


/*DIRECT ACCESS COMMANDS */

/******************************************************************************
*
* scsiTestModeSense - runs disk MODE SENSE command
*
* This routine executes the disk MODE SENSE command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestModeSense
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device     */
    int pageControl,                    /* value of the page control field */
    int pageCode,                       /* value of the page code field    */
    char *buffer,			/* ptr to data buffer              */
    int  bufLength			/* length of buffer in bytes       */
    )
    {
    if (scsiModeSense (pScsiPhysDev, pageControl, pageCode, buffer,
		       bufLength) == ERROR)
        {
	printf ("Failed disk MODE SENSE\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass disk MODE SENSE\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestModeSelect - runs disk MODE SELECT command
*
* This routine executes the disk MODE SELECT command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestModeSelect
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to SCSI physical device      */
    int pageFormat,                     /* value of the page format bit     */
    int saveParams,                     /* value of the save parameters bit */
    char *buffer,			/* ptr to data buffer               */
    int  bufLength			/* length of buffer in bytes        */
    )
    {
    if (scsiModeSelect (pScsiPhysDev, pageFormat, saveParams, buffer,
			bufLength) == ERROR)
        {
	printf ("Failed disk MODE SELECT\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass disk MODE SELECT\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestReserve - runs disk RESERVE command
*
* This routine executes the disk RESERVE command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestReserve
    (
    SCSI_PHYS_DEV *pScsiPhysDev
    )
    {
    if (scsiReserve (pScsiPhysDev) == ERROR)
        {
	printf ("Failed disk RESERVE\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass disk RESERVE\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestRelease - runs disk RELEASE command
*
* This routine executes the disk RELEASE command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestRelease
    (
    SCSI_PHYS_DEV *pScsiPhysDev
    )
    {
    if (scsiRelease (pScsiPhysDev) == ERROR)
        {
	printf ("Failed disk RELEASE\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass disk RELEASE\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestReadCapacity - runs disk READ CAPACITY command
*
* This routine executes the disk READ CAPACITY command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestReadCapacity
    (
    SCSI_PHYS_DEV *pScsiPhysDev,/* ptr to SCSI physical device */
    int *pLastLBA,              /* last logical block address */
    int *pBlkLength             /* where to return block length */
    )
    {
    if (scsiReadCapacity (pScsiPhysDev, pLastLBA, pBlkLength) == ERROR)
        {
	printf ("Failed disk READ CAPACITY\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass disk READ CAPACITY\n");
    return (OK);    
    }

/******************************************************************************
*
* scsiTestRdSecs - runs disk READ command
*
* This routine executes the disk READ command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestRdSecs
    (
    SCSI_BLK_DEV *pScsiBlkDev,  /* ptr to SCSI block device info */
    int sector,                 /* sector number to be read */
    int numSecs,                /* total sectors to be read */
    char *buffer                /* ptr to input data buffer */
    )
    {
    if (scsiRdSecs (pScsiBlkDev, sector, numSecs, buffer) == ERROR)
        {
	printf ("Failed disk READ\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass disk READ\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestWrtSecs - runs disk WRITE command
*
* This routine executes the disk WRITE command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestWrtSecs
    (
    SCSI_BLK_DEV *pScsiBlkDev,  /* ptr to SCSI block device info */
    int sector,                 /* sector number to be written */
    int numSecs,                /* total sectors to be written */
    char *buffer                /* ptr to output data buffer */
    )
    {
    if (scsiWrtSecs (pScsiBlkDev, sector, numSecs, buffer) == ERROR)
        {
	printf ("Failed disk WRITE\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass disk WRITE\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestStartStopUnit - runs disk START STOP UNIT command
*
* This routine executes the disk START STOP UNIT command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestStartStopUnit
    (
    SCSI_PHYS_DEV *pScsiPhysDev,  /* ptr to SCSI physical device */
    BOOL start                    /* TRUE == start, FALSE == stop */
    )
    {
    if (scsiStartStopUnit (pScsiPhysDev, start) == ERROR)
        {
	printf ("Failed disk START STOP UNIT\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass disk START STOP UNIT\n");
    return (OK);
    }

/******************************************************************************
*
* scsiTestFormatUnit - runs disk FORMAT UNIT command
*
* This routine executes the disk FORMAT UNIT command to the specified
* SCSI physical device.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiTestFormatUnit
    (
    SCSI_PHYS_DEV *pScsiPhysDev,/* ptr to SCSI physical device */
    BOOL cmpDefectList,         /* whether defect list is complete */
    int defListFormat,          /* defect list format */
    int vendorUnique,           /* vendor unique byte */
    int interleave,             /* interleave factor */
    char *buffer,               /* ptr to input data buffer */
    int bufLength               /* length of buffer in bytes */
    )
    {
    if (scsiFormatUnit (pScsiPhysDev, cmpDefectList, defListFormat, 
			vendorUnique, interleave, buffer, bufLength) == ERROR)
        {
	printf ("Failed disk FORMAT UNIT\n");
	return (ERROR);
	}
    
    TEST_VERBOSE ("\tPass disk FORMAT UNIT\n");
    return (OK);
    }
