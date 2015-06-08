/* thruputTest.c - Program to obtain throughput of a scsi device (disk,tape) */

/* Copyright 1994-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
Modification History
--------------------
01b,25sep96,dds  added documentation and corrected for wrs coding standards.
01a,20may94,jds  written by adapting some basic routines in rawio.c written
                 by ccc
*/

#include "vxWorks.h"
#include "tickLib.h"
#include "taskLib.h"
#include "ioLib.h"
#include "memLib.h"
#include "usrLib.h"
#include "fppLib.h"
#include "scsiLib.h"
#include "stdio.h"
#include "string.h"


#define SECTOR_SIZE (512)		/* size of a physical sector */
#define MIN_BLOCK_SIZE (0x1000) 	/* size of the smallest block */
#define MAX_BLOCK_SIZE (0x100000)	/* size of the largest block */
#define	FACTOR (0x10)			/* factor for incremental block size */
#define	TOTAL 0x1000000 		/* total bytes to be written */

#define	THIS_TASK 0

IMPORT errno;
IMPORT STATUS scsiReadBlockLimits ();
IMPORT STATUS scsiTapeModeSense ();
IMPORT STATUS scsiTapeModeSelect ();
IMPORT STATUS scsiRewind ();
IMPORT STATUS scsiWrtTape ();
IMPORT STATUS scsiRdTape ();

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
    int  * scsiBusId,             /* scsi id of the device */
    int  * devLun,                /* lun id of the device  */
    int  * numBlocks,             /* no. of blocks in block device */
    int  * blkOffset              /* address of first block in volume */
    )
    {
    sscanf (paramString, "%d %d %x %x", scsiBusId, devLun, numBlocks,
	    blkOffset);

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
* DESCRIPTION:
* This test partions a 16 Mbyte block device into sectors of sizes 4096,
* 65536 or 1048576. Sectors consists of blocks of 512 Mbytes. This test
* writes and reads to the disk devive and calculates the time.
* 
*/

STATUS scsiDiskThruputTest
    (
    char * paramString
    )
    {
    char *buffer;
    UINT count;
    UINT blkSize;
    STATUS status = OK;
    ULONG start_tick,stop_tick;
    double time,ttime;
    int blockNumber;
    int numSects;
    int numWrites;
    int t=1;
    int scsiBusId;              /* scsi id of the device */
    int devLun;                 /* lun id of the device  */
    int numBlocks;              /* no. of blocks in block device */
    int blkOffset;              /* address of first blcok in volume */
    SCSI_PHYS_DEV *pPhysDev;
    SCSI_BLK_DEV *pBlkDev;

    if (paramString == NULL)
        {
	 printf ("Usage: \n");
	 printf ("Example -> scsiDiskThruputTest \"4 0 0x0000 0x0000\"\n");
	 printf ( "      \"\n");
	 printf ("       1. scsiBusId   :  <Target device ID>\n");
	 printf ("       2. devLun      :  <Device Logical Unit ID>\n");       
	 printf ("       3. numBlocks   :  <no. of blocks in block device>\n");
	 printf ("       4. blkOffset   :  <adrs of first block in volume>\n");
	 printf ( "      \"\n");
	 return (ERROR);
         }

    if (validateParameterString (paramString,  &scsiBusId, &devLun, &numBlocks,
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
    
    printf ("\n\nDisk write/read speed test\n");
    printf ("Total: %u bytes\n\n",TOTAL);
    
    for (blkSize = MIN_BLOCK_SIZE; blkSize <= MAX_BLOCK_SIZE; 
	 blkSize *= FACTOR)
	{
        printf("Loop %d:\n",t);
	t++;
	
	buffer = (char *) malloc (blkSize);
        if (buffer == NULL)
    	    {
	    printErrno (errno);
	    printf ("fileio: Error in allocating space\n");
	    return (ERROR);
	    }
	
        bzero (buffer, blkSize);
        taskPrioritySet (THIS_TASK, 10);
	
        printf ("Starting to run WRITE speed test for block size %d\n",
		blkSize);
	
        blockNumber = 0;
	
        numSects = blkSize / SECTOR_SIZE;
        numWrites = TOTAL / blkSize;
	
	start_tick = tickGet();
        
	printf("Writing %d times, total of %d sectors\n", 
	       numSects, numWrites);
	
	for (count = 0; count < numWrites; count++)
	    {
	    status = scsiWrtSecs (pBlkDev, blockNumber, numSects, buffer);
	    blockNumber += numSects;
	    if (status == ERROR)
	        {
	        printErrno (errno);
	        printf ("fileio: Error in writing to disk\n");
	        free (buffer);
	        return (ERROR);
	        }
	    }
	
	printf("Block Number is %d\n", blockNumber);
        stop_tick = tickGet();
	
        time = ((double) (stop_tick - start_tick)) / 60.0;
        ttime = (double) TOTAL / (time * 1024.0 * 1024.0);
        printf ("Write Run time: %4.2f sec. Transfer rate: %4.3f MB/sec.\n", 
		time, ttime);
	
        printf ("Starting to run READ  speed test for block size %d\n",
		blkSize);
        blockNumber = 100;
	
        start_tick = tickGet();
	
	printf("Reading %d times, total of %d sectors\n", 
               numSects, numWrites);
	
        for (count = 0; count < numWrites; count++)
            {
            status = scsiRdSecs (pBlkDev, blockNumber, numSects, buffer);
            if (status == ERROR)
                {
                printErrno (errno);
                printf ("fileio: Error in reading disk\n");
                free (buffer);
                return (ERROR);
                }
            }
	
        stop_tick = tickGet();
	
        time = ((double) (stop_tick - start_tick)) / 60.0;
        ttime = (double) TOTAL / (time * 1024.0 * 1024.0);
        printf ("Read Run time: %4.2f sec. Transfer rate: %4.3f MB/sec.\n\n",
		time, ttime);
        free (buffer);
	}

    scsiPhysDevDelete (pPhysDev);
    printf("rawTapeThruput test completed.\n");

    return (OK);
    }
