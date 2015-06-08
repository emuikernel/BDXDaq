/* rfDrv.c - rimfire 3200 driver */

static char *copyright = "Copyright 1989, Wind River Systems, Inc.";

/*
modification history
--------------------
01c,26jun90,jcf  changed semTake () calling sequence for lint.
01b,23may89,del  made rfMalloc and rfFree LOCALs.
01a,29nov88,del  written.
*/

/*

DESCRIPTION
This is the device driver for the Ciprico Rimfire 3200 SMD hard disk 
controller board.

USER CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system. However, several routines are callable from the application level.
These are:
rfDrv, rfCtlrInit, rfDevCreate, rfDevMount, rfDevShow, rfLabelCreate,
rfLabelPrint, rfLabelRead, rfLabelWrite, rfConfig, rfConfigGet, 
rfFormat, rfVerify.

INITIALIZATION
rfDrv

Before using the driver, it must be initialized by calling the function
rfDrv.

.CS
rfDrv (cpuDualPort)
    BOOL cpuDualPort;			/* does cpu dual port mem ? *
.CE

This function should be called exactly once, before any reads, writes, or
rfDevCreates.  Normally, it is called from usrRoot. The function rfCtlrInit
is used to initialize the controller and drive hardware,  and connect the 
interrupt handler. The argument 'cpuDualPort, tells the driver whether to
use the system memory, on the cpu board, or allocate it's own memory pool
in extened memory. The dimensions of this are given by RF_MEM_BASE and
RF_MEM_SIZE.


.CS
STATUS rfCtlrInit (addrs, intVec, intLvl, driveCnt, ctlr)
    UINT addrs;	   /* address of rf controller board *
    UINT intVec;   /* interrupt vector for controller *
    UINT intLvl;   /* interrupt  level of controller *
    UINT driveCnt; /* number of physical drives on controller *
    UINT ctlr;     /* controller number. (zero based) *
.CE    
CONFIGURING
Before a drive can be accessed by the rimfire 3200, the controller must be
configured with the geometery of the drive . If the drive has been formatted
and the label created, the call to rfCtlrInit would have obtained the 
disk geometery from the disk label and configured the controller accordingly.
If the drive has not been formatted, the function rfConfig must be used to
configure the controller so that formatting can be done.

.CS
STATUS rfConfig (ctlr, drive, pGeomReq)	
    int ctlr;		 /* controller number *
    int drive;		 /* drive number *
    DISK_GEOM *pGeomReq; /* user's requested geometry *
.CE

The DISK_GEOM structure is used to pass in the geometery information to 
the controller (see rfLabel.h). 

FORMATING
Before a disk can be used it must be formatted. To format a disk that has
never been formated by a rimfire 3200, and this driver, the following 
funtctions must used.

    rfDrv (...);	- see above.
    rfCtlrInit (...);	- see above.
    rfConfig (...)	- see above.

STATUS rfFormat (ctlr, drive, pFmtReq)
    int ctlr;			/* controller number *
    int drive;			/* drive number *
    FMT_VER_REQ *pFmtReq;	/* format request block *

The FMT_VER_REQ structure is used to tell rfFormat where to start and
stop formatting. To format the entire disk, both these fields should 
be set to zero. Formatting errors are placed in the error field before
rfFormat returns (see Ciprico manual for more information).

VERIFYING XXXsp

Once a disk has been formatted, disk defects need to be handled. Bad sectors
can be slipped or mapped, and bad tracks can be mapped. (see section 4
Functional Description - Defect Mapping in the Ciprico manual for details.)
To verify the format of a disk, the function rfVerify is used.
Because rfVerify is dependent on a device descriptor, a disk device must 
be created and the device opened. rfVerify is then called via the I/O system
using ioctl. The following example shows how to create and open a disk device.

Example:

    int fd;					/* file descriptor *
    rfDevCreate ("/d0/", 0, 1, 0, 1, 0, 0);   	/* create device *
    fd = open ("/d0/", 1);			/* open disk *
    ioctl (fd, FIOVERIFY, pFmtVerReq);		/* verify disk *

The argument pFmtVerReq is a pointer to a structure of type FMT_VER_REQ.
where the starting and ending blocks are specified. To verify the entire
disk both the starting and ending block fields should be zero. Verify will
run until a bad block is encountered. The error field of pFmtVerReq will
be set to show the error code and the badBlock field will contain the 
number of the block that is defective. This block number can then be used to
slip the sector or map the sector or track to an alternate. If the disk 
geometry is setup such that there are spare sectors per track, the sector 
can be slipped. If there are no spares or the slip command fails, then the
sector or track needs to be mapped to an alternate location on the disk.


DISK DEVICES
Before a disk device can be used, it must be created. This is done with 
the rfDevCreate call.
Each drive to be used may have one or more devices associated with it.
The way the device is created with this call determines whether 
it covers the whole disk or just part of it  , whether it is RT-11
format compatible, etc.

.CS
STATUS rfDevCreate (name, ctlr, drive, rtFmt, trkOffset, nTracks, devType)
    char *name;	    /* name of this device *
    int  ctlr;	    /* controller number . 0 based*
    int  drive;	    /* physical drive number. 1 based *
    BOOL rtFmt;	    /* TRUE if this device is RT-11 formatted *
    int  trkOffset; /* tracks offset from base of phys drive *
    int  nTracks;   /* len of dev, in tracks: 0 remainder of disk*
    int  devType;   /* disk type *
.CE

For instance, to create the device "/d0/", RT-11 compatible, covering
the whole disk, on drive 1, the proper call would be:

.CS
    rfDevCreate ("/d0/", 0, 1, TRUE, 1, 0, 0);
.CE

The arguments passed to this funtion are stored on the disk label so 
when the system is rebooted, the devices do not need to be recreated.
Once a logical device has been created on the disk it must be inittialized
by a call to diskinit() with the logical device name as its argument.

Example:
	diskinit ("/fd0d/");

This call initializes the directory structure on the disk so that it can 
be used by the file system. Because the paramters for the logical devices 
are kept in the disk label the logical devices can be mounted upon system
reboot by calling rfDevMount with the name of the logical device as its
argument. To find out the names of logical devices on a disk call rfDevShow.
Example:

    rfDevShow (ctlr, drive);
    pNo  name  drv  fmt  ofst  len  type
     0   d0     1    0     1    0     0

    rfDevMount ("/d0/");

Now the device /d0/ can be used by the I/O system.


MULTIPLE DEVICES ON A DRIVE
It is possible, and reasonable, to have more than one logical device for
a given drive. rfDevCreate can be called multiple times to create multiple 
devices.

IOCTL
This driver responds to all the same ioctl codes as a normal block
device driver.  Note that currently the defines for ioctl calls for the
rimfire exist in two places, rfIoctl.h and /usr/vw/h/ioLib.h.
*/


/* -------------------------------------------- include vxWorks --------------*/
#include "vxWorks.h"
#include "ioLib.h"
#include "iosLib.h"
#include "semLib.h"
#include "rt11Lib.h"
#include "wdLib.h"
#include "memLib.h"
#include "vme.h"

/* -------------------------------------------- rimfire includes -------------*/
#include "rfReg.h"
#include "rfIoctl.h"
/* #include "rfErr.h"   */
/* #include "rfLabel.h" */
#include "rfDrv.h"

/* -------------------------------------------- global stuff -----------------*/

DISK_GEOM geometry[NRF];			/* drive geometry       */
 /* XXX need to make geom for multiple controllers */
RF_CTLR rfCtlr[NRFC];  				/* controller specifics */
int 	rfDrvNum;				/* number of this driver*/

/* ------------------------------------------- memory partition --------------*/
PART_ID rfMemPartId;
IMPORT sysMemPartition;				

/* ------------------------------------------- forward declarations ----------*/
LOCAL int rfCreate ();
LOCAL int rfDelete ();
LOCAL int rfOpen ();
LOCAL int rfRead ();
LOCAL int rfSecRd ();
LOCAL int rfWrite ();
LOCAL int rfSecWrt ();
LOCAL STATUS rfReset ();
LOCAL STATUS rfIoctl ();
LOCAL void rfIntr ();
LOCAL void rfTimeout ();
LOCAL STATUS rfClose ();
LOCAL long rfLocalToBusAdrs ();
LOCAL STATUS rfSglCmd ();
STATUS rfFormat ();
STATUS rfVerify ();
IOCTL_INFO *rfIoctlInfoPtrGet (); 
LOCAL BOOL rfIoctlStateGet ();
LOCAL VOID rfIoctlStateSet ();
LOCAL DRIVE *rfDriveDescriptorGet ();
STATUS rfSync ();
STATUS rfConfig ();
STATUS rfConfigGet ();
char *rfMalloc ();

/*****************************************************************************
*
* rfDrv - install rimfire driver
*
* install the rimfire 3200 driver. 
*
* RETURNS: OK  | ERROR
*/
STATUS rfDrv (cpuDualPort)
    BOOL cpuDualPort;			/* does cpu dual port its ram ? */

    {
    if (rfDrvNum > 0)
        return (OK);    		/* driver already installed */
 
    if (cpuDualPort)
	rfMemPartId = NULL;
    else
	{
	if ((rfMemPartId = memPartCreate (RF_MEM_BASE, RF_MEM_SIZE)) == NULL)
	    {
	    printf ("rfDrv: can't create memory partition\n");
	    return (ERROR);
	    }
	}
    rfDrvNum = iosDrvInstall (rfCreate, rfDelete, rfOpen, rfClose,
                                 rfRead, rfWrite, rfIoctl);
    return (rfDrvNum == ERROR ? ERROR : OK);
    }

/******************************************************************************
*
* rfCtlrInit -
*
* probes for the hardware at the given addrs, and if present, initializes
* the hardware, connects interrupts, initializes the controller 
* descriptor, then initializes the drive.
*
* RETURNS: OK | ERROR
*/  
STATUS rfCtlrInit (addrs, intVec, intLvl, ctlr, driveCnt, devType )
    UINT addrs;		/* address of rf controller board */
    UINT intVec;	/* interrupt vector for controller */
    UINT intLvl;	/* interrupt  level of controller */
    UINT ctlr;		/* No. of this controller */
    UINT driveCnt;	/* No. of physical drives on controller */
    UINT devType;	/* drive device type */
 
    {
    FAST RF_CTLR *pRfCtlr = &rfCtlr[ctlr];
    FAST int i;
    FAST int drive; 	
    UINT localAdrs;

				/* get the local address for this controller */
    if (sysBusToLocalAdrs (VME_AM_SUP_SHORT_IO, addrs, &localAdrs) == ERROR)
	{
	printf ("rfDrv: bus address: %8u not accessable.\n", addrs);
        return (ERROR);
        }
     
					/* probe for hardware */
    if (rfProbe (localAdrs, ctlr) == ERROR)
        {
	printf ("rfDrv: no hdwr @ given address: %u\n", addrs);
        return (ERROR);
        }

    if ( (intConnect ( (intVec * 4 ), rfIntr, 0)) != OK )
	{
	printf ("rfCtlrInit: intConnect failed\n");
	return (ERROR);
	}

    sysIntEnable (intLvl); 
				/* null the pointers to drives, 
				 * this is important for rfDevCreate
				 * to work properly */
    for (i = 0 ; i < NRF; ++i)
	{	
	pRfCtlr->pDrives[i] = NULL;
	}

				    /* init the controller specific stuff */
    pRfCtlr->bsybit	= NULL;		/* number of drives */
    pRfCtlr->driveCnt	= driveCnt;	/* number of drives */
    pRfCtlr->localAdrs  = localAdrs;	/* local addrs of controller */
    pRfCtlr->addrs	= addrs;	/* bus address of controller */
    pRfCtlr->intVec	= intVec;	/* interrupt vector */
    pRfCtlr->intLvl	= intLvl;	/* int level for board */
    pRfCtlr->rfSemId	= semCreate (); /* controller semaphore */

    semGive (pRfCtlr->rfSemId);		/* let this ctlr be used */



			    /* allocate a Command List for this controller */
    if ((pRfCtlr->pCmdList = (CMDLIST *) rfMalloc (sizeof (CMDLIST))) ==
	     NULL )
	{
	printf ("rfDrv: can't malloc CMDLIST\n");
	return (ERROR);
	}
				/* initialize the rimfire hardware */	 
    rfHrdInit (pRfCtlr);		

    for (drive = 1; drive <= driveCnt; ++drive)
	{
	printf ("rfCtlrInit: initializing drive 0x%x\n", drive);
	rfDriveInit (pRfCtlr->ctlr, drive, devType); 
	printf ("rfCtlrInit: done\n");
	}

    return (OK);
    }

/****************************************************************************
*
* rfHrdInit - 
*
* do the controller hardware setup.
*
* RETURNS: OK | ERROR
*    
*/
LOCAL STATUS rfHrdInit (pRfCtlr)
    FAST RF_CTLR *pRfCtlr;		/* controller descriptor */

    {
    FAST EXTPB 	 *pXPB; 		/* extended parameter block */
    FAST CMDLIST *pCmdLst;		/* command list */
    SETUPPB 	*pSCLPB;		/* Setup Command List PB */
    IOCGPB 	*pIOCGPB;		/* IO Control Group PB */
    GOPTPB 	*pGOPTPB;		/* general options PB */
    FAST int 	i; 
    USHORT 	*wp;
    USHORT 	cksum;

    if ((pXPB = (EXTPB *) rfMalloc (sizeof (EXTPB))) == NULL)
	{
	printf ("rfHrdInit: can't malloc EXTPB\n");
	return (ERROR);
	}

			    /* allocate a Command List for this controller */
    if (pRfCtlr->pCmdList == (CMDLIST *) NULL)
	{
	if ((pRfCtlr->pCmdList = (CMDLIST *) rfMalloc (sizeof (CMDLIST))) ==
	    (CMDLIST *) NULL )
	    {
	    printf ("rfHrdInit: can't malloc CMDLIST\n");
	    return (ERROR);
	    }
	}
#ifdef DSKBUG
    printf ("rfHrdInit:  allocated cmdlist at %x\n", pRfCtlr->pCmdList);
#endif DSKBUG

    /* Use an extended parameter block to issue type 0 commands
    * disable interrupts and wait for these commands to complete.
    */
    pXPB->intr 		= 0;
    pXPB->errintr 	= 0;
    pXPB->resv 		= 0;

    /* -------------------------------- issue 'Setup Command List' command */
    pCmdLst 		= pRfCtlr->pCmdList;
    pCmdLst->pbin 	= 0;		/* parameter block in ptr = 0 */
    pCmdLst->pbout 	= 0;		/* pb out ptr = 0 */
    pCmdLst->sbin 	= 0;		/* status block input ptr = 0 */
    pCmdLst->sbout 	= 0;		/* sb out ptr = 0 */	
    pCmdLst->pbsize 	= RF32NPB;	/* max number of paramter blocks */
    pCmdLst->sbsize 	= RF32NSB;	/* max number of status blocks */
    pCmdLst->resv1 	= 0;
    pCmdLst->resv2 	= 0;

    pSCLPB 		= (SETUPPB *)&pXPB->pb;
    pSCLPB->id 		= 0;
    pSCLPB->command 	= C_SETUPCL;
    pSCLPB->iocg 	= 0;
    pSCLPB->unit 	= 0;
				    /* making done and error intrs the same */
    pSCLPB->errintr 	= (pRfCtlr->intLvl << 8) | pRfCtlr->intVec;
    pSCLPB->doneintr 	= (pRfCtlr->intLvl << 8) | pRfCtlr->intVec;	

#ifdef DSKBUG
    printf("Errintr=%x, Doneintr=%x\n", pSCLPB->errintr, pSCLPB->doneintr);
#endif DSKBUG

    if ((pSCLPB->start 	= rfLocalToBusAdrs(pCmdLst)) == NULL)
	;			/* XXX some error handling needed here */
    pSCLPB->addrmod 	= VME_ADD_MOD;
    pSCLPB->listno 	= HOST_ID;
    pSCLPB->resv[0] 	= 0;
    pSCLPB->resv[1] 	= 0;
    pSCLPB->resv[2] 	= 0;
#ifdef DSKBUG
    printf("HrdInit: trying to set up command list...\n");
#endif DSKBUG

    if (rfSglCmd (pXPB, pRfCtlr))
	{
	printf ("Cannot setup command list!\n");
	pRfCtlr->pCmdList = (CMDLIST *) NULL;
	rfFree ((char *) pCmdLst);
	return (ERROR);
	}

    /*----------------------issue 'Initialize I/O control groups' command */
    /* IOCG's in rfParam.h 
     * IOCG(1) - disable cache
     * IOCG(2) - read IOCG
     * IOCG(3) - write IOCG
     */

#ifdef DSKBUG
    printf("Setting I/O Control Groups\n");
#endif DSKBUG

    for (i = 1; i <= MAXIOCG; i++)
	{
	pIOCGPB 		= (IOCGPB *)&pXPB->pb;
	pIOCGPB->id 		= 0;
	pIOCGPB->command 	= C_SETIOCG;
	pIOCGPB->control 	= i;
	pIOCGPB->unit 		= 0;
	pIOCGPB->resv1 		= 0;
	pIOCGPB->recovery 	= iocg[i].recovery;
	pIOCGPB->readahead 	= iocg[i].readahead;
	pIOCGPB->cache 		= iocg[i].cache;
	pIOCGPB->ndretry 	= iocg[i].ndretry;
	pIOCGPB->dretry 	= iocg[i].dretry;
	pIOCGPB->resv2 		= 0;
	pIOCGPB->resv3 		= 0;

	if (rfSglCmd (pXPB, pRfCtlr))
	    {
	    printf ("rfHrdInit: Cannot set I/O Control Group %d!\n", i); 
	    return (ERROR);
	    }
#ifdef DSKBUG
    printf("HrdInit: init IOCG #%d ok.\n", i);
#endif DSKBUG
	}


    /* ----------------------------------- issue 'General Options' command */
#ifdef DSKBUG
    printf ("Setting General Board Options\n");
#endif DSKBUG

    pGOPTPB 		= (GOPTPB *)&pXPB->pb;
    pGOPTPB->id 	= 0;
    pGOPTPB->command 	= C_OPTION;
    pGOPTPB->control 	= 0;
    pGOPTPB->unit 	= 0;
    pGOPTPB->throttle 	= THROTTLE;
    pGOPTPB->sbenable 	= GOPTWDL | GOPTDTE | GOPTIDE;
    pGOPTPB->sbintr 	= GOPTWDL;
    pGOPTPB->resv[0] 	= 0;
    pGOPTPB->resv[1] 	= 0;

    if (rfSglCmd (pXPB,  pRfCtlr))
	{
	printf ("rfHrdInit: Cannot set General Board Options!\n");
	return (ERROR);
	}
				/* make sure EXTPB won't cause interrupts */
    pXPB->intr 		= 0;
    pXPB->errintr 	= 0;
    pXPB->resv 		= 0;
    return (OK);
    }
/******************************************************************************
*
* rfDriveInit -
*
* initialize the given drive.
*
* RETURNS: OK | ERROR
*/  
LOCAL STATUS rfDriveInit (ctlr, drive, devType)
    FAST int ctlr;			/* controller number */
    FAST int drive;			/* drive number */
    FAST int devType;			/* SMD device type */

    {
    FAST DISK_GEOM *pDskGeom;	     /* disk geometry pointer */
    FAST CONFIG_GET_REQ *pConfGetReq; /* request drive configuration */
    FAST SMDCHRS   *pChrs;	     /* SMD characteristics pointer */
    FAST EXTPB	   *pXPB;	     /* extended parameter block */
    FAST RF_LABEL  *pLabel;	     /* ptr to label structure */
    FAST RETCONF   *pRetConf;	     /* returned configuration parameters */

    /* ---------------------------------issue 'Configure Disk' command */

    pDskGeom 	= &geometry [drive]; 	
    *pDskGeom 	= smdLabel[devType].geom;

    if ((pXPB = (EXTPB *) rfMalloc (sizeof (EXTPB))) == NULL)
	{
	printf ("rfDriveInit: can't malloc EXTPB\n");
	return (ERROR);
	}

    rfXPBClear (pXPB);
    rfConfig (ctlr, drive, pDskGeom);


    /* --------------------------------- Try to read label from the drive */ 

    if ( (pLabel = (RF_LABEL *) rfMalloc (sizeof (RF_LABEL))) == NULL)
	{
	printf ("rfDriveInit: No memory for the disk label.\n");
	return (ERROR);
	}

    if (rfLabelRead (ctlr, drive, pLabel) == ERROR)
	{
	/* read failed ... */
	/* keep minimal config so label can be written */
	pDskGeom->ncyl 	 = 1;
	pDskGeom->acyl 	 = 0;
	pDskGeom->nhead  = 1;
	pDskGeom->bhead  = 0;
	pDskGeom->sectrk = 1;
	pDskGeom->intrlv = 1;
	pDskGeom->gap1 	 = 0;
	pDskGeom->gap2 	 = 0;
	rfFree ((char *)pLabel);
	return (OK);
	}

    /* -------------------------------------- this disk has a valid label */
#ifdef DSKBUG
    printf("rfDriveInit: disk label OK.\n");
    rfLabelPrint (pLabel); 
    printf("rfDriveInit: setting geom. for drive %x from label \n", drive);
#endif DSKBUG

    *pDskGeom 	= pLabel->geom;			/* set the geom from label */
    rfFree ((char *) pLabel); 			/* rfFree the label space */

    taskDelay (100);				/* so rfConfig doesn't hang */

				    /* re-configure the disk according to the 
				     * geometry read from the label */
    rfConfig (ctlr, drive, pDskGeom); 

#ifdef DSKBUG			  
    /* ------------------------------------ get the disk configuration */
    if ((pRetConf = (RETCONF *) rfMalloc (sizeof (RETCONF))) == NULL)
	{
	printf ("rfDriveInit: can't malloc RETCONF space\n");
	return (ERROR);
	}

    bzero (pRetConf, sizeof (RETCONF));
    pConfGetReq->pConfParams = pRetConf;

    if (rfConfigGet (ctlr, drive, pConfGetReq)== ERROR)
	{			/* error */
	printf("rfDriveInit: final Ret Config returned...\n");
	rfConfigParamsPrint (ctlr, drive, pConfGetReq);
	rfFree ((char *) pRetConf);
	return (ERROR);
	}
    rfFree ((char *) pRetConf);
#endif DSKBUG

#ifdef DSKBUG
    /* ------------------------------ issue an 'Interrogate Disk' command  */ 
    
    if ((pChrs = (SMDCHRS *) rfMalloc (sizeof (SMDCHRS))) == NULL)
	{
	printf ("rfDriveInit: can't malloc SMDCHRS space\n");
	return (ERROR);
	}
    bzero (pChrs, sizeof (SMDCHRS));

    pXPB.pb->id 	= 0;
    pXPB.pb->command 	= C_INTERROGATE;
    pXPB.pb->unit 	= drive;
    pXPB.pb->control 	= 0;
    pXPB.pb->addrmod 	= VME_ADD_MOD;
    if ((pXPB.pb->vmeaddr = rfLocalToBusAdrs(pChrs)) == NULL)
	;			/* XXX error hanling needed here */ 
    pXPB.pb->reserved 	= 0;
    pXPB.pb->diskaddr 	= 0;
    pXPB.pb->blkcount 	= 0;

    if (rfSglCmd (pXPB, &rfCtlr[ctlr]))
	{
	printf ("rfDriveInit: couldn't interrogte the disk!\n");
	rfFree ((char *) pChrs);
	return (ERROR); 	
	}

    rfSMDChrsPrint (ctlr, drive, pChrs);

    rfFree ( (char*) pChrs);
#endif DSKBUG
    return (OK);
    }

/************************************************************************
*
* rfProbe -
*
* function first checks to see if the board exists by reseting it.
* if the board exists we read the type of controller from
* the status port
*
* RETURNS: 
*    OK | ERROR if board not present or won't reset.
*/

LOCAL STATUS rfProbe (rf, ctlr)
    FAST RF32REG *rf;			/* adrs of rimfire control reg */
    UINT ctlr;				/* controller number */

    {
    int val = 1;
    int finished;			/* save reset complete status */

#ifdef DSKBUG
    printf ("rfProbe: controller %d at %x\n", ctlr, rf);
#endif DSKBUG

				/* Reset controller and check if it exists */
    if (vxMemProbe (&rf->reset, WRITE, 1, &val) == ERROR)
	{
#ifdef DSKBUG
	printf ("rfProbe: no controller present at %x\n", rf);
#endif DSKBUG
	return (ERROR);				/* no controller */
	}

    rfCtlr[ctlr].bsybit = 0;

					    /* Wait for reset to complete */
#ifdef TOOLONG
    CDELAY((finished = (rf->status & SWAB(STATRST)) == SWAB(RESETDONE)),
	    TOOLONG);

    if (!finished)
	{
	printf ("rfProbe: Cannot reset controller: status %x\n",
	SWAB (rf->status));
	return (ERROR);
	}
#else TOOLONG				/* wait forever */
    while ((rf->status & SWAB(STATRST)) != SWAB(RESETDONE))
	;
#endif TOOLONG

    rfCtlr[ctlr].rfctype = rf->status & STATCTYPE;

#ifdef DSKBUG
    printf ("Controller %d is a ",ctlr);
    printf ("%s\n", rfCtlr[ctlr].rfctype == STATRF3200 ? "3200" : "3400");
#endif DSKBUG

    return (OK);
    }
/******************************************************************************
*
* rfMalloc -
*
* function to allocate data from memory. Hook to modify for allocating
* data from a memory pool other than the system memory pool.
*
* RETURNS: pointer to allocated memory or 
*          NULL if error.
*/  
char *rfMalloc (nBytes)
    unsigned nBytes;			/* number of bytes to allocate */

    {

    if (rfMemPartId)
	return (memPartAlloc (rfMemPartId, nBytes));
    else
	return (malloc (nBytes));
    }
/******************************************************************************
*
* rfFree -
*
* free a previously allocated block of memory.
*
* RETURNS: OK | ERROR
*/  
STATUS rfFree (pBlock)
    char *pBlock;		/* start of block to free */

    {
    if (rfMemPartId)
	return (memPartFree (rfMemPartId, pBlock));
    else
	return (free (pBlock));
    }
/*---------------------------------------------------------------------------
 *-------------------------- controller functions ---------------------------*/

/*****************************************************************************
*
* rfCtlrGive -
*
* release control of a rimfire controller.
*
* RETURNS: nothing.
*/
LOCAL VOID rfCtlrGive (ctlr)
    int ctlr;					/* controller number */

    {
    semGive (rfCtlr[ctlr].rfSemId);
    }

/****************************************************************************
*
* rfCtlrTake -
*
* take control of a rimfire controller.
*
* RETURNS: nothing
*/
LOCAL VOID rfCtlrTake (ctlr)
    int ctlr;					/* controller number */

    {
    semTake (rfCtlr[ctlr].rfSemId, WAIT_FOREVER);
    }

/*----------------------------------------------------------------------------
 *------------------------- device functions -------------------------------*/

/*****************************************************************************
*
* rfDevCreate - 
*
* Create a logical device on a drive with a given offset and 
* length. Sets up the device with the I/O system and rt11 file system.
*
* RETURNS: OK | ERROR 
*/
 
STATUS rfDevCreate (name, ctlr, drive, rtFmt, trkOffset, nTracks, devType)
    char *name;		/* name of this device */
    int  ctlr;		/* controller number . 0 based*/
    int  drive;		/* physical drive number. 1 based */
    BOOL rtFmt;		/* TRUE if this device is RT-11 formatted */
    int  trkOffset;	/* tracks offset from base of phys drive */
    int  nTracks;	/* len of dev, in tracks: 0 remainder of disk*/
    int  devType;	/* disk type */
 
    {
    RF_LABEL 	*pLabel;
    DRIVE 	*pDrive; 
    int 	partNo;

    if (rfDrvNum <= 0)
        {
        errnoSet (S_ioLib_NO_DRIVER);
        return (ERROR);
        }
				/* get a pointer to the drive  descriptor */
    if ( (pDrive = rfDriveDescriptorGet (ctlr, drive)) == (DRIVE *) NULL)
	{
	printf ("rfDevCreate: could not get drive desc. drive: %x\n", drive);
	return (ERROR);
	}
					/* see if name exists on drive */
    if (rfDevNameCheck (ctlr, drive, name) == OK)
	{				/* it does, get out */
	printf ("rfDevCreate: device %s exists on drive %d\n", name, drive);
	return (OK);
	}
					    /* get a partition number to use */
    if ( (partNo = rfPartitionNumGet (pDrive)) == ERROR)
	{
	printf ("rfDevCreate: no more partitions available\n");
	return (ERROR);
	}

    if (rfDevSetup (ctlr, drive, name, rtFmt, trkOffset, 
		    nTracks, devType, partNo) == ERROR )
	{
	printf ("rfDevCreate: could not setup device\n");
	return (ERROR);
	}

				    /* and update the label on the drive */
    rfLabelPartMapUpdate (pDrive);

    printf ("rfDevCreate: OK.\n");

    return (OK);
    }

/****************************************************************************
*
* rfDevMount -
*
* function to mount the logical devices according to the partition map
* as read from the disk label.
*
* RETURNS: 
*    OK | ERROR if name  not found or setup failed.
*
*/  
STATUS rfDevMount (name, ctlr)
    char *name;		/* device name */
    int  ctlr;		/* controller number */

    {
    FAST RF_LABEL *pLabel;
    FAST int d;
    FAST int i;

    if ((pLabel = (RF_LABEL *) rfMalloc (sizeof (RF_LABEL)) ) == NULL)
	{
	printf ("rfDevMount: could not allocate label mem.\n");
	return (ERROR);
	}

						/* see if name exists */
    for (d = 1; d <= NRF; d++)		
	{
	if (rfLabelRead (ctlr, d, pLabel) != OK)
	    {
	    rfFree ((char *) pLabel);
	    return (ERROR);
	    }

	for (i = 0 ; i < NDKMAP; i ++)
	    {
	    if (strcmp (pLabel->map[i].name, name) == NULL)
		{
		if (rfDevSetup  (ctlr,
			     	 pLabel->map[i].drive,
			     	 pLabel->map[i].name,
				 pLabel->map[i].rtFmt,
				 pLabel->map[i].trkOffset,
				 pLabel->map[i].nTracks,
				 pLabel->map[i].devType,
				 pLabel->map[i].partNo) != OK)
		     {
		     printf ("rfDevMount: mount failed. dev: %s\n", name);
		     rfFree ((char *) pLabel);
		     return (ERROR);
		     }
		rfFree ((char *) pLabel);
		return (OK);
		}
	    }
	}
    printf ("rfDevMount: device %s not found\n", name);
    rfFree ((char *) pLabel);
    return (ERROR);
    }

/******************************************************************************
*
* rfDevNameCheck - 
*
* checks to see if the given device name exists on the given drive.
*
* RETURNS OK if name exists | 
*	  ERROR if it does not exist.
*/  
LOCAL STATUS rfDevNameCheck(ctlr, drive, name)
    int  ctlr;			/* controller number */
    int  drive;			/* drive number */
    char *name;			/* name of device to look for */

    {
    FAST RF_LABEL *pLabel;
    FAST int i;

    if ((pLabel = (RF_LABEL *) rfMalloc (sizeof (RF_LABEL)) ) == NULL)
	{
	printf ("rfDevNameCheck: could not allocate label mem.\n");
	return (ERROR);
	}

    if (rfLabelRead (ctlr, drive, pLabel) != OK)
	{
	rfFree ((char *) pLabel);
	return (ERROR);
	}

    for (i = 0; i < NDKMAP; i++)
	{
	if (strcmp (pLabel->map[i].name, name) == NULL)
	    {
	    rfFree ((char *) pLabel);
	    return (OK);
	    }
	}
    rfFree ((char *) pLabel);
    return (ERROR);
    }

/******************************************************************************
*
* rfDevSetup -
*
* initalize the logical device with the IO system and the file system.
*
* RETURNS: OK | ERROR
*
*/  
LOCAL STATUS rfDevSetup (ctlr, drive, name, rtFmt, trkOffset, 
			 nTracks, devType, partNo)
    int  ctlr;		 /* controller number */
    int  drive;          /* physical drive number */
    char *name;          /* name of this device */
    BOOL rtFmt;          /* TRUE if this device is RT-11 formatted */
    int  trkOffset;      /* number of tracks offset from beginning
                          * of physical device. */
    int  nTracks;        /* length of this device, in tracks;
                          * if 0, the rest of the disk. */
    int  devType;        /* disk type */
    int  partNo;	 /* logical partition number */
 
    {
    FAST RF_DEV *pRfDev;
    FAST DRIVE	*pDrive;

    if ( (pDrive = rfDriveDescriptorGet (ctlr, drive)) == (DRIVE *) NULL)
	{
	printf ("rfDevSetup: could not get drive desc. drive: %x\n", drive);
	return (ERROR);
	}

					/* allocate the RF_DEV descriptor */
    if ((pRfDev = (RF_DEV *) rfMalloc (sizeof (RF_DEV))) == NULL) 
	{ 
	printf ("rfDevSetup: can't allocate device.\n");
	return (ERROR); 
	}
 
					/* initialize the device structure */ 
    if (nTracks == 0)
        nTracks = pRfDev->pDiskGeom->trkdisk - trkOffset;
    pRfDev->ctlr		= ctlr;
    pRfDev->devInfo.partNo	= partNo;
    strcpy (pRfDev->devInfo.name, name); 
    pRfDev->devInfo.drive 	= drive;
    pRfDev->devInfo.rtFmt	= rtFmt;
    pRfDev->devInfo.trkOffset 	= trkOffset;
    pRfDev->devInfo.nTracks 	= nTracks;
    pRfDev->devInfo.devType	= devType;
    pRfDev->devSemId 		= semCreate ();
    pRfDev->intrSemId		= semCreate ();
    pRfDev->pDiskGeom		= &geometry[drive];
    if ((pRfDev->pPB = (PARMBLK *) rfMalloc (sizeof (PARMBLK))) == NULL)
	{
	printf ("rfDevSetup: can't malloc paramter block for device\n");
	rfFree ((char *) pRfDev);
	return (ERROR);
	}

    semGive (pRfDev->devSemId);			/* let the device be used */

    pRfDev->pDiskGeom->nFiles = RT_FILES_FOR_2_BLOCK_SEG; 

#ifdef DEL
    printf ("rfDevSetup: bytesec: %x, sectrk: %x ntrakcks: %x nfiles: %x\n",
	pRfDev->pDiskGeom->bytsec, pRfDev->pDiskGeom->sectrk,
	nTracks * pRfDev->pDiskGeom->sectrk, pRfDev->pDiskGeom->nFiles);
#endif DEL
				/* link the device to the rt11 file system */
    if (rt11DevInit ((RT_VOL_DESC *) pRfDev, 
	pRfDev->pDiskGeom->bytsec,
	pRfDev->pDiskGeom->sectrk,
	nTracks * pRfDev->pDiskGeom->sectrk,
	rtFmt,
	pRfDev->pDiskGeom->nFiles,
	rfSecRd, rfSecWrt, rfReset) != OK)
        {
        rfFree ((char *) pRfDev);
        printf ("rfDevsSetup: rt11DevInit failed.\n");
        return (ERROR);
        }
         
				    /* add the device to the IO system */
    if (iosDevAdd ((DEV_HDR *) pRfDev, name, rfDrvNum) != OK)
        {
        rfFree ((char *) pRfDev);
        printf ("rfDevSetup: iosDevAdd failed.\n");
        return (ERROR);
        }
 
			/* Insert the device in the drive's device chain */
    pRfDev->next = pDrive->devs;
    pDrive->devs = pRfDev;  
    return (OK); 
    }

/******************************************************************************
*
* rfDevShow -
*
* print a list of devices on the label of a rimfire drive.
*
* REUTRNS: nothing.
*/  
STATUS rfDevShow (ctlr, drive)
    int ctlr;			/* controller number */
    int drive;			/* drive number */

    {
    RF_LABEL *pLabel;
    PART_MAP *pMap;
    int i = 0;


    if ((pLabel = (RF_LABEL *) rfMalloc (sizeof (RF_LABEL)) ) == NULL)
	{
	printf ("rfDevShow: could not allocate label mem.\n");
	return (ERROR);
	}

    if (rfLabelRead (ctlr, drive, pLabel) != OK)
	{
	printf ("rfDevShow: could not read disk label\n");
	rfFree ((char *) pLabel);
	return (ERROR);
	}

    pMap = pLabel->map;

	printf("pNo name drv fmt ofst len type\n");
    for (i = 0 ; i < NDKMAP ; i++, pMap++)
	{
	if (strcmp (pMap->name, NULL) != NULL)
	    printf(" %x  %s   %x   %x   %x   %x   %x\n", 
		pMap->partNo, pMap->name,pMap->drive,
		pMap->rtFmt, pMap->trkOffset, pMap->nTracks, 
		pMap->devType);
	}
    return (OK);
    }
/*---------------------------------------------------------------------------
 *-------------------- deivce  semaphore functions -------------------------*/

/*****************************************************************************
*
* rfDevGive -
*
* release a rimfire drive. 
*
* RETURNS: nothing
*
*/  
LOCAL VOID rfDevGive(pRfDev)
    FAST RF_DEV *pRfDev; 			/* device descriptor */

    {
    semGive (pRfDev->devSemId);
    }

/*****************************************************************************
*
* rfDevTake -
*
* take control of a rimfire drive.	 
*
* RETURNS: nothing
*
*/  
LOCAL VOID rfDevTake (pRfDev, reset)  
    FAST RF_DEV *pRfDev;		/* device descriptor */
    BOOL reset;				/* reset state */

    {
    semTake (pRfDev->devSemId, WAIT_FOREVER);
    }

/******************************************************************************
*
* rfDriveDescriptorGet -
*
* returns a pointer to the drive descriptor of the given drive.
* if the drive descriptor has not been initialized for the given
* drive, space is allocated and the descriptor initialized.
*
* RETURNS: pointer to drive descriptor 
*          or NULL if error.
*/ 
LOCAL DRIVE *rfDriveDescriptorGet (ctlr, drive)
    int ctlr;					/* ctlr number */
    int drive;					/* drive number */

    {
    FAST DRIVE *pDrive = rfCtlr[ctlr].pDrives[drive];

    /* if this is the first device on this drive, allocate the drive desc. */ 
    if ( pDrive == NULL)
	{
	if ((rfCtlr[ctlr].pDrives[drive] = pDrive = 
		(DRIVE*) rfMalloc (sizeof (DRIVE)) ) == NULL)
		{
		printErr ("rfDriveDesc: can't malloc drive descriptor.\n");
		return ((DRIVE *) NULL);
		}
	pDrive->drive 	  = drive;
	pDrive->activeDev = NULL;
	pDrive->devs 	  = NULL;
	pDrive->wdog 	  = wdCreate ();
	pDrive->semId	  = semCreate (); 	/* m.e. sem for drive */
	semGive (pDrive->semId);		/* let drive be used */
	pDrive->intrSemId = semCreate();	/* task sync sem for drive */
	/* pDrive->devType   = devType;		/* type of drive */
	pDrive->ioCtlState = FALSE;		/* init cond. */
	} 
    return (pDrive);
    }

/*------------------------------------------------------------------------
 *----------------- IO system interface functions ------------------------*/

/*************************************************************************
*
* rfClose - 
* 
* close a file on a rimfire drive.
*
* RETURNS: 
*    value returned by rt11Close.
*/

LOCAL int rfClose (pFd)
    RT_FILE_DESC *pFd;

    {
    return ( rt11Close (pFd));
    }

/************************************************************************
* 
* rfCreate -
*
* create a file on a rimfire device.
*
* RETURNS:
*    pointer to a file descriptor as int, or
*    ERROR if rt11Create returned NULL.
*/
LOCAL int rfCreate (pRfDev, name, mode)
    FAST RF_DEV *pRfDev;		/* pointer to rimfire device desc. */
    char *name;				/* name of file to create */
    int  mode;				/* access mode */

    {
    FAST RT_FILE_DESC *pFd;

#ifdef DSKBUG
    printf ("rfCreate: drive: %x ctlr: %x file: %s mode: %x \n",
	pRfDev->devInfo.drive, pRfDev->ctlr, name, mode );
#endif DSKBUG

    pFd = rt11Create ((RT_VOL_DESC *) pRfDev, name, mode);
    if (pFd == NULL)
	return (ERROR);

    return ((int) pFd);
    }

/**************************************************************************
*
* rfDelete - 
*
* function to delete a file on a rimfire device.
*
* RETURNS: 
*    value returned by rt11Delete.
*
*/
LOCAL STATUS rfDelete (pRfDev, name)
    FAST RF_DEV *pRfDev;	/* device descriptor */
    char *name;			/* file name */

    {
    return (rt11Delete ((RT_VOL_DESC *) pRfDev, name));
    }

/*************************************************************************
*
* rfOpen -
*
* open a file on a rimfire device.
*
* RETURNS: 
*	pointer of type RF_FILE_DESC as int or NULL if 
*	rt11Open failed.
*/
LOCAL int rfOpen (pRfDev, name, mode)
    FAST RF_DEV *pRfDev;	/* device descriptor */
    char   *name;		/* file name */
    int    mode;  		/* r/w mode */

    {
    FAST RT_FILE_DESC *pfd = rt11Open((RT_VOL_DESC*)pRfDev, name, mode);
    if (pfd == NULL)
	{
	printf(" rfOpen: could not open file %s.\n", name);
	return (ERROR);
	}
    return ((int)pfd);
    }

/******************************************************************************
*
* rfRead -
*
* Handle unbuffered reads from the disk.
*
* RETURNS: 
*    value returned by rt11Read.
*/
LOCAL int rfRead (pfd, buffer, nbytes)
    RT_FILE_DESC *pfd;		/* rt11 file descriptor */
    char *buffer;		/* buffer to read into */
    int nbytes;			/* number of bytes to read */

    {
    return (rt11Read (pfd, buffer, nbytes ));
    }

/******************************************************************************
*
* rfWrite -
*
* function to write data to a rimfire device.
*
* RETURNS: 
*    value returned from the call to rt11Write.
*/

LOCAL int rfWrite (pfd, buffer, nbytes)
    RT_FILE_DESC *pfd;				/* rt11 file descriptor */
    char 	*buffer;			/* buffer to write to disk */
    int 	nbytes;				/* number of bytes to read */

    {
    return (rt11Write (pfd, buffer, nbytes));
    }

/*----------------------------------------------------------------------------
 *-------------------- file system interface functions ---------------------*/

/*****************************************************************************
*
* rfReset -
* 
* reset a rimfire controller.
* XXX not quite sure what reset needs to do but it is called from 
* XXX the IO system, and needs to return OK for anything to work.
*/
LOCAL STATUS rfReset ()

   {
   return (OK);
   }

/******************************************************************************
*
* rfSecRd - 
*
* read a sector from a rimfire device.
*
* RETURNS: 
*    OK | ERROR if rfLocalToBusAdrs fails.
*/

LOCAL STATUS rfSecRd (pRfDev, sector, buffer)
    FAST RF_DEV *pRfDev;			/* device descriptor */
    FAST UINT 	sector;				/* sector offset in device */
    FAST char   *buffer;			/* destination buffer */

    {
    FAST int pSec;				/* physical sector on drive */
    FAST PARMBLK *pPb;				/* command parameter block */

    rfDevTake (pRfDev, FALSE);			/* block other users */ 

    pSec = sector + (pRfDev->devInfo.trkOffset * pRfDev->pDiskGeom->sectrk);

    pPb 		= pRfDev->pPB;		/* device parameter block */
    pPb->id 		= (UINT) pRfDev; 	/* cookie points to dev */
    pPb->addrmod 	= VME_ADD_MOD;
    pPb->unit 		= pRfDev->devInfo.drive;
    pPb->control	= 0;			/* XXX use IOCG here */
    pPb->command	= C_READ;
    pPb->diskaddr 	= pSec;
    if ((pPb->vmeaddr = rfLocalToBusAdrs(buffer)) == NULL)
	{
	return (ERROR);
	}
    pPb->blkcount  = 1;
    pPb->reserved  = 0;

    rfIoctlStateSet (FALSE, pRfDev);

    rfPBPut (pPb, pRfDev);		/* put PB in Cmd List */

    semTake (pRfDev->intrSemId, WAIT_FOREVER);

    rfDevGive (pRfDev);			/* allow access to device */
    return (OK);
    }

/****************************************************************************
*
* rfSecWrt -
*
* write a sector to a rimfire drive.
*
* REUTRNS:
*   OK | ERROR
*/
LOCAL int rfSecWrt (pRfDev, sector, buffer)
    FAST RF_DEV *pRfDev;		/* device descriptor */
    FAST UINT 	sector;			/* sector offset in device */
    FAST char   *buffer;		/* source buffer */

    {
    FAST int 	 pSec;			/* physical sector on disk */
    FAST PARMBLK *pPb;			/* command parameter block */

    rfDevTake (pRfDev, FALSE);   	/* block out others */ 

    pSec = sector + (pRfDev->devInfo.trkOffset * pRfDev->pDiskGeom->sectrk);

    pPb 		= pRfDev->pPB;
    pPb->id 		= (UINT) pRfDev;	
    pPb->addrmod  	= VME_ADD_MOD;
    pPb->unit 		= pRfDev->devInfo.drive;
    pPb->control 	= 0;			/* XXX use IOCG here */
    pPb->command 	= C_WRITE;
    pPb->diskaddr 	= pSec;
    if ((pPb->vmeaddr = rfLocalToBusAdrs(buffer)) == NULL)
	{
	return (ERROR);
	}
    pPb->blkcount	= 1;		/* read one sector */
    pPb->reserved  	= 0;

    rfIoctlStateSet (FALSE, pRfDev);

    rfPBPut (pPb, pRfDev);    

    semTake (pRfDev->intrSemId, WAIT_FOREVER);
    rfDevGive (pRfDev); 		/* allow acess to device */
    return (OK);
    }

/*---------------------------------------------------------------------------
--------------------------- interrupt service ------------------------------*/

/****************************************************************************
*
* rfIntr -
* 
* Interrupt service routine when ctlr finishes a command 
*
* XXX if this driver is going to handle multiple controllers,
* then this ISR is going to need a mechanism by which to find
* out from which controller this interrupt was genrated.
* (get it from the cookie).
*/

LOCAL void rfIntr ()
    {

    FAST RF_DEV *pRfDev;	/* pointer to device descriptor */
    FAST STATBLK *pSB;		/* pointer to status block */
    FAST ctlr = 0;		/* XXX assuming 1 controller for now */
    FAST IOCTL_INFO *pIoctlInfo;
    STATBLK *rfSBGet();
    FAST status;

    while ((pSB = rfSBGet (&rfCtlr[ctlr])) != NULL)
	{
	status = pSB->status;
#ifdef DEL
#ifdef DSKBUG
	logMsg ("status %x err %x id %x data %x\n", status,
	    pSB->errcode, pSB->id, pSB->errdata);
	logMsg ("rfIntr: id %x, drive %x\n", 
	    pSB->id, ((RF_DEV*)pSB->id)->drive); 
#endif DSKBUG
#endif DEL
				/* check for general board error */
	if (pSB->id == (unsigned)0xffffffff)
	    {
	    rfErr (pSB);
	    continue;
	    }
	else  	
	    {
	    pRfDev = (RF_DEV *)pSB->id;	/* get device for pSB */
	    }
				/* What does this status block say? */
	if (status & ST_RTY)
	    {
	    logMsg ("rfIntr: retry required. \n"); 
	    }
	if (status & ST_COR)
	    {
	    logMsg ("rfIntr: correction required. \n"); 
	    }
			/* check for cmd complete or xfer complete */

	if (status & ST_CC)
	    {				/* command completed */
					/* check for errors */
	    if (status & ST_ERR)
		{
		logMsg ("rfIntr: inside ST_ERR\n");
		if (rfIoctlStateGet(pRfDev) == TRUE)
		    {
		    pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
		    pIoctlInfo->error = pSB->errcode;	/* code */
		    pIoctlInfo->data  = pSB->errdata;	/* code specific */
		    pIoctlInfo->resid = pSB->errextra;	/* etc. */
		    semGive (pRfDev->intrSemId); 
		    return;
		    }
		else
		    {
		    logMsg ("rfintr: Hard error. sector: \n"); 
		    rfErr (pSB);
		    }
		}
	    }
	}  		/* while there are status blocks to process */
    semGive (pRfDev->intrSemId); 
    }

/*---------------------------------------------------------------------------
 *------------------------- label functions  -------------------------------*/

/****************************************************************************
*
* rfLabelCreate -
*
* create a disk label and write it to the disk
*
* RETURNS: 
*    value returned by rfLabelWrite, or
*    ERROR if memory for label structure could not be 
*    allocated.
*/

STATUS rfLabelCreate (ctlr, drive, devType )
    int ctlr;			/* controller number */
    int drive;		 	/* drive number */
    int devType;		/* used to reference labels from
				 * smdLabels structure (see rflabel.h */
    {
    FAST RF_LABEL *pLabel;
    FAST DRIVE    *pDrive = rfDriveDescriptorGet(ctlr, drive);
    FAST RF_DEV   *pRfDev;
    FAST int 	  i;

    if (pDrive == NULL)
	{
	printf ("rfLabelCreate: no drive descriptor\n");
	return (ERROR);
	}

    if ((pLabel = (RF_LABEL *) rfMalloc (sizeof (RF_LABEL)) ) == NULL)
	{
	printf ("rfLabelCreate: could not allocate label mem.\n");
	return (ERROR);
	}

    *pLabel = smdLabel[devType];	/* use default geom */
    pRfDev = pDrive->devs;		/* start of device chain */

    for (i = 0; i < NDKMAP ; i++)	/* copy in existing devs */
	{
	if (pRfDev != NULL)
	    {
	    pLabel->map[pRfDev->devInfo.partNo] = pRfDev->devInfo;
	    pRfDev = pRfDev->next;
	    }
	else
	    break;
	}
    rfFree ((char *) pLabel);
    return (rfLabelWrite (ctlr, drive, pLabel));
    }


/*****************************************************************************
*
* rfLabelPartMapUpdate -
*
* function to update the current label on the disk with 
* the info in the partition map
*
* RETURNS: 
*    ERROR if label can't be read, or
*    value returned by rfLabelWrite
*/  
LOCAL STATUS rfLabelPartMapUpdate (pDrive)
    FAST DRIVE *pDrive;			/* drive descriptor */

    {
    FAST RF_LABEL *pLabel;		
    FAST RF_DEV *pRfDevBase = pDrive->devs; /* start of drives device chain */
    FAST RF_DEV *pRfDev = pRfDevBase; 		/* pointer to play with */
    FAST int i;

    if ((pLabel = (RF_LABEL *) rfMalloc (sizeof (RF_LABEL)) ) == NULL)
	{
	printf ("rfLPMU: could not allocate label mem.\n");
	return (ERROR);
	}

    if (rfLabelRead (pRfDev->ctlr, pRfDev->devInfo.drive, pLabel) != OK)
	{
	printf ("rfULPM: could not read label, use rfLabelCreate\n");
	rfFree ((char *) pLabel);
	return (ERROR);
	}

			/* copy partition map for drive into label structure */
    for (i = 0; i < NDKMAP ; i++)
	{
	if (pRfDev != NULL)
	    {
	    printf ("LPMU: partNo: %x pRfDev: %x\n", pRfDev->devInfo.partNo,
	    pRfDev);
	    pLabel->map[pRfDev->devInfo.partNo] = pRfDev->devInfo;
	    pRfDev = pRfDev->next;
	    }
	else
	    break;
	}

    if (rfLabelWrite (pRfDevBase->ctlr, pRfDevBase->devInfo.drive, 
	pLabel) != OK)
	{
	printf ("rfLPMU: error writing label\n");
	rfFree ((char *) pLabel);
	return (ERROR);
	}
    rfFree ((char *) pLabel);
    return (OK);
    }

/******************************************************************************
*
* rfLabelPrint - 
*
* print the disk label to stdout.
*
* RETURNS: 
*    nothing
*/  
VOID rfLabelPrint (pLabel)
    FAST RF_LABEL *pLabel;		/* pointer to label structure */

    {
    FAST int i;
    printf("DISK LABEL:\n");
    printf("	geom.trkdisk: %d\n", pLabel->geom.trkdisk);
    printf("	geom.sectrk ; %d\n", pLabel->geom.sectrk);
    printf("	geom.bytsec : %d\n", pLabel->geom.bytsec);
    printf("	geom.nhead  : %d\n", pLabel->geom.nhead);
    printf("	geom.n      : %d\n", pLabel->geom.n);
    printf("	geom.gplfmt : %d\n", pLabel->geom.gplfmt);
    printf("	geom.dtl    : %d\n", pLabel->geom.dtl);
    printf("	geom.nFiles : %d\n", pLabel->geom.nFiles);
    printf("	geom.ncyl   : %d\n", pLabel->geom.ncyl);
    printf("	geom.acyl   : %d\n", pLabel->geom.acyl);
    printf("	geom.bcyl   : %d\n", pLabel->geom.bcyl);
    printf("	geom.bhead  : %d\n", pLabel->geom.bhead);
    printf("	geom.intrlv : %d\n", pLabel->geom.intrlv);
    printf("	geom.gap1   : %d\n", pLabel->geom.gap1);
    printf("	geom.gap2   : %d\n", pLabel->geom.gap2);
    printf("	geom.apc    : %d\n", pLabel->geom.apc);
    printf("	geom.ppart  : %d\n", pLabel->geom.ppart);

    for (i = 0; i < NDKMAP; i ++)
	{
	printf("	map[%d].name    : %s\n", i,pLabel->map[i].name);
	printf("	map[%d].drive   : %d\n", i,pLabel->map[i].drive);
	printf("	map[%d].rtFmt   : %d\n", i,pLabel->map[i].rtFmt);
	printf("	map[%d].trkOfst : %d\n", i,pLabel->map[i].trkOffset);
	printf("	map[%d].nTracks : %d\n", i,pLabel->map[i].nTracks);
	printf("	map[%d].devType : %d\n", i,pLabel->map[i].devType);
	}
    }

/******************************************************************************
*
* rfLabelRead -
*
* read the disk label. 
* 
* RETURNS: 
*    OK, or ERROR if read fails, checksum fails or no label on drive.
*/  
STATUS rfLabelRead (ctlr, drive, pLabel)
    int 	ctlr;				/* controller number */
    int 	drive;				/* drive number */
    RF_LABEL 	*pLabel;			/* desination of label */

    {
    FAST int 	 i; 
    FAST EXTPB	 *pXPB;
    FAST int 	 cksum    = 0;
    FAST USHORT  *wp      = (USHORT *)pLabel;
    FAST RF_CTLR *pRfCtlr = &rfCtlr[ctlr];


    if ((pXPB = (EXTPB *) rfMalloc (sizeof (EXTPB))) == NULL)
	{
	printf ("rfLabelRead: can't malloc XPB\n");
	return (ERROR);
	}

    rfXPBClear (pXPB);
    /* ------------------------------ Try to read label from the drive */ 

    /* set up a read command for the first sector , this is the
    *  label of the drive */
    pXPB->pb.id 		= 0;
    pXPB->pb.command 	= C_READ;	/* read command */
    pXPB->pb.control 	= 0;
    pXPB->pb.unit 	= drive;	/* assume now only for drive*/ 
    pXPB->pb.addrmod 	= VME_ADD_MOD;
    pXPB->pb.diskaddr	= 0;
    if ((pXPB->pb.vmeaddr = rfLocalToBusAdrs(pLabel)) == NULL)
	return (ERROR);			   	
    pXPB->pb.blkcount	= 1;
    pXPB->pb.reserved	= 0;

    if (rfSglCmd (pXPB, pRfCtlr ))
	{
	/* the read failed, the drive is probably not formatted */
	printf ("rfLabelRead: disk 0x%x may not exist or be formatted.\n",
		drive);
	return (ERROR);
	}

    /* read worked, see if there is a valid label on this sector */
    /* Compute what the checksum should be... */

    for (i = 0; i < sizeof (RF_LABEL) / 2 - 1; i++)
	cksum ^= *wp++;

    if (cksum != pLabel->cksum || pLabel->magic != DKL_MAGIC)
	{
	if (cksum != pLabel->cksum)
	    {
	    printf ("ReadLabel: Bad checksum in label on unit %d\n", drive);
	    return (ERROR);
	    }
	else
	    {
	    printf ("ReadLabel: No label on unit %d\n", drive); 
	    return (ERROR);
	    }
	}
    return (OK);
    }

/************************************************************************
*
* rfLabelWrite -
*
*  write a label to the first sector of a disk.
*  
* RETURNS:  OK | ERROR
*/

STATUS rfLabelWrite(ctlr, drive, pLabel)
    int ctlr;				/* contoller number */
    int drive;				/* drive number */
    RF_LABEL *pLabel;			/* pointer to label to be written*/ 

    {
    FAST RF_CTLR *pRfCtlr = &rfCtlr[ctlr];	
    FAST USHORT *wp;	
    FAST int 	i;		
    FAST EXTPB  *pXPB; 

    if ((pXPB = (EXTPB *) rfMalloc (sizeof (EXTPB))) == NULL )
	{
	printf ("rfLabelWrite: can't malloc XPB\n");
	return (ERROR);
	}
				    /* calculate the checksum of the label */
    pLabel->magic = DKL_MAGIC;
    pLabel->cksum = 0;
    for (i = 0, wp = (USHORT *)pLabel; i < sizeof(RF_LABEL)/2-1; i++)
	pLabel->cksum ^= *wp++;

    pXPB->errintr 	= 0;
    pXPB->intr 		= 0;
    pXPB->resv 		= 0;
    pXPB->pb.id 	= 0;
    pXPB->pb.addrmod 	= VME_ADD_MOD;
    pXPB->pb.unit 	= drive;	/* drive to be written */
    pXPB->pb.control 	= 0;
    pXPB->pb.command 	= C_WRITE;
    pXPB->pb.diskaddr 	= 0;	/* label starts at beginning of disk */
    if ((pXPB->pb.vmeaddr = rfLocalToBusAdrs (pLabel)) == NULL)
	return (ERROR);		/* error handling needed here */
    pXPB->pb.blkcount 	= 1;	/* label is 1 sector big */
    pXPB->pb.reserved 	= 0;

    if (rfSglCmd (pXPB, pRfCtlr))
	{
	printf ("rfLabelWrite: failed. \n");
	return (ERROR);
	}
     return (OK);
     }

/*---------------------------------------------------------------------------
 *------------------- logical partition functions --------------------------*/

/******************************************************************************
*
* rfPartitionNumGet -
*
* searches through the given drive's device chain for a NULL
* device pointer, and thus a free logical partition number.
*
* RETURNS: 
*    partition number , or
*    ERROR (as int)if no more partitions available.
*/
LOCAL int rfPartitionNumGet (pDrive)
    DRIVE *pDrive;				/* drive descriptor */

    {
    RF_DEV *pRfDev = pDrive->devs;		/* get start of device chain */
    int    partNum;

    for (partNum = 0; partNum < NDKMAP; partNum++)
	{
	if (pRfDev != NULL)
	    pRfDev = pRfDev->next;
	else
	    return (partNum);
	}
    return ((int) ERROR);
    }

/*----------------------------------------------------------------------------
 *----------------- controller interface functions --------------------------*/

/*****************************************************************************
*
* rfPBPut -
*
* Place a parameter block in the command list and notify the controller.
*
* RETURNS: 
*    nothing.
*/

LOCAL VOID rfPBPut (pPB, pRfDev)
    FAST PARMBLK *pPB;			/* the parameter block to send */
    FAST RF_DEV  *pRfDev;		/* device descriptor */

    {
    FAST CMDLIST *pCmdList = rfCtlr[pRfDev->ctlr].pCmdList;

    rfCtlrTake (pRfDev->ctlr);

#ifndef NPBMASK
    while ((pCmdList->pbin + 1) % RF32NPB == pCmdList->pbout)
#else NPBMASK
    while (((pCmdList->pbin + 1) & NPBMASK) == pCmdList->pbout)
#endif NPBMASK
	{
	/* the list is full - sleep until a status block returns.  */
	rfCtlrGive (pRfDev->ctlr);
	taskDelay (sysClkRateGet ());
	rfCtlrTake (pRfDev->ctlr);
	}		
					/* copy the given PB to the list */
    pCmdList->pblist[pCmdList->pbin] = *pPB;

#ifndef NPBMASK
    pCmdList->pbin = (pCmdList->pbin+ 1) % RF32NPB;
#else NPBMASK
    pCmdList->pbin = (pCmdList->pbin + 1) & NPBMASK;
#endif NPBMASK

					    /* controller ACHTUNG ! */
    ((RF32REG *) rfCtlr[pRfDev->ctlr].localAdrs)->attention = SWAB(HOST_ID);

    rfCtlrGive (pRfDev->ctlr);
    }

/*****************************************************************************
*
* rfSBGet -
*
* Copy the status block from the command list and return a
* pointer to it.  If there is no status block return NULL.
*
* RETURNS: 
*    pointer to status block, or 
*    NULL if none available.
*/
LOCAL STATBLK *rfSBGet (pRfCtlr)
    FAST RF_CTLR *pRfCtlr;		/* controller descriptor */

    {
    STATBLK *pSB;			/* pointer to status block */
    FAST CMDLIST *pCmdList = pRfCtlr->pCmdList;

    if (pCmdList->sbin >= RF32NSB)
	{
	/* XXX see john about using sprintDrv here since called from
	 * XXX int lvl. 
	 * printf ("panic - rfSBGet: sbin pointer out of range!\n"); 
	 * d (&pCmdList->sblist[pCmdList->sbout], 2); */
	logMsg ("rfSBGet: sbin pointer out of range\n");
	}

    if (pCmdList->sbout >= RF32NSB)
	{
	/* printf (" panic - rfSBGet: sbout pointer out of range!\n");
	/* d (&pCmdList->sblist[pCmdList->sbout], 2); */
	logMsg ("rfSBGet: sbin pointer out of range\n");
	}

    if (pCmdList->sbout == pCmdList->sbin)
	{
	/* printf ("rfSBGet: no statblk \n"); */
	return ((STATBLK *)NULL);		/* list empty */
	}

				/* get pointer to this status block */
    pSB = &pCmdList->sblist [pCmdList->sbout];
    pCmdList->sbout++;

#ifndef NSBMASK
    pCmdList->sbout %= RF32NSB;
#else NSBMASK
    pCmdList->sbout &= NSBMASK;
#endif NSBMASK
    return (pSB);
    }

/****************************************************************************
*
* rfSglCmd -
*
* A type 0 command is sent to the RF 3200.  The address of the
* extended parameter block is written to the address buffer port
* and a 0 is written to the channel attention port.  The status
* block is then polled until the command completes.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS rfSglCmd (xpb, pRfCtlr)
    FAST EXTPB *xpb; 		/* extended RF 3200 parameter block */
    RF_CTLR *pRfCtlr; 		/* controller descriptor */

    {
    FAST RF32REG *rf = (RF32REG *) pRfCtlr->localAdrs; 
    FAST 	int waitfor;
    dword 	pb;		/* address of the extended parameter block */
    word  	lsb, msb;
    int 	finished;	/* save completion status	*/

					    /* Clear the status block */
    xpb->sb.id 		= 0;
    xpb->sb.status 	= 0;
    xpb->sb.errcode 	= 0;
    xpb->sb.errextra 	= 0;
    xpb->sb.errdata 	= 0;

    /* Wait until we can write to address buffer port */
    /* this should test toggling of the busy bit */
    
    waitfor = SWAB(pRfCtlr->bsybit);

#ifdef TOOLONG
    CDELAY((finished = (rf->status & waitfor) == waitfor), TOOLONG);
    if (!finished)
	{
	printf ("rfSglCmd: timeout ... BSY\n");
	printf ("rfSglCmd: status port = %x\n", rf->status);
	rfErr (&xpb->sb);	/* write a status block */
	return (1);
	}

#else TOOLONG
    while ((rf->status & waitfor) != waitfor)
	;
#endif TOOLONG

    rf->addrbuf = SWAB(CNTRL << 8 | VME_ADD_MOD);

#ifdef CMDBUG
    printf ("rfSglCmd: status port = %x\n", rf->status);
#endif CMDBUG

    if ((pb = rfLocalToBusAdrs(xpb)) == NULL)
	;				/* XXX need error handling here */

    msb = SWAB(pb >> 16 & 0xffff);
    lsb = SWAB(pb & 0xffff);

#ifdef CMDBUG
    printf ("rfSglCmd: pb=%x msb=%x lsb=%x\n", pb, msb, lsb);
#endif CMDBUG

    rf->addrbuf = msb;
    rf->addrbuf = lsb;

    rf->attention = 0;			/* controller attention */

    /* Toggle our busy bit...as the controller should do when it
     * finishes the command. */

    pRfCtlr->bsybit ^= 1;

    /* Wait for the command to complete */
#ifdef CMDBUG
	printf ("rfSglCmd: Waiting for ST_CC\n");
	printf ("rfSglCmd: status port = %x\n", rf->status);
#endif CMDBUG

#ifdef TOOLONG
    CDELAY((xpb->sb.status & ST_CC),TOOLONG);
    if ((xpb->sb.status & ST_CC) == 0)
	{
	printf ("rfSglCmd: timeout ... no ST_CC\n");
	printf ("rfSglCmd: command = %x, status port = %x\n",
	xpb->pb.command, rf->status);
	rfErr (&xpb->sb);
	return (1);
	}
#else TOOLONG
    while ((xpb->sb.status & ST_CC) == 0)
	;
#endif TOOLONG

    if (xpb->sb.status & ST_ERR)		/* errors ? */
	{
	printf ("rfSglCmd: ST_ERR!\n");
	printf ("rfSglCmd: command = %x, status port = %x\n",
	xpb->pb.command, rf->status);
	rfErr(&xpb->sb);		/* on error dump status block */
	return(1);
	}

#ifdef CMDBUG
    printf ("rfSglCmd: return\n");
#endif CMDBUG

    return (0);				/* no error return */	
    }

/****************************************************************************
*
* rfErr -
*
* Displays the status block which caused an error.
*
* RETURNS:
*    nothing.
*/
LOCAL VOID rfErr (sb)
    STATBLK *sb; 		/* pointer to status block from controller */

    {
    /* When the status block id is null, we are booting or dumping core.
    * Otherwise it will always be set to a buffer header. */

    printf ("rfErr: id=%x, status=%x, error=%x, extra=%x %x\n",
	    sb->id, sb->status, sb->errcode, sb->errextra, sb->errdata);
    }

/******************************************************************************
*
* rfLocalToBusAdrs -
*
* change the given local address to a VME bus address
* 
* RETURNS: VME bus address or NULL if rfLocalToBusAdrs returned 
* 	an error.
*
*/ 
LOCAL long rfLocalToBusAdrs (localAdrs)
    char *localAdrs;		/* local address to convert */

    {
    char *pBusAdrs;
    if (sysLocalToBusAdrs(VME_AM_STD_SUP_DATA , localAdrs, &pBusAdrs) != OK)
	{
	printf("sysLocalToBusAdrs: unable to get to local adrs from bus\n");
	return((long) localAdrs);
	}
    return((long)pBusAdrs);
    }

/******************************************************************************
*
* rfXPBClear -
*
* zero out an extended parameter block
*
* RETURNS: OK | ERROR
*/  
STATUS rfXPBClear (pXPB)
    EXTPB *pXPB;				/* pointer to EXTPB to Clear */

    {
    return (bzero (pXPB, sizeof(EXTPB)));
    }


/*---------------------------------------------------------------------------
 *-------------------- ioctl and related  functions ------------------------*/

/****************************************************************************
*
* rfIoctl -
*
* function to allow I/O commands for the Rim Fire controller
* to be executed via the file system.
*
* RETURNS: value returned by call to subfunction.
*/

LOCAL int rfIoctl (pFd, cmd, arg)
    RT_FILE_DESC *pFd;				/* rt11 file descriptor */
    int cmd;					/* command to perform */
    int arg;					/* argument to pass */

    {
    RF_DEV *pRfDev = (RF_DEV *) pFd->rfd_vdptr;

    /* only allow one ioctl at a time */
    /* XXX   yo! what is happening here looks like int priority scheme*/
    /* s = splx (pritospl (device->md_mc->mc_intpri)); */
/* XXX put in rfDevTake here */
#ifdef DEL 
    printf ("rfIoctl: cmd: %x\n", cmd);
#endif DEL


    switch (cmd)
	{
	case FIODISKFORMAT:		/* Format track(s) */
	    printf ("rfIoctl: skipping format for now\n");
	    return (OK);
	    /* return (format (unit, (struct dk_fmt *)addr, &ioctlbuf)); */

	case FIODISKINIT:
	    return (rt11Ioctl (pFd, cmd, arg ));

	case FIODIRENTRY:
	    return (rt11Ioctl (pFd, cmd, arg ));

	case FIOCONFIG:		
	    return (rfConfig (pRfDev->ctlr, pRfDev->devInfo.drive, 
			(DISK_GEOM *)arg));

	case FIOCONFIGGET:		
	    return (rfConfigGet (pRfDev->ctlr, pRfDev->devInfo.drive, 
			(CONFIG_GET_REQ *)arg));

	case FIODEFECTDATAREAD:
	    return (rfDefectDataRead (pRfDev, (RDDEFDATA_REQ *)arg));

	case FIOIDGET:
	    return (rfIDGet (pRfDev, (RETID *)arg));

	case FIOCTLGRPSET:
	    return (rfIoctlGrpSet (pRfDev, (IOCGRP *)arg));

	case FIOINTERROGATE:
	    return (rfInterrogate (pRfDev, (SMDCHRS *)arg));

	case FIOSECIDREAD:
	    return (rfSecIDRead (pRfDev, (READSECIDREQ *)arg));

	case FIOSECTORSLIP:
	    return (rfSecSlip (pRfDev, (SLIP_REQ *) arg));

	case FIOSMDDEFINE:
	    return (rfSMDDefine (pRfDev, (DEFSMD_REQ *)arg));

	case FIOSTATGET:
	    return (rfStatGet (pRfDev, (STAT_REQ *)arg));

	case FIOTRKSECMAP:
	    return (rfTrkSecMap (pRfDev, (MAP_REQ *)arg));

	case FIOTUNE:
	    return (rfTune (pRfDev, (TUNEST *)arg));

	case FIOVERIFY:			
	    return (rfVerify (pRfDev, (FMT_VER_REQ *)arg));

	case FIOGEOMGET:		/* return a disk's geometry */
	    *(DISK_GEOM *) arg = *(pRfDev->pDiskGeom);
	    return (OK);
#ifdef DEL
	case FIOGEOMSET:		/* set the geometry of a disk */
	    return (rfSetGeom (pRfDev, (DISK_GEOM*) arg ));
#endif DEL
	default:
	    return (rt11Ioctl (pFd, cmd, arg));
	}
    }

/******************************************************************************
*
* rfIoctlStateGet -
*
* returns the value of rfIoctlState for the given controller.
*
* RETURNS: value of rfIoctlState.
*/  
LOCAL BOOL rfIoctlStateGet (pRfDev)
    RF_DEV *pRfDev;			/* device descriptor */

    {
    return (rfCtlr[pRfDev->ctlr].pDrives[pRfDev->devInfo.drive]->ioCtlState);
    }
/******************************************************************************
*
* rfIoctlInfoClear -
*
* clear the info in the given IoctlInfo structure.
*
* RETURNS:  OK | ERROR.
*/  
LOCAL STATUS rfIoctlInfoClear (pIoctlInfo)
    IOCTL_INFO *pIoctlInfo;		/* mem to be cleared */

    {
    return (bzero (pIoctlInfo, sizeof (IOCTL_INFO)));
    }

/******************************************************************************
*
* rfIoctlStateSet -
* 
* set the state of the driver for ioctl.
* this is used to let the command complete ISR know 
* whether or not we were doing Ioctl.
*
* RETURNS: nothing.
*/ 
LOCAL VOID rfIoctlStateSet (state, pRfDev)
    BOOL 	state;				/* state to set */
    RF_DEV 	*pRfDev;			/* device descriptor */

    {
    rfCtlr[pRfDev->ctlr].pDrives[pRfDev->devInfo.drive]->ioCtlState = state;
    }

/******************************************************************************
*
* rfIoctlInfoPtrGet -
*
* function returns the pointer to the IOCTL_INFO structure
* for the given drive on the given controller.
*
* RETURNS:
*   pointer to IOCTL_INFO structure or,
*   ERROR if invalid controller or drive is specified.
*/  
IOCTL_INFO *rfIoctlInfoPtrGet (pRfDev)
    RF_DEV 	*pRfDev;			/* device descriptor */

    {
    DRIVE *pDrive = rfDriveDescriptorGet (pRfDev->ctlr, pRfDev->devInfo.drive);

    if (pDrive == NULL)
	return (IOCTL_INFO *) (ERROR);

    return (&pDrive->ioCtlInfo);
    }

/******************************************************************************
*
* rfConfig -
*
* Configure the given drive according to the specified drive
* configuration information.
*
* RETURNS: 
*   OK or ERROR
*
*/

STATUS rfConfig (ctlr, drive, pGeomReq)		
    int ctlr;					/* controller number */
    int drive;					/* drive number */
    DISK_GEOM *pGeomReq;			/* user's requested geometry */

    {
    EXTPB	*pXPB;			/* use extended parameter block */
    CONFPB	*pConfPB;
    DEFSMDPB 	*pDefSMDPB;
    RETCONF 	*pConfParams; 		/* configuration parameters */
    CONFIG_GET_REQ *pConfGetReq;	/* request for config params */
    RF_CTLR	*pRfCtlr = &rfCtlr[ctlr];

    if ((pXPB = (EXTPB *) rfMalloc (sizeof(EXTPB))) == NULL)
	{
	printf ("rfConfig: could not allocate EXTPB\n");
	return (ERROR);
	}

    rfXPBClear (pXPB);
			    /* Set up a pConfPB->guration parameter block */
    pConfPB 		= (CONFPB *)&pXPB->pb;
    pConfPB->id 	= 0;
    pConfPB->command 	= C_SETCONFIG;
    pConfPB->control 	= 0;
    pConfPB->unit 	= drive;
    pConfPB->resv1 	= 0;
    pConfPB->cyldsk 	= pGeomReq->ncyl + pGeomReq->acyl;
    pConfPB->bytsec 	= pGeomReq->bytsec;
    pConfPB->headcyl 	= pGeomReq->nhead;
    pConfPB->sectrk 	= pGeomReq->sectrk;
    pConfPB->resv2[0] 	= 0;
    pConfPB->resv2[1] 	= 0;
    pConfPB->resv2[2] 	= 0;

    if (pRfCtlr->rfctype == STATRF3200)
	{
	pConfPB->basehead = pGeomReq->bhead;
	/* XXX pConfPB->flags 	= pGeomReq->extra[XTCFF]; */
	}
    else
	{
	pConfPB->basehead = 0;		/* STATRF3400 */
	pConfPB->flags 	= 0;	
	}

#ifdef NSPARES
    pConfPB->nspares 	= pGeomReq->apc / pGeomReq->nhead;
#else
    pConfPB->nspares 	= pGeomReq->extra[0] / pGeomReq->nhead;
#endif NSPARES
    if (rfSglCmd (pXPB, pRfCtlr))
	{
	printf ("rfConfig: configure disk command failed\n");
	rfFree ((char *)pXPB);
	}

			    /* Get whatever params the drive already has. */
    if ((pConfParams = (RETCONF *) rfMalloc (sizeof (RETCONF))) == NULL)
	{
	printf ("rfConfig: could not allocate CONFPARAM space\n");
	rfFree ((char *) pXPB);
	return (ERROR);
	}

    if ((pConfGetReq = (CONFIG_GET_REQ *) rfMalloc (sizeof (CONFIG_GET_REQ))) 
	== NULL)
	{
	printf ("rfConfig: could not allocate CONFPARAM space\n");
	rfFree ((char *) pXPB);
	return (ERROR);
	}

    pConfGetReq->pConfParams = pConfParams;

    if (rfConfigGet (ctlr, drive, pConfGetReq))
	{			/* error */

#ifdef DSKBUG
	printf ("rfConfig: Couldn't get drive configuration\n");
	printf ("rfConfig: setting minimal SMD params\n");
#endif DSKBUG
	pConfParams->interleave = 1;		/* use minimal SMD params */
	pConfParams->headskew   = 3;
	pConfParams->cylskew    = 5;
	pConfParams->hgskew     = 0;
	}
			    /* Set up a define SMD parameter block */
    pDefSMDPB 		= (DEFSMDPB *)&pXPB->pb;
    pDefSMDPB->id 	= 0;
    pDefSMDPB->resv1 	= 0;
    pDefSMDPB->unit 	= drive;
    pDefSMDPB->control 	= 0;
    pDefSMDPB->command 	= C_SETSMDPARAM;
    pDefSMDPB->interleave = pConfParams->interleave;
    pDefSMDPB->headskew = pConfParams->headskew;
    pDefSMDPB->cylskew 	= pConfParams->cylskew;
    pDefSMDPB->hgskew 	= pConfParams->hgskew;
    pDefSMDPB->recovery = 0;
    pDefSMDPB->idpre 	= pGeomReq->gap1;
    pDefSMDPB->datapre 	= pGeomReq->gap2;
    pDefSMDPB->resv3 	= 0;

    if (rfSglCmd (pXPB, pRfCtlr))
	{
	printf ("rfConfig: define SMD params command failed\n");
	rfFree ((char *)pXPB);
	rfFree ((char *)pConfParams);
	return (ERROR);
	}

	/* all configuation commands worked 
	 * set geom structure for future reference 
	 */
    geometry[drive] = *pGeomReq;

    rfFree ((char *)pXPB);
    rfFree ((char *)pConfParams);
    return (OK);    
    }



/****************************************************************************
*
* rfConfigGet -
*
* gets the configuration parameters for the specified drive.
* params are left in the memory pointed to by the user's request.
*  
* RETURNS: OK | ERROR
*/

STATUS rfConfigGet (ctlr, drive, pConfGetReq)		
    int ctlr;				/* controller number */
    int drive;				/* drive number */
    CONFIG_GET_REQ *pConfGetReq;	/* return configuration request */

    {
    EXTPB   *pXPB;			/* extened PB for single command */
    PARMBLK *pPb;
    RF_CTLR *pRfCtlr = &rfCtlr[ctlr];


    if ((pXPB = (EXTPB *) rfMalloc (sizeof(EXTPB))) == NULL)
	{
	printf ("rfConfigGet: could not allocate EXTPB\n");
	return (ERROR);
	}

    rfXPBClear (pXPB);

#ifdef DSKBUG
    printf ("rfConfigGet: unit %x\n", drive);
#endif DSKBUG

    /* set up return configuration  command for controller */
    pPb 	 = &pXPB->pb;
    pPb->id 	 = 0;
    pPb->command = C_GETCONFIG;
    pPb->unit 	 = drive;
    pPb->control = 0;
    pPb->addrmod = VME_ADD_MOD;
    if ((pPb->vmeaddr = rfLocalToBusAdrs(pConfGetReq->pConfParams)) == NULL)
	/* XXX error handling needed here */ ;
    pPb->diskaddr = 0;
    pPb->blkcount = 0;
    pPb->reserved = 0;


    if (rfSglCmd (pXPB, pRfCtlr))
	{
	printf ("rfConfigGet: define SMD params command failed\n");
	rfFree ((char *)pXPB);
	return (ERROR);
	}

    return (OK);
    }


/******************************************************************************
*
* rfDefectDataRead -
*
* read defect data of a rimfire drive.
*
* RETURNS: 
*    OK or ERROR.
*/
LOCAL STATUS rfDefectDataRead (pRfDev, pRdDefDat)	/* untested */
    RF_DEV *pRfDev;
    RDDEFDATA_REQ *pRdDefDat;

    {
    IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
    DISK_GEOM *pDiskGeom   = &geometry[pRfDev->devInfo.drive];
    PARMBLK   dDatPb;	 		/* parameter block for this request */
    DEFECTSMD *pRdDB;			/* place to read in defect data */

    rfDevTake (pRfDev, FALSE);		/* block other users */ 

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */

    /* get some memory to DMA the defect data block into */
    if ((pRdDB = (DEFECTSMD *) rfMalloc (sizeof(DEFECTSMD))) == NULL)
	{
	printf ("rfDefectDataRead: Cannot malloc Defect buffer!\n");
	rfDevGive (pRfDev);
	return (ERROR);
	}

    rfIoctlStateSet (TRUE, pRfDev);
						/* Set up slip command */
    dDatPb.id 		= (UINT)pRfDev;
    dDatPb.command 	= C_READDEFECT;
    dDatPb.unit 	= pRfDev->devInfo.drive;
    dDatPb.control 	= 0;
    dDatPb.addrmod 	= VME_ADD_MOD;
    dDatPb.blkcount 	= 0;
    if ((dDatPb.vmeaddr = rfLocalToBusAdrs(&pRdDB)) == NULL)
	; /* XXX error handling needed here */ 

    dDatPb.diskaddr = (pRdDefDat->smd.cylinder *pDiskGeom->nhead +
		       pRdDefDat->smd.head) * pDiskGeom->sectrk;
    
    rfPBPut (&dDatPb, pRfDev);			/* put PB in Cmd List */

    semTake (pRfDev->intrSemId, WAIT_FOREVER);

    pRdDefDat->error = pIoctlInfo->error;  	/* give error to user */

    pRdDefDat->smd = *pRdDB;			/* give info to user */

    rfFree ((char*)pRdDB);			/* free DVMA mem. */

    rfIoctlStateSet (FALSE, pRfDev);
    rfDevGive(pRfDev); 				/* release device */
    return (pIoctlInfo->error ? ERROR : OK);    /* return error status */
    }

/******************************************************************************
*
* rfDefectMapRead -
*
* read the defect data map of the given device.
*
* RETURNS: 0 if ok or errno
*/
LOCAL STATUS rfDefectMapRead (pRfDev, pRMapReq)		/* untested */
    RF_DEV *pRfDev;
    READ_MAP_REQ *pRMapReq;		/* user's read map request */

    {
    IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
    FAST PARMBLK defMapPb; 		/* parameter block */
    FAST DEFECTMAP *pDefectMap;	 

    rfDevTake (pRfDev, FALSE);		/* block other users */ 

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */

    /* get some memory to DMA the defect map into */
    pDefectMap = (DEFECTMAP *) rfMalloc (sizeof (DEFECTMAP));

    if (pDefectMap == NULL)
	{
	printf ("defectmap: Cannot malloc Defect buffer!\n");
	return (ERROR);
	}

#ifdef DSKBUG
    printf ("defect map: unit %x\n", pRfDev->devInfo.drive);
#endif DSKBUG

    rfIoctlStateGet (pRfDev);
					    /* set up read map command */
    defMapPb.id 	= (UINT)pRfDev; 
    defMapPb.command 	= C_READMAP;
    defMapPb.unit 	= pRfDev->devInfo.drive;
    defMapPb.control 	= 0;
    defMapPb.addrmod 	= VME_ADD_MOD;
    if ((defMapPb.vmeaddr = rfLocalToBusAdrs(pDefectMap)) == NULL)
	/* XXX error handling here */ ;
    defMapPb.diskaddr = 0;
    defMapPb.blkcount = 0;
    defMapPb.reserved = 0;

    rfPBPut (&defMapPb, pRfDev);		/* put PB in Cmd List */

    semTake (pRfDev->intrSemId, WAIT_FOREVER);	/* wait for interrupt */

    if ((pRMapReq->error = pIoctlInfo->error) != NULL)
	{
	rfFree ((char*)pDefectMap);
	rfIoctlStateSet (FALSE, pRfDev);
	rfDevGive(pRfDev); 
	return (ERROR);
	}

    *pRMapReq->pDefectMap = *pDefectMap;	/* give user defect map */

    rfFree ((char*)pDefectMap);
    rfIoctlStateSet (FALSE, pRfDev);
    rfDevGive(pRfDev); 
    return (pIoctlInfo->error ? ERROR : OK);    /* return error status */
    }

/******************************************************************************
*
* rfFormat -
*
* Format the drive for the given number of tracks.
*
* RETURNS: OK | ERROR
*/
STATUS rfFormat (ctlr, drive, pFmtReq)		
    int ctlr;					/* controller number */
    int drive;					/* drive number */
    FAST FMT_VER_REQ *pFmtReq;			/* format request block */

    {
    FAST FMTVERPB *pFmtPb;
    FAST RF_CTLR  *pRfCtlr = &rfCtlr[ctlr];
    FAST EXTPB    *pXPB;	
    FAST RF32REG  *rf = (RF32REG *) pRfCtlr->localAdrs; 
    FAST dword	  pbBusAdrs;			/* local to bus addrs */
    FAST int 	  waitfor;
    FAST int  	  lsb;
    FAST int  	  msb;
    FAST int 	  finished;			/* save completion status */

#ifdef DSKBUG
    printf ("rfFormat: controller %d, drive %d\n", ctlr, drive);
#endif DSKBUG
    
    if ( geometry[drive].sectrk == 0)
	{
	printf ("rfFormat: geometry not set for drive %d\n", drive);
	return (ERROR);
	}

    if ((pXPB = (EXTPB *) rfMalloc (sizeof (EXTPB))) == NULL)
	{
	printf ("rfFormat: could not allocate param block\n");
	return (ERROR);
	}
						/* Clear the status block */
    pXPB->sb.id 	= 0;
    pXPB->sb.status 	= 0;
    pXPB->sb.errcode 	= 0;
    pXPB->sb.errextra 	= 0;
    pXPB->sb.errdata 	= 0;
						/* create a format command */
    pFmtPb 		= (FMTVERPB *) &pXPB->pb;
    pFmtPb->id 		= 0x0; 
    pFmtPb->command 	= C_FORMAT;
    pFmtPb->control 	= 0x0;
    pFmtPb->unit 	= 1; 
    pFmtPb->resv0 	= 0x0;
    pFmtPb->diskaddr 	= pFmtReq->startBlock;	/* start at block 0 */
    pFmtPb->resv1 	= 0x0;
    pFmtPb->blkcount 	= pFmtReq->blockCount;	/* go til end of disk */  

    printf ("formatting ...");

    /* Wait until we can write to address buffer port */
    /* this should test toggling of the busy bit */
    
    waitfor = SWAB(pRfCtlr->bsybit);

				/* don't wait too long for the dang thing */
    CDELAY((finished = (rf->status & waitfor) == waitfor), TOOLONG);
    if (!finished)
	{
	printf ("rfSglCmd: timeout ... BSY\n");
	printf ("rfSglCmd: status port = %x\n", rf->status);
	rfErr (&pXPB->sb);		/* write a status block */
	return (1);
	}


    rf->addrbuf = SWAB(CNTRL << 8 | VME_ADD_MOD);

#ifdef CMDBUG
    printf ("rfFormat: status port = %x\n", rf->status);
#endif CMDBUG

    if ((pbBusAdrs = rfLocalToBusAdrs(pXPB)) == NULL)
	;				/* XXX need error handling here */

    msb = SWAB(pbBusAdrs >> 16 & 0xffff);
    lsb = SWAB(pbBusAdrs & 0xffff);

#ifdef CMDBUG
    printf ("rfFormat: pbBusAdrs=%x msb=%x lsb=%x\n", pbBusAdrs, msb, lsb);
#endif CMDBUG

    rf->addrbuf = msb;
    rf->addrbuf = lsb;

    rf->attention = 0;			/* controller attention */

    /* Toggle our busy bit...as the controller should do when it
     * finishes the command. */

    pRfCtlr->bsybit ^= 1;

					/* Wait for the command to complete */
#ifdef CMDBUG
    printf ("rfFormat: Waiting for ST_CC\n");
    printf ("rfFormat: status port = %x\n", rf->status);
#endif CMDBBUG

    CDELAY((pXPB->sb.status & ST_CC), WAYTOOLONG);
    if ((pXPB->sb.status & ST_CC) == 0)
	{
	printf ("rfFormat: timeout ... no ST_CC\n");
	printf ("rfFormat: command = %x, status port = %x\n",
	pXPB->pb.command, rf->status);
	rfErr (&pXPB->sb);	/* write a status block */
	rfFree ((char *)pXPB);
	return (ERROR);
	}

    if (pXPB->sb.status & ST_ERR)		/* errors ? */
	{
	printf ("rfFormat: ST_ERR!\n");
	printf ("rfFormat: command = %x, status port = %x\n",
	    pXPB->pb.command, rf->status);
	rfErr(&pXPB->sb);			/* dump status block */
	}

    printf("rfFormat:  done\n");
    rfFree ((char *)pXPB);
    return (OK); 
    }		

/******************************************************************************
*
* rfIDGet -
*
* Process a user's return identification request.
*
* RETURNS: 0 sucess 1 error
*
* code tested OK but rimfire would not return ID, rfSglCmd times out.
*/

LOCAL STATUS rfIDGet (pRfDev, pURetID)
    FAST RF_DEV *pRfDev;		/* device descriptor */
    FAST RETID  *pURetID; 		/* the return id structure */

    {
    FAST EXTPB   *pExtPB;
    FAST PARMBLK *pb; /* parameter block for identification request */

						/* allocate the XPB */
    if ((pExtPB = (EXTPB *) rfMalloc (sizeof(EXTPB))) == NULL)
	{
	printf ("rfIDGet: can't malloc XPB\n");
	return (ERROR);
	}

    /* initialize extended parameter block - disable interrupts */
    pExtPB->intr 		= 0;
    pExtPB->errintr 		= 0;
    pExtPB->resv 		= 0;
					/* build the Identify command */
    pb 			= (PARMBLK *)&pExtPB->pb;
    pb->id 		= 0;
    pb->command 	= C_IDENTIFY;
    pb->control 	= 0;
    pb->unit 		= 0;
    pb->addrmod 	= 0;
    pb->diskaddr 	= 0;
    pb->vmeaddr 	= 0;
    pb->blkcount 	= 0;
    pb->reserved 	= 0;

    if (rfSglCmd (pExtPB, pRfDev))		/* issue the command */
	{
	printf ("retident: Cannot identify controller %d\n", pRfDev->ctlr);
	rfFree ((char*)pExtPB);
	return (ERROR);
	}

    *(STATBLK *)pURetID = pExtPB->sb;		/* give ID to user */
    rfFree ((char*)pExtPB);
    return (pURetID->error ? ERROR : OK);    	/* return error status */
    }

/****************************************************************************
*
* rfInterrogate -
*
* Issue Interrogate Drive command and return results to user
*
* RETURNS: 0 ok or error 
* smd characteristics placed in users buffer.
*/
LOCAL STATUS rfInterrogate (pRfDev, pUSMDChar) 		
    FAST RF_DEV *pRfDev; 		/* device descriptor */
    FAST SMDCHRS *pUSMDChar;		/* user's SMD characteristics buffer */

    {
    SMDCHRS *pSMDChar;			/* buffer for DVMA chars */ 
    PARMBLK pb;				/* command parameter block */
    IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */

    /* get some memory to DMA the characteristics into */
    if ((pSMDChar = (SMDCHRS *) rfMalloc (sizeof(SMDCHRS))) == NULL)
	{
	printf ("rfInterrogate: no memory for characteristics\n");
	return (ERROR);
	}

				/* set up interrogate command for controller */
    pb.id 	= (UINT)pRfDev;
    pb.command 	= C_INTERROGATE;
    pb.unit 	= pRfDev->devInfo.drive;
    pb.control 	= 0;
    pb.addrmod 	= VME_ADD_MOD;
    if ((pb.vmeaddr = rfLocalToBusAdrs(pSMDChar)) == NULL)
	/* XXX error hanling needed here */ ;
    pb.reserved = 0;
    pb.diskaddr = 0;
    pb.blkcount = 0;

    rfPBPut (&pb, pRfDev);

    semTake (pRfDev->intrSemId, WAIT_FOREVER); 		/* wait for interrupt */

    *pUSMDChar = *pSMDChar;			/* give chars to user */

    rfFree ((char*)pSMDChar);

    return(pIoctlInfo->error ? ERROR : OK);	/* return error status */
    }

/******************************************************************************
*
* rfIoctlGrpSet -
*
* Changes the I/O control group settings.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS rfIoctlGrpSet (pRfDev, pIocgrp)  
    FAST RF_DEV *pRfDev;		/* device descriptor */
    FAST IOCGRP *pIocgrp; 		/* i/o control group pointer */

    {
    FAST IOCGPB	iocgPb;
    FAST IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);

				    /* Is the group requested within bounds? */
    if (pIocgrp->group < 1 || pIocgrp->group > MAXIOCG)
	{
	printf ("rfIoctlGrpSet: bad iocgrp number: 0x%x\n", pIocgrp->group);
	return (ERROR);
	}

    rfDevTake (pRfDev, FALSE);		/* block other users */ 

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */
    rfIoctlStateSet (TRUE, pRfDev);
   						/* set up command block */ 
    iocgPb.id 		= (UINT)pRfDev;
    iocgPb.command 	= C_SETIOCG;
    iocgPb.control 	= pIocgrp->group;
					/* unit here seems to be an error,
					 * since passing in the drive number
					 * causes the controller to give
					 * error 0x03 " bad unit number " */
    iocgPb.unit 	= 0; 		/* pRfDev->devInfo.drive; */
    iocgPb.resv1 	= 0;
    iocgPb.cache 	= pIocgrp->cache;
    iocgPb.readahead 	= pIocgrp->readahead;
    iocgPb.recovery 	= pIocgrp->recovery;
    iocgPb.resv2 	= 0;
    iocgPb.dretry 	= pIocgrp->dretry;
    iocgPb.ndretry 	= pIocgrp->ndretry;
    iocgPb.resv3 	= 0;
    
    rfPBPut (&iocgPb, pRfDev);			/* queue the command */

    semTake (pRfDev->intrSemId, WAIT_FOREVER); 		/* wait for interrupt */

    if (pIoctlInfo->error)
	{
	printf ("rfIoctlGrpSet: error: 0x%x\n", pIoctlInfo->error);
	}

    rfDevGive (pRfDev);

    return(pIoctlInfo->error ? ERROR : OK);	/* return error status */
    }

/*****************************************************************************
*
* rfSecIDRead -
*
* return the ID structure of the given sector to the user's
* memory.
*
* RETURNS: 0 if ok or errno
*/

LOCAL STATUS rfSecIDRead ( pRfDev, pRSID)	
    FAST RF_DEV *pRfDev;			/* device descriptor */
    READSECIDREQ *pRSID;			/* read id request */

    {
    FAST PARMBLK pb;
    FAST IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
    FAST DISK_GEOM *pDiskGeom = &geometry[pRfDev->devInfo.drive];
    FAST long diskAddr;		/* for setting diskaddr of param block */
    TRAKID *pTrkIDs;		/* place in which to read id's of track */ 
    TRAKID *pTrkIDsTmp;		/* place in which to read id's of track */ 
    int trksize;
    int i;

    rfDevTake (pRfDev, FALSE);		/* block other users */ 

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */

#ifdef DSKBUG
    printf ("rfSecIDRead: unit %x\n", pRfDev->devInfo.drive);
#endif DSKBUG

				/* calculate the physical size of a track */
    trksize = sizeof(TRAKID) * (pDiskGeom->sectrk /* +
	( pDiskGeom->extra[XTCFF] & CONFSSP )*/);

    if ( (pTrkIDs = (TRAKID *) rfMalloc (trksize)) == NULL)
	{
	printf ("trakid: Cannot malloc Track id buffer!\n");
	return (ERROR);
	}

    if (pDiskGeom->sectrk == 0)			/* check if geom. set */
	{
	printf ("%d: drive geometry not set!\n", pRfDev->devInfo.drive);
	return (ERROR);
	}

    
    rfIoctlStateSet (TRUE, pRfDev);

    if (pRSID->cyl == -1)
	diskAddr = -1;
    else
	diskAddr = (pRSID->cyl
		    * pDiskGeom->nhead 
		    + pRSID->head)
		    * pDiskGeom->sectrk;

    pb.id 	= (UINT)pRfDev;		/* read track of id's command */
    pb.command 	= C_READTID;
    pb.unit 	= pRfDev->devInfo.drive;
    pb.control 	= 0;
    pb.addrmod 	= VME_ADD_MOD;
    if ((pb.vmeaddr = rfLocalToBusAdrs(pTrkIDs)) == NULL)
	; /* XXX error handling needed here */ 
    pb.diskaddr = diskAddr;
    pb.blkcount = 0;
    pb.reserved = 0;

    rfPBPut (&pb, pRfDev);			/* put PB in Cmd List */

    semTake (pRfDev->intrSemId, WAIT_FOREVER);	/* wait for interrupt */

		    /* copy the correct sector ID where the user wants it */
    *pRSID->pSecID = pTrkIDs[pRSID->secOffset];
    
    rfFree ((char*)pTrkIDs);  			/* free DVMA memory */

    rfIoctlStateSet (FALSE, pRfDev);
    rfDevGive(pRfDev); 
    return (pIoctlInfo->error ? ERROR : OK);    /* return error info */
    }


/******************************************************************************
*
* rfSecSlip -
*
* Process a user's sector slipping request.
*
* RETURNS: OK | ERROR
*/

LOCAL STATUS rfSecSlip (pRfDev, pSlipReq)		
    FAST RF_DEV 	*pRfDev;		/* device descriptor */
    FAST SLIP_REQ 	*pSlipReq;		/* user's slip request */

    {
    FAST IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
    FAST MAPPB mappb; 			/* parameter block for slip commands */
    FAST DISK_GEOM *pDiskGeom = &geometry[pRfDev->devInfo.drive];

    rfDevTake (pRfDev, FALSE);		/* block other users */ 

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */

#ifdef DSKBUG
    printf ("slip: unit %x\n", pRfDev->devInfo.drive);
#endif DSKBUG

    if (pDiskGeom->sectrk == 0)			/* check if geom. set */
	{
	printf ("%d: drive geometry not set!\n", pRfDev->devInfo.drive);
	return (ERROR);
	}

    rfIoctlStateSet (TRUE, pRfDev);
						/* Set up slip command */
    mappb.id		= (UINT) pRfDev; 		
    mappb.command 	= C_SLIPSECTOR;
    mappb.control 	= 0;
    mappb.unit 		= pRfDev->devInfo.drive;
    mappb.baditem 	= pSlipReq->startBlock; /* sector to slip */
    mappb.alternate 	= 0;
    mappb.recover 	= pSlipReq->recover;
    mappb.resv1 	= 0;
    mappb.resv2 	= 0;

    rfPBPut (&mappb, pRfDev);			/* put PB in Cmd List */

    semTake (pRfDev->intrSemId, WAIT_FOREVER);	/* wait for interrupt */

    pSlipReq->error = pIoctlInfo->error;	/* reuturn error to user */

    rfIoctlStateSet (FALSE, pRfDev);
    rfDevGive(pRfDev); 
    return (pIoctlInfo->error ? ERROR : OK);    /* return error status */
    }

/******************************************************************************
*
* rfSMDDefine -
*
* Process a user's define SMD parameter request.
*
* RETURNS:
*/
LOCAL STATUS rfSMDDefine (pRfDev, pDefSMDReq)		
    FAST RF_DEV     *pRfDev;			/* device descriptor */
    FAST DEFSMD_REQ *pDefSMDReq;		/* user's define SMD request */

    {
    DEFSMDPB   defSMDPb;
    FAST IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
    FAST DISK_GEOM  *pDiskGeom  = &geometry[pRfDev->devInfo.drive];
    FAST RF_CTLR    *pRfCtlr    = &rfCtlr[pRfDev->ctlr];

    rfDevTake (pRfDev, FALSE);		/* block other users */ 

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */

#ifdef DSKBUG
    printf ("defSMD: unit %x\n", pRfDev->devInfo.drive);
#endif DSKBUG

    rfIoctlStateSet (TRUE, pRfDev);
						/* Set up slip command */

				/* Set up a define SMD parameter block */
    defSMDPb.id 	= (UINT) pRfDev;
    defSMDPb.resv1 	= 0;
    defSMDPb.unit 	= pRfDev->devInfo.drive;
    defSMDPb.control 	= 0;
    defSMDPb.command 	= C_SETSMDPARAM;
    defSMDPb.interleave = pDefSMDReq->interleave;
    defSMDPb.headskew 	= pDefSMDReq->headskew;
    defSMDPb.cylskew 	= pDefSMDReq->cylskew;
    defSMDPb.hgskew 	= pDefSMDReq->hgskew;
    defSMDPb.recovery 	= pDefSMDReq->recovery;
    defSMDPb.resv3 	= 0;
    defSMDPb.hgskew 	= 0;
    defSMDPb.idpre 	= pDiskGeom->gap1 = pDefSMDReq->idpre;
    defSMDPb.datapre 	= pDiskGeom->gap2 = pDefSMDReq->datapre;

    rfPBPut (&defSMDPb, pRfDev);		/* put PB in Cmd List */

    semTake (pRfDev->intrSemId, WAIT_FOREVER);	/* wait for interrupt */

    rfIoctlStateSet (FALSE, pRfDev);
    rfDevGive(pRfDev); 
    return (pIoctlInfo->error ? ERROR : OK);     /* return error status */
    }

/******************************************************************************
*
* rfStatGet -
*
* Process a user's board statistics request.
*
* RETURNS: 0 if ok or errno
*/
LOCAL STATUS rfStatGet (pRfDev, pStatReq)		/* untested */
    FAST RF_DEV     *pRfDev;			/* device descriptor */
    FAST STAT_REQ *pStatReq; 		/* the statistics request */

    {
    IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
    DISK_GEOM 	    *pDiskGeom = &geometry[pRfDev->devInfo.drive];
    FAST STATPB     statPb;
    BOARDST 	    *pDMAStats; /* parameter block for statistics commands */
    BOARDST 	    stats;

    rfDevTake (pRfDev, FALSE);		/* block other users */ 

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */

			/* get some memory to DMA the statistics into */
    pDMAStats = (BOARDST *) rfMalloc (sizeof(BOARDST));
    if (pDMAStats == NULL)
	{
	printf ("getstat: Cannot malloc statistics buffer!\n");
	return (ERROR);
	}

    rfIoctlStateSet (TRUE, pRfDev);	
					    /* set up statistics command */
    statPb.id 		= (UINT)pRfDev;
    statPb.command 	= C_STATS;
    statPb.unit 	= pRfDev->devInfo.drive;
    statPb.control 	= 0;
    statPb.addrmod 	= VME_ADD_MOD;
    if ((statPb.vmeaddr = rfLocalToBusAdrs(pDMAStats)) == NULL)
	/* XXX error handling here */ ;
    statPb.spare 	= 0;
    statPb.resv 	= 0;
    statPb.resv1 	= 0;
    statPb.clear 	= pStatReq->clear;

    rfPBPut (&statPb, pRfDev);			/* put PB in Cmd List */

    semTake (pRfDev->intrSemId, WAIT_FOREVER);	/* wait for interrupt */

    if (pIoctlInfo->error)
	{
	pStatReq->error = pIoctlInfo->error;
	rfFree ((char*)pDMAStats);			/* free DVMA memory */
	rfIoctlStateSet (FALSE, pRfDev);
	rfDevGive(pRfDev); 
	return (OK);
	}

    *pStatReq->pBoardStats = *pDMAStats;	/* give stats to user */
    rfFree ((char*)pDMAStats);			/* free DVMA memory */
    rfIoctlStateSet (FALSE, pRfDev);
    rfDevGive(pRfDev); 			/* release drive */
    return (pIoctlInfo->error ? ERROR : OK);    /* return error status */
    }

/******************************************************************************
*
* rfSync -
*
* Configure the given drive according to the specified drive
* configuration information.
*
* RETURNS: 
*   OK or ERROR
*
*/
STATUS rfSync (ctlr, drive)
    int ctlr;			/* controller number */
    int drive;			/* controller number */
    
    {
    RF_CTLR   *pRfCtlr = &rfCtlr[ctlr];
    RF_DEV    *pRfDev;				/* device descriptor */ 
    DISK_GEOM *pGeomReq;			/* user's requested geometry */
    PARMBLK   *pb;
    EXTPB     *pXPB;
#ifdef DEL
    CONFPB	confPb;
    DEFSMDPB 	defSMDPb;
    RETCONF 	confParams; 			/* configuration parameters */
    CONFIG_GET_REQ configGetReq;		/* request for config params */
    IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
#endif DEL
    pXPB = (EXTPB *) rfMalloc (sizeof (EXTPB));

    pXPB->sb.id 	= 0;
    pXPB->sb.errextra	= 0;
    pXPB->sb.errcode	= 0;
    pXPB->sb.status	= 0;
    pXPB->sb.errdata	= 0;
			    /* Set up a pbiguration parameter block */
    pb = &pXPB->pb;
    pb->id 		= 0;
    pb->command 	= C_REZERO;
    pb->control 	= 0;
    pb->unit 		= drive;
    pb->addrmod		= 0;
    pb->diskaddr	= 0;
    pb->vmeaddr		= 0;
    pb->blkcount	= 0;
    pb->reserved	= 0;


    if (rfSglCmd (pXPB, pRfCtlr))
	{
	printf ("rfSync: command failed, shit.\n");
	rfFree ((char *) pXPB);
	return (ERROR);
	}

    rfFree ((char *) pXPB);
    return (OK);
    }

/******************************************************************************
*
* rfTrkSecMap -
*
* Process a user's track mapping request.
*
* RETURNS:
*/

LOCAL STATUS rfTrkSecMap (pRfDev, pMapReq)	
    FAST RF_DEV     *pRfDev;			/* device descriptor */
    FAST MAP_REQ *pMapReq; 			/* the mapping request */

    {
    FAST IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
    FAST DISK_GEOM *pDiskGeom = &geometry[pRfDev->devInfo.drive];
    MAPPB 	mappb; 		/* param block for map commands */
    long 	count; 		/* number of sectors or tracks to map */
    int 	next; 		/*  1: map sector, sec/track: map track */
    word 	error = 0; 	/* did an error occur on any map request */

    if (pDiskGeom->sectrk == 0)
	{
	printf ("rfTrkSecMap:disk geom not set.drive: %x\n", 
		    pRfDev->devInfo.drive);
	return (ERROR);
	}

    rfDevTake (pRfDev, FALSE);		/* block other users */ 

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */

				/* set the appropriate fields in the IOPB */
    rfIoctlStateSet (TRUE, pRfDev);

#ifdef DSKBUG
    printf ("rfTrkSecMap: unit %x\n", pRfDev->devInfo.drive);
#endif DSKBUG
						/* Set up mapping command */
    mappb.id 		= (UINT) pRfDev;
    mappb.control 	= 0;
    mappb.unit 		= pRfDev->devInfo.drive;
    mappb.baditem 	= (dword)pMapReq->startBlock; /* sec to map */
    mappb.alternate 	= (dword)pMapReq->destBlock;  /* and where to map it*/
    mappb.resv1 	= 0;
    mappb.resv2 	= 0;
    mappb.recover 	= pMapReq->recover;

    /* Set command for either mapping sector or track:
    * if the bad sector is the first on the track and
    * the number of sectors to map is a multiple of track size
    * then map tracks, otherwise map sectors
    */

    if ((pMapReq->startBlock % pDiskGeom->sectrk) == 0 &&
	(pMapReq->nBlocks % pDiskGeom->sectrk) == 0)
	{  			/* map tracks - keep count in tracks */
#ifdef DSKBUG
	printf ("Mapping tracks\n");
#endif DSKBUG
	mappb.command 	= C_MAPTRACK;
	count 		= pMapReq->nBlocks / pDiskGeom->sectrk;
	next 		= pDiskGeom->sectrk;
				/* set return info to user */
	pMapReq->mapType = C_MAPTRACK;

	}
    else  			/* map sectors - keep count in sectors */
	{
#ifdef DSKBUG
	printf ("Mapping sectors\n");
#endif DSKBUG
	mappb.command 	= C_MAPSECTOR;
	count 		= pMapReq->nBlocks;
	next 		= 1;
				/* set return info to user */
	pMapReq->mapType = C_MAPSECTOR;
	}

    /* 
     * execute successive mapping commands until all sectors or tracks
     * are mapped.  If an error occurs on any request stop.
     */
    while (count-- > 0)
	{
	rfPBPut (&mappb, pRfDev);		/* put PB in Cmd List */

	semTake (pRfDev->intrSemId, WAIT_FOREVER);	/* wait for interrupt */

	if (error = pIoctlInfo->error)		/* check for an error */
	    break;

	mappb.baditem += next;			/* get next sector to map */

	if (pMapReq->startBlock == -1)
	    mappb.alternate = -1;
	else
	    mappb.alternate += next;
	}

    pMapReq->error     = error; 	    /* pass back error information */
    pMapReq->destBlock = mappb.alternate;  /* and the place it was mapped to */

    rfIoctlStateSet (FALSE, pRfDev);
    rfDevGive (pRfDev);		  		/* allow access to drive */
    return (pIoctlInfo->error ? ERROR : OK);    /* return error status */
    }

/******************************************************************************
*
* rfTune -
*
* process a user's SMD tuning request.
*
* RETURNS: OK | ERROR
*/
LOCAL STATUS rfTune (pRfDev, pUStats)		/* untested */
    FAST RF_DEV *pRfDev;			/* device descriptor */
    FAST TUNEST *pUStats;			/* user board tuning request */

    {
    FAST IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
    FAST PARMBLK pb;
    FAST DISK_GEOM *pDiskGeom = &geometry[pRfDev->devInfo.drive];
    FAST TUNEST *pDMAStats;
    FAST int flag;

    rfDevTake (pRfDev);				/* block others */

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */

#ifdef DSKBUG
    printf ("rfTune: unit %x\n", pRfDev->devInfo.drive);
#endif DSKBUG


    /* get some memory to DMA the statistics into */
    pDMAStats = (TUNEST *) rfMalloc (sizeof(TUNEST));

    if (pDMAStats == NULL)
	{
	printf ("tune: Cannot rfMalloc statistics buffer!\n");
	return (1);
	}

    /* Set the tuning parameters */
    if (flag)
	*pDMAStats = *pUStats;


    if (pDiskGeom->sectrk == 0)			/* check if geom. set */
	{
	printf ("%d: drive geometry not set!\n", pRfDev->devInfo.drive);
	return (ERROR);
	}

    rfIoctlStateSet (TRUE, pRfDev);

    /* set up statistics command for controller */
    pb.id 	= (UINT)pRfDev;
    pb.command 	= flag ? 0xB : 0xA;
    pb.unit 	= 0;
    pb.control 	= 0;
    pb.addrmod 	= VME_ADD_MOD;
    if ((pb.vmeaddr = rfLocalToBusAdrs(pDMAStats)) == NULL)
	/* XXX error handling needed here */ ;
    pb.diskaddr = 0;
    pb.reserved = 0;
    pb.blkcount = 0;

    rfPBPut (&pb, pRfDev);			/* put PB in Cmd List */

    semTake (pRfDev->intrSemId, WAIT_FOREVER); 	/* wait for interrupt */

    /* copy statistics into where the user wants it */
    if (!flag)
	*pUStats = *pDMAStats;

    rfFree ((char*)pDMAStats);      		/* free DVMA memory */

    return (pIoctlInfo->error ? ERROR : OK);

    }
/*****************************************************************************
*
* rfVerify -
*
* process a users disk verification request.
*
* RETURNS: OK | ERROR
*/
STATUS rfVerify (pRfDev, pVerReq)		
    FAST RF_DEV *pRfDev;			/* device descriptor */
    FAST FMT_VER_REQ *pVerReq;			/* verify request */

    {
    FAST IOCTL_INFO *pIoctlInfo = rfIoctlInfoPtrGet(pRfDev);
    FMTVERPB 	verPb;

    rfDevTake (pRfDev, FALSE);		/* block other users */ 

    rfIoctlInfoClear (pIoctlInfo);	/* clear residual data */

				/* set the appropriate fields in the IOPB */

    rfIoctlStateSet (TRUE, pRfDev);
						/* create a verify command */
    verPb.id 		= (UINT)pRfDev; 
    verPb.command 	= C_VERIFY;
    verPb.control 	= 0;
    verPb.unit	 	= pRfDev->devInfo.drive;
    verPb.diskaddr 	= pVerReq->startBlock;
    verPb.blkcount 	= pVerReq->blockCount;
    verPb.resv0 	= 0;
    verPb.resv1 	= 0;

    rfPBPut (&verPb, pRfDev);			/* put PB in Cmd List */

    semTake (pRfDev->intrSemId, WAIT_FOREVER);	/* wait for interrupt */

    pVerReq->error    = pIoctlInfo->error;	/* give error code to user */
    pVerReq->badBlock = pIoctlInfo->data;	/* give bad block to user */

    rfIoctlStateSet (FALSE, pRfDev);

    rfDevGive (pRfDev);				/* allow access to drive */

    return  (pIoctlInfo->error ? ERROR : OK);
    }



/*---------------------------------------------------------------------------
* these functions can be used from the application level to 
* help debug things.
* ---------------------------------------------------------------------------*/
/******************************************************************************
*
* rfParamBlkPrint -
* 
* function to print out the contents of a standard parameter block.
*
* RETURNS: nothing.
*/  
LOCAL STATUS rfParamBlkPrint (pPB)
    PARMBLK *pPB;		/* parameter block to be printed */

    {
    printf ("id: 0x%x\n", pPB->id);
    printf ("addrmod: 0x%x\n", pPB->addrmod);
    printf ("unit: 0x%x\n", pPB->unit);
    printf ("control: 0x%x\n", pPB->control);
    printf ("command: 0x%x\n", pPB->command);
    printf ("diskaddr: 0x%x\n", pPB->diskaddr);
    printf ("vmeaddr: 0x%x\n", pPB->vmeaddr);
    printf ("reserved: 0x%x\n", pPB->reserved);
    printf ("blkcount: 0x%x\n", pPB->blkcount);
    }

/******************************************************************************
*
* rfSMDChrsPrint -
*
* print out the smd characteristics of a drive
*
* REUTRNS: nothing.
*/  
VOID rfSMDChrsPrint (ctlr, drive, pSMDChrs)
    int ctlr;
    int drive;
    SMDCHRS *pSMDChrs;


    {
    printf ("SMD characteristics for drive 0x%x are:\n", drive);
    printf ("   resv0:   0x%x\n", pSMDChrs->resv0);
    printf ("   cyld:    0x%x\n", pSMDChrs->cyld);
    printf ("   nspare:  0x%x\n", pSMDChrs->nspare);
    printf ("   dsctrk:  0x%x\n", pSMDChrs->dsctrk);
    printf ("   nhead:   0x%x\n", pSMDChrs->nhead);
    printf ("   resv1:   0x%x\n", pSMDChrs->resv1);
    printf ("   resv2:   0x%x\n", pSMDChrs->resv2);
    printf ("   psctrk:  0x%x\n", pSMDChrs->psctrk);
    printf ("   flags:   0x%x\n", pSMDChrs->flags);
    }

/******************************************************************************
*
* rfConfigParamsPrint -
*
* print out the RETCONF params structure.
*
* RETURNS nothing.
*/  
VOID rfConfigParamsPrint (ctlr, drive, pRetConf)
    int ctlr;			/*controller number */
    int drive;			/* drive number */
    RETCONF *pRetConf;		/* returned configuration paramters */

    {
    printf ("    bytsec:     0x%x\n", pRetConf->bytsec);
    printf ("    cyldsk:     0x%x\n", pRetConf->cyldsk);
    printf ("    nspares:    0x%x\n", pRetConf->nspares);
    printf ("    sectrk:     0x%x\n", pRetConf->sectrk);
    printf ("    headcyl:    0x%x\n", pRetConf->headcyl);
    printf ("    basehd:     0x%x\n", pRetConf->basehead);
    printf ("    flags:      0x%x\n", pRetConf->flags);
    printf ("    secdsk:     0x%x\n", pRetConf->secdsk);
    printf ("    interleave: 0x%x\n", pRetConf->interleave);
    printf ("    headskew:   0x%x\n", pRetConf->headskew);
    printf ("    cylskew:    0x%x\n", pRetConf->cylskew);
    printf ("    recovery:   0x%x\n", pRetConf->recovery);
    printf ("    idpre:      0x%x\n", pRetConf->idpre);
    printf ("    datapre:    0x%x\n", pRetConf->datapre);
    }
