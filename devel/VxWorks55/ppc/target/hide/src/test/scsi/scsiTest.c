/* scsiTest.c - Program to test SCSI library */

/* Copyright 1994 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,25sep96,dds  added documentation and corrected for wrs coding standards.
01b,17jun94,jds  added check in testDirectCmdsAll for 10 byte commands not
                 supported by SCSI device.
01a,29apr94,jds  written
*/

/* 
DESCRIPTION
These set of routines are written to test the functionality of the SCSI
library. This object module must be loaded after "scsiTestLib.o" has
been loaded.
*/

#define INCLUDE_SCSI2
#include "vxWorks.h"
#include "tickLib.h"
#include "taskLib.h"
#include "ioLib.h"
#include "memLib.h"
#include "usrLib.h"
#include "fppLib.h"
#include "scsiLib.h"
#include "scsi2Lib.h"
#include "stdio.h"

/* defines */

#define	FACTOR (0x800 * 512)
#define	TOTAL 0x1000000
#define	BLOCK (0x400 * 512)
#define	LINES (TOTAL / FACTOR)
#define	THIS_TASK 0

/* Note: change size according to the tape drive under test */

#define FIXED_BLK_SIZE 0x400	/* default fixed block size for Exabyte drive */


IMPORT	errno;
IMPORT STATUS errnoGet ();
IMPORT STATUS scsiTargetOptionsGet ();
IMPORT STATUS scsiTargetOptionsSet ();
IMPORT STATUS scsiTestTestUnitRdy ();
IMPORT VOID logMsg ();
IMPORT STATUS scsiTestReqSense ();
IMPORT STATUS scsiTestTapeModeSense ();
IMPORT STATUS scsiTestTapeModeSelect ();
IMPORT STATUS scsiTestReadBlockLimits ();
IMPORT STATUS scsiTestLoad ();
IMPORT STATUS scsiTestUnload ();
IMPORT STATUS scsiTestReserveUnit ();
IMPORT STATUS scsiTestRewind ();
IMPORT STATUS scsiTestErase ();
IMPORT STATUS scsiTestReleaseUnit ();
IMPORT STATUS scsiTestWrtFileMarks ();
IMPORT STATUS scsiTestSpace ();
IMPORT STATUS scsiTestWrtTapeVar ();
IMPORT STATUS scsiTestWrtTapeFixed ();
IMPORT STATUS scsiTestRdTapeVar ();
IMPORT STATUS scsiTestRdTapeFixed ();
IMPORT STATUS scsiTestInquiry ();
IMPORT STATUS scsiTestModeSense ();
IMPORT STATUS scsiTestModeSelect ();
IMPORT STATUS scsiTestReserve ();
IMPORT STATUS scsiTestRelease ();
IMPORT STATUS scsiTestReadCapacity ();
IMPORT STATUS scsiTestStartStopUnit ();
IMPORT STATUS scsiTestFormatUnit ();
IMPORT STATUS scsiTestWrtSecs ();
IMPORT STATUS scsiTestRdSecs ();
IMPORT STATUS scsiIoctl ();

/* stdlib functions */

IMPORT int strcpy ();
IMPORT int strlen ();
IMPORT int strcmp ();
IMPORT int bzero ();
IMPORT int strncmp ();

/* global variables */

char scsiTestBuffer[500];
int  scsiMaxBlockLimit;
UINT16 scsiMinBlockLimit;

union mixed
{
    char * string;
    int testNo;
};
	
/******************************************************************************
*
* optionSet - set maxOffset and minPeriod for the scsi target
*
* This function call scsiTargetOptionsSet to set option for scsi target
* Variables need to be changed:
*		busID
*		which
*		option.maxOffset/option.minPeriod
* 
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

int optionSet()
    {
    SCSI_OPTIONS	option;
    
    int which;
    int busID = 0;
    option.maxOffset = 0;
    option.minPeriod = 25;
    
    which = SCSI_SET_OPT_XFER_PARAMS; 
    
    if (scsiTargetOptionsSet(pSysScsiCtrl, busID, &option, which) == ERROR) 
	{
	SCSI_DEBUG_MSG("userScsiConfig: could not set options\n",0,0,0,0,0,0);
	return(ERROR);
	}
    return (OK);
    }

/******************************************************************************
*
* testCommonCmds - test scsi commands common to sequential & direct devices
*
*  test all mandatory and commonn scsi commands for sequential devices and
*  direct access devices. These commands are
*		1. TEST UNIT READY
*		2. REQUEST SENSE
*		3. INQUIRY
* 
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS testCommonCmds
    (
    SCSI_PHYS_DEV *pScsiPhysDev
    )
    {
    /* retry if unit attention */
    if (scsiTestTestUnitRdy (pScsiPhysDev) == ERROR)
        {
        if (errnoGet() == S_scsiLib_UNIT_ATTENTION)
            {
            if (scsiTestTestUnitRdy (pScsiPhysDev) == ERROR)
                return (ERROR);
            else
                printf ("Pass TEST UNIT READY\n");
            }
        else
            return (ERROR);
        }
    
    /* if TUR OK then do a Request Sense */
    if (scsiTestReqSense (pScsiPhysDev, scsiTestBuffer,
			  REQ_SENSE_ADD_LENGTH_BYTE + 1) == ERROR)
        return (ERROR);
    
    /* if REQUEST SENSE OK then do an INQUIRY */
    if (scsiTestInquiry (pScsiPhysDev, scsiTestBuffer, 36) == ERROR)
        return (ERROR);
    
    return (OK);
    }

/******************************************************************************
*
* testDirectCmdsAll - test all direct access device (disk) commands
*
* test all the direct access commands. FORMAT command is optionally tested
* by specifying the boolean parameter doFormat as TRUE
* test the following commands:
*		1. MODE SENSE 6
*		3. MODE SELECT 6
* 		5. RESERVE
*		6. RELEASE
*		7. READ CAPACITY
*		8. READ 6
*		10. WRITE 6
*		12. START STOP UNIT
*		13. FORMAT (optional)
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS testDirectCmdsAll
    (
    SCSI_PHYS_DEV *pScsiPhysDev,
    SCSI_BLK_DEV *pBlkDev,
    BOOL 	   doFormat
    )
    {

    int  lastLBA;
    int  blkLength;
    char *buffer;
    int  i;
    int  blockNumber;
    int  ctrlBusId;
    
    
    buffer = (char *) malloc (512 * 257);
    
    /* 6 Byte Mode Sense */
    
    if (scsiTestModeSense (pScsiPhysDev, 0x00, 0x00, buffer, 0x0c) == ERROR)
        return (ERROR);
    
    buffer[0] = 0x00;
    buffer[1] = 0x00;
    buffer[2] = buffer[2] & 0x7f;
    
    /* 6 Byte Mode Select */
    
    if (scsiTestModeSelect (pScsiPhysDev, 1, 0, buffer, 0x0c) == ERROR)
        return (ERROR);
    
    if (scsiTestReserve (pScsiPhysDev) == ERROR)
        return (ERROR);
    
    if (scsiTestRelease (pScsiPhysDev) == ERROR)
        return (ERROR);
    
    if (scsiTestReadCapacity (pScsiPhysDev, &lastLBA, &blkLength) == ERROR)
        return (ERROR);
    
    if (scsiTestStartStopUnit (pScsiPhysDev, FALSE) == ERROR)
        return (ERROR);
    
    if (scsiTestStartStopUnit (pScsiPhysDev, TRUE) == ERROR)
        return (ERROR);
    
    if (doFormat)
        {
        if (scsiTestFormatUnit (pScsiPhysDev, 0, 0, 0, 0, (char *) NULL, 0)
	    == ERROR)
            return (ERROR);
        }
        
    ctrlBusId = pScsiPhysDev->pScsiCtrl->scsiCtrlBusId;
    
    blockNumber = 1000 * ctrlBusId;
    
    /* initialize buffer with pattern */
    
    for (i=0; i < 512; i++)
        buffer[i] = 'a';
    
    /* 6 BYTE WRITE AND READ */
    
    /* write the pattern to 1 sector */
    if (scsiTestWrtSecs (pBlkDev, blockNumber, 1, buffer) == ERROR)
        return (ERROR);
    
    /* read pattern from sector written to */
    if (scsiTestRdSecs (pBlkDev, blockNumber, 1, buffer) == ERROR)
        return (ERROR);
    
    free (buffer);
    
    return (OK);
    }

/******************************************************************************
*
* testDirectRW - test direct access device (disk) read and write commands
*
* test the following scenarios:
*		1. write, read and check data pattern for 6Byte commands
*		2. write, read and check data pattern for 10Byte commands
* In doing so, cache coherency will automatically get tested
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS testDirectRW
    (
    SCSI_PHYS_DEV *pScsiPhysDev, /* physical device structure pointer    */
    SCSI_BLK_DEV *pBlkDev,       /* ptr to block device                  */
    int  numLoops,		 /* number of times to do RW test        */
    BOOL noDataCompare 		 /* default (0) means do data comparison */
    )
    {
    char *buffer;
    int  i;
    int blockNumber;
    int loops;
    int ctrlBusId;
    
    if (numLoops == 0)
	numLoops = 1;
    
    buffer = (char *) malloc (512 * 1);
    
    ctrlBusId = pScsiPhysDev->pScsiCtrl->scsiCtrlBusId;
    
    blockNumber = 1000 * ctrlBusId;
    /* if numLoops is negative then loop forever */
    for (loops = 0; loops != numLoops; loops++)
	{
        /* initialize buffer with pattern */
        if (!noDataCompare)
            { 
            for (i=0; i < 512; i++)
                buffer[i] = 'a';
            }
	
        /* 6 BYTE WRITE AND READ */
	
        /* write the pattern to 1 sector */
        if (scsiTestWrtSecs (pBlkDev, blockNumber, 1, buffer) == ERROR)
            return (ERROR);
	
        /* clear pattern from buffer */
	if (!noDataCompare)
            bzero (buffer, 512);
	
        /* read pattern from sector written to */
        if (scsiTestRdSecs (pBlkDev, blockNumber, 1, buffer) == ERROR)
            return (ERROR);
	
        /* compare pattern */
        if (!noDataCompare) 
    	    {
            for (i=0; i < 512; i++)
                {
                if (buffer[i] != 'a')
                    {
                    printf ("Failed reading disk. Incorrect pattern\n");
                    return (ERROR);
                    }
                }
            }
	
	printf ("Pass disk READ/WRITE Test\n");
        }
    
    free (buffer);
    
    return (OK);
    }

/******************************************************************************
*
* scsiTestIoctl - test a device-specific control function
*
* This routine performs a specified function using a specified SCSI block
* device. The command issued here is the TEST UNIT READY command. The command
* is built and passed on to scsiIoctl() in scsiLib
*
* RETURNS: The status of the request, or ERROR if the request is unsupported
*/

STATUS scsiTestIoctl 
    (
    SCSI_PHYS_DEV *pScsiPhysDev		/* ptr to SCSI block device info */
    )
    {
    SCSI_COMMAND myScsiCmdBlock;        /* SCSI command byte array */
    SCSI_TRANSACTION myScsiXaction;     /* info on a SCSI transaction */
    
    /* build the test unit ready command */
    if (scsiCmdBuild (myScsiCmdBlock, &myScsiXaction.cmdLength,
		      SCSI_OPCODE_TEST_UNIT_READY, pScsiPhysDev->scsiDevLUN,
		      FALSE, 0, 0, (UINT8) 0) == ERROR)
	return (ERROR);
    
    myScsiXaction.cmdAddress    = myScsiCmdBlock;
    myScsiXaction.dataAddress   = NULL;
    myScsiXaction.dataDirection = NONE;
    myScsiXaction.dataLength    = 0;
    myScsiXaction.addLengthByte = NONE;
    myScsiXaction.cmdTimeout    = SCSI_TIMEOUT_5SEC;
    
    return (scsiIoctl (pScsiPhysDev, FIOSCSICOMMAND, (int) &myScsiXaction));
    }

/******************************************************************************
*
* validateParameterString - parse the paramater string
*
* This routine parses the parmater string and checks the validity of
* each parameter.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS validateParameterString 
    (
    char * paramString,
    char * test,                  /* which test to run: #1,#2,#3 or all */
    int  * scsiBusId,             /* scsi id of the device */
    int  * devLun,                /* lun id of the device  */
    int  * testNo,                 /* which test to run: #1,#2,#3 or all */
    int  * numIterationsRW,       /* how many times to run RW test      */
    int  * numBlocks,             /* no. of blocks in block device */
    int  * blkOffset              /* address of first block in volume */
    )
    {
    sscanf (paramString, "%s %d %d %d %x %x", test, scsiBusId, devLun, 
	    numIterationsRW, numBlocks, blkOffset);

    if ((*scsiBusId < 0) || (*scsiBusId > 7))
       {
       printf ("Invalid SCSI BUS ID: Range [0..7]\n");
       return (ERROR);
       }
    if ((*devLun < 0) || (*devLun > 7))
       {
       printf ("Invalid Logical Unit\n");
       return (ERROR);
       }	 

    if ((((int) atoi (test) >= 1) && ((int) atoi (test) <= 3)) ||
	 (strcmp (test,"-a") == 0))
	{
	if (strcmp (test,"-a") == 0)
	    *testNo = 0;
	else
	    *testNo = (int) atoi (test);
	}
    else
	{
	printf ("Invalid Test: Range [1..3 or -a]\n");
	return (ERROR);
	}

    if (*numIterationsRW < 0)
       {
       printf ("Invalid number of iterations\n");
       return (ERROR);
       }
    if (*numBlocks < 0)
       {
       printf ("Innvalid Number of blocks\n");
       return (ERROR);
       }
    if (*blkOffset < 0)
       {
       printf ("Innvalid block address\n");
       return (ERROR);
       }
    else
      return (OK);
    }

/******************************************************************************
*
* scsiDiskTest - test direct access device (disk) 
*
* Three types of tests are available
*		1. testCommonCmds ()
*		2. testDirectRW ()
*		3. testDirectCmdsAll ()
*
* Specifying 1 for the whichTests parameter invokes only test#1. Specifying
* 2 invokes test#2 and 3 invokes test#3. Specifying  invokes all 3 tests
* The RW test can be passed the numIterationsRW param, which runs that test
* numIterationsRW times.
*
* RETURNS: OK if passes, or ERROR if an error is encountered.
*/

STATUS scsiDiskTest
    (
    char * paramString
    )
    {
    int scsiBusId;              /* scsi id of the device */
    int devLun;                 /* lun id of the device  */
    char *test;       	        /* which test to run: #1,#2,#3 or -[a] */
    int	testNo;       	        /* which test to run: #1,#2,#3 or all */
    int numIterationsRW; 	/* how many times to run RW test */
    int numBlocks;              /* no. of blocks in block device */
    int blkOffset;              /* address of first blcok in volume */
    SCSI_PHYS_DEV *pPhysDev;
    SCSI_BLK_DEV *pBlkDev;    

    if (paramString == NULL)
        {
	 printf ("Usage: \n");
	 printf ("Example -> scsiDiskTest \"-a 4 0 10 0x0000 0x0000\"\n");
	 printf ( "      \"\n");
	 printf ("       1. test        :  <#1,#2,#3 -[a]:runs all test>\n");
	 printf ("       2. scsiBusId   :  <Target device ID>\n");
	 printf ("       3. devLun      :  <Device Logical Unit ID>\n");       
	 printf ("       4. Iterations  :  <how many times to run RW tests\n");
	 printf ("       5. numBlocks   :  <no. of blocks in block device>\n");
	 printf ("       6. blkOffset   :  <adrs of first block in volume>\n");
	 printf ( "      \"\n");
	 return (ERROR);
         }

    test = (char *) malloc (4 * sizeof (char));

    if (validateParameterString (paramString, test, &scsiBusId, &devLun, &testNo,
				 &numIterationsRW, &numBlocks, 
				 &blkOffset) == ERROR)
      return (ERROR);

    /* create physical SCSI device */

    if ((pPhysDev=scsiPhysDevIdGet(0,scsiBusId,devLun))==(SCSI_PHYS_DEV *)NULL)
        if ((pPhysDev=scsiPhysDevCreate(pSysScsiCtrl,scsiBusId,devLun,0,
					NONE,0,0,0)) == (SCSI_PHYS_DEV *) NULL)
            {
            printf ("pkTestOneUnit: scsiPhysDevCreate() failed\n");
            return (ERROR);
            }

    /* create logical SCSI device */

    if ((pBlkDev = (SCSI_BLK_DEV *) scsiBlkDevCreate (pPhysDev, 0, 0)) == NULL)
        {
        printf ("pkTestOneUnit: scsiBlkDevCreate() failed\n");
        return (ERROR);
        }

    switch (testNo)
        {
        case 1 :
            if (testCommonCmds (pPhysDev) == ERROR)
 	        return (ERROR);
            printf ("Pass disk COMMON COMMANDS test\n\n");
	    break;

        case 2 :
            if (testDirectRW (pPhysDev, pBlkDev, numIterationsRW, FALSE) 
		== ERROR)
                return (ERROR);
            break;

	case 3 : 
            if (testDirectCmdsAll (pPhysDev, pBlkDev,  FALSE) == ERROR)
 	        return (ERROR);
            printf ("Pass disk ALL COMMANDS test\n");
	    break;

	case 0:
            if (testCommonCmds (pPhysDev) == ERROR)
	        return (ERROR);

            printf ("Pass disk COMMON COMMANDS test\n");

            if (testDirectRW (pPhysDev, pBlkDev, numIterationsRW, FALSE)
		== ERROR)
 	        return (ERROR);

            if (testDirectCmdsAll (pPhysDev, pBlkDev, FALSE) == ERROR)
 	        return (ERROR);

            printf ("Pass disk ALL COMMANDS test\n");
	    break;

        default :
	    break;
        }
    
    return (OK);
    }
