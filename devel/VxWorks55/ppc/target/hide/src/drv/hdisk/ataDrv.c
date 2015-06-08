/* ataDrv.c - ATA/IDE and ATAPI CDROM (LOCAL and PCMCIA) disk device driver */

/* Copyright 1989-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01y,29apr02,pmr  SPR 76487: set up configType properly in ataDrv().
01x,15jan02,jkf  SPR#72183, ataDevCreate missing ctrl parameter description.
01w,10dec01,jkf  avoid 32 access to 16 bit boundary in ataPiBlkRd
01v,09dec01,jkf  changed copyright to 2002
01u,08dec01,jkf  fixed the NULL pointer accesses in ataPiPktCmdExec().
01t,08dec01,jkf  SPR#28746, ataInit() caused extra int on some hardware
01s,19nov01,jac  fixed mishandling of UNIT_ATTENTION condition for ATAPI
                 devices.
01r,09nov01,jac  SPR#67795, added support for ATAPI CD-ROMs (according to
                 SFF-8020i) by merging changes from mjc's ATAPI driver
                 extensions.
01q,24jul00,jkf  SPR#29571, nSectors reset change,fixed typo in 01p.
01p,12jul00,rcs  Reset nSectors back to nSecs in ataRW on retryRW SPR#29571.
01o,19sep99,jkf  Removed extra SYS_ATA_INIT_RTN, now check wait in ataRW().
01n,17jul99,jkf  using words 60-61 to determine LBA instead 
		 of CHS calculation, for big drives, 8.4Gb+. SPR#22830.
01m,04mar99,jkf  Added SYS_ATA_INIT_RTN.  _func_sysAtaInit. SPR#24378.
01l,09jun98,dat  fixed conflicting prototypes, removed ref to sysMsDelay()
01k,09jun98,ms   removed IMPORT prototypes conflicting with sysLib.h proto's
01j,31mar98,map  removed INCLUDE_ATA, redefined sys* prototypes.
01i,23mar98,map  renamed macros, made endian safe, added docs.
01h,30oct97,db   added cmd to reinitialize controller with params read. fixed
                 bug reported in SPR #9139. used PCI macros for input/output.
01g,21apr97,hdn  fixed a semaphore timeout problem(SPR 8394).
01f,28feb97,dat  fixed SPRs 8084, 3273 from ideDrv.
01e,06nov96,dgp  doc: final formatting
01d,01nov96,hdn  added support for PCMCIA.
01c,25sep96,hdn  added support for ATA-2.
01b,01mar96,hdn  cleaned up.
01a,02mar95,hdn  written based on ideDrv.c.
*/

/*
DESCRIPTION

This is a driver for ATA/IDE and ATAPI CDROM devices on PCMCIA, ISA, and other
buses. The driver can be customized via various macros to run on a variety of
boards and both big-endian, and little endian CPUs.

USER-CALLABLE ROUTINES

Most of the routines in this driver are accessible only through the I/O
system.  However, two routines must be called directly:  ataDrv() to
initialize the driver and ataDevCreate() to create devices.

Before the driver can be used, it must be initialized by calling ataDrv().
This routine must be called exactly once, before any reads, writes, or
calls to ataDevCreate().  Normally, it is called from usrRoot() in
usrConfig.c.

The routine ataRawio() supports physical I/O access. The first
argument is a drive number, 0 or 1; the second argument is a pointer
to an ATA_RAW structure.

NOTE
Format is not supported, because ATA/IDE disks are already formatted, and bad
sectors are mapped.

During initialization this driver queries each disk to determine 
if the disk supports LBA.  16 bit words 0x60 and 0x61 (returned 
from the ATA IDENTIFY DEVICE command) may report a larger value 
than the product of the CHS fields on newer large disks (8.4Gb+).  
The driver will use strict LBA access commands and LBA geometry for 
drives reporting "total LBA sectors" greater than the product of CHS.
Although everyone should also be using strict LBA on LBA disks, some 
older systems (mostly PC's) do not and use only CHS.  Such system cannot 
view drives larger than 8GB.  VxWorks does not have such limitations.    
However, it may be desirable to force VxWorks ignore the LBA information 
in favor of CHS in order to mount a file system originally formatted on 
a CHS only system.  Setting the boolean ataForceCHSonLBA to TRUE will 
force the use of CHS parameters on all drives and the LBA parameters 
are ignored.  Again, setting this boolean may prevent access to the 
drives full capacity, since some manufacturers have stopped setting 
a drives CHS accurately in favor of LBA.

PARAMETERS
The ataDrv() function requires a configuration flag as a parameter.
The configuration flag is one of the following:

.TS
tab(|);
l l .
Transfer mode 

ATA_PIO_DEF_0    | PIO default mode
ATA_PIO_DEF_1    | PIO default mode, no IORDY
ATA_PIO_0        | PIO mode 0
ATA_PIO_1        | PIO mode 1
ATA_PIO_2        | PIO mode 2
ATA_PIO_3        | PIO mode 3
ATA_PIO_4        | PIO mode 4
ATA_PIO_AUTO     | PIO max supported mode
ATA_DMA_0        | DMA mode 0
ATA_DMA_1        | DMA mode 1
ATA_DMA_2        | DMA mode 2
ATA_DMA_AUTO     | DMA max supported mode

Transfer bits

ATA_BITS_16      | RW bits size, 16 bits
ATA_BITS_32      | RW bits size, 32 bits

Transfer unit

ATA_PIO_SINGLE   | RW PIO single sector
ATA_PIO_MULTI    | RW PIO multi sector
ATA_DMA_SINGLE   | RW DMA single word
ATA_DMA_MULTI    | RW DMA multi word

Geometry parameters

ATA_GEO_FORCE    | set geometry in the table
ATA_GEO_PHYSICAL | set physical geometry
ATA_GEO_CURRENT  | set current geometry
.TE

DMA transfer is not supported in this release.  If ATA_PIO_AUTO or ATA_DMA_AUTO
is specified, the driver automatically chooses the maximum mode supported by the
device.  If ATA_PIO_MULTI or ATA_DMA_MULTI is specified, and the device does
not support it, the driver automatically chooses single sector or word mode.
If ATA_BITS_32 is specified, the driver uses 32-bit transfer mode regardless of
the capability of the drive.  

If ATA_GEO_PHYSICAL is specified, the driver uses the physical geometry 
parameters stored in the drive.  If ATA_GEO_CURRENT is specified,
the driver uses current geometry parameters initialized by BIOS.  
If ATA_GEO_FORCE is specified, the driver uses geometry parameters stored 
in sysLib.c.

The geometry parameters are stored in the structure table
`ataTypes[]' in sysLib.c. That table has two entries, the first for
drive 0, the second for drive 1. The members of the structure
are:
.CS
    int cylinders;              /@ number of cylinders @/
    int heads;                  /@ number of heads @/
    int sectors;                /@ number of sectors per track @/
    int bytes;                  /@ number of bytes per sector @/
    int precomp;                /@ precompensation cylinder @/
.CE

This driver does not access the PCI-chip-set IDE interface, but rather takes
advantage of BIOS or VxWorks initialization.  Thus, the BIOS setting should 
match the modes specified by the configuration flag.

The BSP may provide a sysAtaInit() routine for situations where an ATA
controller RESET (0x1f6 or 0x3f6, bit 2 is set) clears ATA specific
functionality in a chipset that is not re-enabled per the ATA-2 spec.
 
This BSP routine should be declared in sysLib.c or sysAta.c as follows:
 
.CS
void sysAtaInit (BOOL ctrl)
    {
    /@ BSP SPECIFIC CODE HERE @/
    }
.CE

Then the BSP should perform the following operation
before ataDrv() is called, in sysHwInit for example:

.CS
    IMPORT VOIDFUNCPTR _func_sysAtaInit;
    /@ setup during initialization @/
    _func_sysAtaInit = (VOIDFUNCPTR) sysAtaInit;
.CE


 
It should contain chipset specific reset code, such as code which re-enables
PCI write posting for an integrated PCI-IDE device, for example.  This will
be executed during every ataDrv(), ataInit(), and ataReset() or equivalent  
block device routine.  If the sysAtaInit routine is not provided by the
BSP it is ignored by the driver, therefore it is not a required BSP routine.

SEE ALSO:
.pG "I/O System"
*/

#include "vxWorks.h"
#include "taskLib.h"
#include "ioLib.h"
#include "memLib.h"
#include "stdlib.h"
#include "errnoLib.h"
#include "stdio.h"
#include "string.h"
#include "private/semLibP.h"
#include "intLib.h"
#include "iv.h"
#include "wdLib.h"
#include "sysLib.h"
#include "sys/fcntlcom.h"
#include "drv/pcmcia/pcmciaLib.h"
#include "logLib.h"
#include "drv/hdisk/ataDrv.h"

#define VXDOS                           "VXDOS"
#define VXEXT                           "VXEXT"

/* imports */

IMPORT ATA_TYPE		ataTypes [ATA_MAX_CTRLS][ATA_MAX_DRIVES];
IMPORT ATA_RESOURCE	ataResources [ATA_MAX_CTRLS];

/* Byte swapping version of sysInWordString(), big-endian CPUs only */

IMPORT void	sysInWordStringRev (int port, short *pData, int count);

/* defines */

/* Read a BYTE from IO port, `ioAdrs' */

#ifndef ATA_IO_BYTE_READ
#define ATA_IO_BYTE_READ(ioAdrs)	sysInByte (ioAdrs)
#endif	/* ATA_IO_BYTE_READ */

/* Write a BYTE `byte' to IO port, `ioAdrs' */

#ifndef ATA_IO_BYTE_WRITE
#define ATA_IO_BYTE_WRITE(ioAdrs, byte)	sysOutByte (ioAdrs, byte)
#endif	/* ATA_IO_BYTE_WRITE */

/* Read 16-bit little-endian `nWords' into `pData' from IO port, `ioAdrs' */

#ifndef ATA_IO_NWORD_READ
#define ATA_IO_NWORD_READ(ioAdrs, pData, nWords)                        \
	sysInWordString (ioAdrs, pData, nWords)
#endif	/* ATA_IO_NWORD_READ */

/* Write 16-bit little-endian `nWords' from `pData' into IO port, `ioAdrs' */

#ifndef ATA_IO_NWORD_WRITE
#define ATA_IO_NWORD_WRITE(ioAdrs, pData, nWords)                       \
	sysOutWordString (ioAdrs, pData, nWords)
#endif /* ATA_IO_NWORD_WRITE */
    
/* Read 32-bit little-endian `nLongs' into `pData' from IO port, `ioAdrs' */

#ifndef ATA_IO_NLONG_READ
#define ATA_IO_NLONG_READ(ioAdrs, pData, nLongs)                        \
	sysInLongString (ioAdrs, pData, nLongs)
#endif	/* ATA_IO_NLONG_READ */

/* Write 32-bit little-endian `nLongs' from `pData' into IO port, `ioAdrs' */

#ifndef ATA_IO_NLONG_WRITE
#define ATA_IO_NLONG_WRITE(ioAdrs, pData, nLongs)                       \
	sysOutLongString (ioAdrs, pData, nLongs)
#endif	/* ATA_IO_NLONG_WRITE */

/* Read 32-bit CPU-endian `nWords' into `pData' from IO port, `ioAdrs' */

#ifndef	ATA_IO_NWORD_READ_SWAP
#  if (_BYTE_ORDER == _BIG_ENDIAN)
#  define ATA_IO_NWORD_READ_SWAP(ioAdrs, pData, nWords)                 \
	sysInWordStringRev (ioAdrs, pData, nWords)
#  else	/* (_BYTE_ORDER == _BIG_ENDIAN)	*/
#  define ATA_IO_NWORD_READ_SWAP(ioAdrs, pData, nWords)                 \
	ATA_IO_NWORD_READ (ioAdrs, pData, nWords)
#  endif /* (_BYTE_ORDER == _BIG_ENDIAN) */
#endif	/* ATA_IO_NLONG_READ_SWAP */

/* Delay to ensure Status Register content is valid */

#define ATA_WAIT_STATUS		sysDelay ()	/* >= 400 ns */

/* definitions relating to ATAPI commands */

#define ATAPI_MAX_CMD_LENGTH 12 /* maximum length in bytes of a ATAPI command */

#ifdef ATA_DEBUG

int	ataDebugLevel = 0;	/* debug verbosity level */
BOOL	ataIntrDebug  = 0;	/* interrupt notification On */

char *	ataErrStrs [ ] = 	/* error reason strings */
    {
    "Unknown Error",					/*  0 */
    "Wait for Command Packet request time expire",	/*  1 */
    "Error in Command Packet Request",			/*  2 */
    "Error in Command Packet Request",			/*  3 */
    "Wait for Data Request time expire",		/*  4 */
    "Data Request for NON Data command",		/*  5 */
    "Error in Data Request",				/*  6 */
    "Error in End of data transfer condition",		/*  7 */
    "Extra transfer request",				/*  8 */
    "Transfer size requested exceeds desired size",	/*  9 */
    "Transfer direction miscompare",			/* 10 */
    "No Sense",						/* 11 */
    "Recovered Error",					/* 12 */
    "Not Ready",					/* 13 */
    "Medium Error",					/* 14 */
    "Hardware Error",					/* 15 */
    "Illegal Request",					/* 16 */
    "Unit Attention",					/* 17 */
    "Data Protected",					/* 18 */
    "Aborted Command",					/* 19 */
    "Miscompare",					/* 20 */
    "\0",						/* 21 */
    "\0",						/* 22 */
    "\0",						/* 23 */
    "\0",						/* 24 */
    "\0",						/* 25 */
    "Overlapped commands are not implemented",		/* 26 */
    "DMA transfer is not implemented",			/* 27 */
    };

#   define ATA_DEBUG_MSG(lvl, fmt, a1, a2, a3, a4, a5, a6)		\
        if ((lvl) <= ataDebugLevel)					\
            logMsg (fmt, (int)(a1), (int)(a2), (int)(a3), (int)(a4), 	\
                    (int)(a5), (int)(a6));
#   define ATA_INTR_MSG(fmt, a1, a2, a3, a4, a5, a6)			\
        if (ataIntrDebug)						\
            logMsg (fmt, (int)(a1), (int)(a2), (int)(a3), (int)(a4), 	\
                    (int)(a5), (int)(a6));
#else
#   define ATA_DEBUG_MSG(lvl, fmt, a1, a2, a3, a4, a5, a6)
#   define ATA_INTR_MSG(fmt, a1, a2, a3, a4, a5, a6)
#endif

/* typedefs */

typedef struct ATAPI_CMD
    {
    uint8_t	cmdPkt [ATAPI_MAX_CMD_LENGTH];
    char * *	ppBuf;
    uint32_t	bufLength;
    t_data_dir	direction;
    uint32_t	desiredTransferSize;
    BOOL	dma;
    BOOL	overlap;
    } ATAPI_CMD;


/* Special BSP INIT After ATA Reset */

#ifndef SYS_ATA_INIT_RTN
#define SYS_ATA_INIT_RTN(ctrl)  if (_func_sysAtaInit != NULL)    \
                                    {                            \
                                    ((*_func_sysAtaInit)(ctrl)); \
                                    }
#endif

/* globals */

BOOL	  ataDrvInstalled = FALSE;	/* TRUE if installed */
BOOL	  ataForceCHSonLBA = FALSE;	/* hack, forces use of CHS params */

ATA_CTRL  ataCtrl [ATA_MAX_CTRLS];

/* BSP specific ATA Init/Reset routine */

VOIDFUNCPTR       _func_sysAtaInit = NULL;


/* locals */

LOCAL int       ataRetry = 3;		/* max retry count */

/* Used to hold LBA information, if larger than calculated CHS value */

LOCAL UINT32 ataLbaTotalSecs [ATA_MAX_CTRLS][ATA_MAX_DRIVES];


/* function prototypes */

LOCAL STATUS ataBlkRd	(ATA_DEV *pDev, int startBlk, int nBlks, char *p);
LOCAL STATUS ataBlkWrt	(ATA_DEV *pDev, int startBlk, int nBlks, char *p);
LOCAL STATUS ataReset	(ATA_DEV *pDev);
LOCAL STATUS ataStatus	(ATA_DEV *pDev);
LOCAL STATUS ataIoctl	(ATA_DEV *pDev, int function, int arg);
LOCAL STATUS ataBlkRW	(ATA_DEV *pDev, int startBlk, int nBlks, char *p,
			 int direction);
LOCAL void   ataWdog	(int ctrl);
LOCAL void   ataIntr	(int ctrl);
LOCAL STATUS ataInit	(int ctrl, int dev);
LOCAL void   ataWait	(int ctrl, int request);
LOCAL STATUS ataCmd	(int ctrl, int drive, int cmd, int arg0, int arg1);
LOCAL STATUS ataPread	(int ctrl, int drive, void *p);
LOCAL STATUS ataRW	(int ctrl, int drive, int cylinder, int head, int sec, 
	 		 void *p, int nSecs, int direction);

LOCAL STATUS ataPiWait	    (int ctrl, int request, BOOL reset);
LOCAL STATUS ataPiBlkRd     (ATA_DEV *pDev, int startBlk, int nBlks, char *pBuf);
LOCAL STATUS ataPiIoctl     (ATA_DEV *pDev, int function, int arg);
LOCAL STATUS ataPiReset     (ATA_DEV *pAtapiDev);
LOCAL STATUS ataPiStatusChk (ATA_DEV *pDev);
LOCAL STATUS ataPiInit      (int ctrl, int dev);
LOCAL STATUS ataPiPread     (int ctrl, int drive, void * buffer);
LOCAL STATUS ataDevIdentify (int ctrl, int dev);

/*******************************************************************************
*
* ataStub - block device stub
*
* This routine serves a as stub for a block device structure's routine pointers.
*
* RETURNS: ERROR always.
*/

LOCAL STATUS ataStub (void)
    {
    return (ERROR);
    } /* ataStub */


/*******************************************************************************
*
* ataDriveInit - initialize ATA drive
*
* This routine checks the drive presents, identifies its type, initializes the 
* drive controller and driver control structers.
*
* RETURNS: OK if drive was initialized successfuly, or ERROR.
*/

STATUS ataDriveInit
    (
    int	ctrl,
    int	drive
    )
    {
    ATA_CTRL    *pCtrl		= &ataCtrl[ctrl];
    ATA_DRIVE   *pDrive		= &pCtrl->drive[drive];
    ATA_PARAM   *pParam		= &pDrive->param;
    ATA_TYPE    *pType		= &ataTypes[ctrl][drive];
    int		configType	= pCtrl->configType;	/* configuration type */

    if (!pCtrl->installed)
        return (ERROR);

    if (pType->cylinders == 0)
        return (ERROR);		/* user specified the device as not present */

    semTake (&pCtrl->muteSem, WAIT_FOREVER);

    /* Identify device presence and its type */

    if (ataDevIdentify (ctrl, drive) != OK)
        goto driveInitExit;

    /* Set Reset function according to device type */

    if (pDrive->type == ATA_TYPE_ATA)
        pDrive->Reset = ataInit;
    else if (pDrive->type == ATA_TYPE_ATAPI)
        pDrive->Reset = ataPiInit;

    if (pDrive->type == ATA_TYPE_ATA)
        {

        if (ataPread (ctrl, drive, (char *)pParam) == OK)
            {

	    if ((pCtrl->ctrlType == ATA_PCMCIA) ||
		((pCtrl->ctrlType != ATA_PCMCIA) && (drive == 0)))
		{

                if (ataCmd (ctrl, drive, ATA_CMD_DIAGNOSE, 0, 0) != OK)
		    {
	            semGive (&pCtrl->muteSem);

	            return (ERROR);
		    }
		}

            /* find out geometry */

            if ((configType & ATA_GEO_MASK) == ATA_GEO_FORCE)
                {
                (void) ataCmd (ctrl, drive, ATA_CMD_INITP, 0, 0);
                (void) ataPread (ctrl, drive, (char *)pParam);
                }
            else if ((configType & ATA_GEO_MASK) == ATA_GEO_PHYSICAL)
                {
                pType->cylinders = pParam->cylinders - 1;
                pType->heads     = pParam->heads;
                pType->sectors   = pParam->sectors;
                }
            else if ((configType & ATA_GEO_MASK) == ATA_GEO_CURRENT)
                {

                if ((pParam->currentCylinders != 0) &&
                    (pParam->currentHeads != 0) &&
                    (pParam->currentSectors != 0))
                    {
                    pType->cylinders = pParam->currentCylinders - 1;
                    pType->heads     = pParam->currentHeads;
                    pType->sectors   = pParam->currentSectors;
                    }
                else
                    {
                    pType->cylinders = pParam->cylinders - 1;
                    pType->heads     = pParam->heads;
                    pType->sectors   = pParam->sectors;
                    }
                }

            /* 
             * Not all modern hard drives report a true capacity value 
             * in their IDENTIFY DEVICE CHS fields.
             * For example, a Western Digital 20 Gb drive reports 
             * its CHS as 16383 cylinders, 16 heads, and 63 spt.
             * This is about 8.4GB, but the LBA sectors is reported
             * as 0x02607780, which is closer to 20Gb, the true capacity
             * of the drive.  The reason for this is PC BIOS can have a 
             * 8.4GB limitation, and drive manufacturers have broken the 
             * ATA specification to be compatable.  Negative competition. 
             * Note that the ATA specifications original limit is 
             * about 136.9 Gb, however when combinined with a PC BIOS 
             * interface, a 8.4 Gb limit is produced.    
             * VxWorks does not have such limitations being a true 32bit OS,
             * but since the drive manufactures are not honoring the CHS
             * values, we have to allow for devices that demand "pure" LBA
             * and present incorrect CHS.
             * If the drive supports Logical Block Addresses (LBA)
             * then we need to check the field located at 16bit words 60 & 61,
             * "Total number of user addressable sectors (LBA mode only)". 
             * If this value is greater than the CHS fields report, 
             * then 60-61 holds the true size of the disk and that 
             * will be reported to the block device interface.
             * Note that the CHS values are still left as the disk reported.
             * This is tracked at WRS as SPR#22830
             */

            if (pParam->capabilities & 0x0200)  /* if (drive supports LBA) */
                {
                ataLbaTotalSecs[ctrl][drive] =  (UINT32) 
			((((UINT32) ((pParam->sectors0) & 0x0000ffff)) <<  0) | 
			 (((UINT32) ((pParam->sectors1) & 0x0000ffff)) << 16));

                ATA_DEBUG_MSG (1, "ID_DRIVE reports LBA (60-61) as 0x%08lx\n",
                               ataLbaTotalSecs[ctrl][drive], 0, 0, 0, 0, 0);
                }

            /*
             * reinitialize the controller with parameters read from the
             * controller.
             */

            (void) ataCmd (ctrl, drive, ATA_CMD_INITP, 0, 0);

            /* recalibrate (this command !!! is absent in ATA-4) */

            (void) ataCmd (ctrl, drive, ATA_CMD_RECALIB, 0, 0);
            }
        else
            {
            pDrive->state = ATA_DEV_PREAD_F;
            goto driveInitExit;
            }
        }
    else if (pDrive->type == ATA_TYPE_ATAPI)
        {
        /* Although ATAPI device parameters have been read by ataDevIdentify(), 
         * execute ATAPI Identify Device command to allow ATA commands 
         * acceptance by an ATAPI device after Diagnostic or Reset commands. 
         */

        if (ataPiPread (ctrl, drive, pParam) != OK)
            {
            pDrive->state = ATA_DEV_PREAD_F;

            goto driveInitExit;
            }
        }

    /* find out supported capabilities of the drive */

    pDrive->multiSecs = pParam->multiSecs & 0x00ff;
    pDrive->okMulti   = (pDrive->multiSecs != 0) ? TRUE : FALSE;
    pDrive->okIordy   = (pParam->capabilities & 0x0800) ? TRUE : FALSE;
    pDrive->okLba     = (pParam->capabilities & 0x0200) ? TRUE : FALSE;
    pDrive->okDma     = (pParam->capabilities & 0x0100) ? TRUE : FALSE;

    /* find out supported max PIO mode */

    pDrive->pioMode = (pParam->pioMode >> 8) & 0x03;	/* PIO 0,1,2 */

    if (pDrive->pioMode > 2)
        pDrive->pioMode = 0;

    if ((pDrive->okIordy) && (pParam->valid & 0x02))	/* PIO 3,4 */
        {

        if (pParam->advancedPio & 0x01)
            pDrive->pioMode = 3;

        if (pParam->advancedPio & 0x02)
            pDrive->pioMode = 4;
        }

    /* find out supported max DMA mode */

    if ((pDrive->okDma) && (pParam->valid & 0x02))
        {
        pDrive->singleDmaMode = (pParam->dmaMode >> 8) & 0x03;

        if (pDrive->singleDmaMode >= 2)
            pDrive->singleDmaMode = 0;

        pDrive->multiDmaMode  = 0;

        if (pParam->singleDma & 0x04)
            pDrive->singleDmaMode = 2;
        else if (pParam->singleDma & 0x02)
            pDrive->singleDmaMode = 1;
        else if (pParam->singleDma & 0x01)
            pDrive->singleDmaMode = 0;

        if (pParam->multiDma & 0x04)
            pDrive->multiDmaMode = 2;
        else if (pParam->multiDma & 0x02)
            pDrive->multiDmaMode = 1;
        else if (pParam->multiDma & 0x01)
            pDrive->multiDmaMode = 0;
        }

    /* find out transfer mode to use */

    pDrive->rwBits = configType & ATA_BITS_MASK;
    pDrive->rwPio  = configType & ATA_PIO_MASK;
    pDrive->rwDma  = configType & ATA_DMA_MASK;
    pDrive->rwMode = ATA_PIO_DEF_W;

    switch (configType & ATA_MODE_MASK)
        {
        case ATA_PIO_0:
        case ATA_PIO_1:
        case ATA_PIO_2:
        case ATA_PIO_3:
        case ATA_PIO_4:
        case ATA_PIO_DEF_0:
        case ATA_PIO_DEF_1:
            pDrive->rwMode = configType & ATA_MODE_MASK;
            break;

        case ATA_PIO_AUTO:
            pDrive->rwMode = ATA_PIO_W_0 + pDrive->pioMode;
            break;

        case ATA_DMA_0:
        case ATA_DMA_1:
        case ATA_DMA_2:

            if (pDrive->okDma)
                {

                if (pDrive->rwDma == ATA_DMA_SINGLE)
                    pDrive->rwMode |= ATA_DMA_SINGLE_0;

                if (pDrive->rwDma == ATA_DMA_MULTI)
                    pDrive->rwMode |= ATA_DMA_MULTI_0;
                }
            break;

        case ATA_DMA_AUTO:

            if (pDrive->okDma)
                {

                if (pDrive->rwDma == ATA_DMA_SINGLE)
                    pDrive->rwMode = ATA_DMA_SINGLE_0 + 
                                     pDrive->singleDmaMode;

                if (pDrive->rwDma == ATA_DMA_MULTI)
                    pDrive->rwMode = ATA_DMA_MULTI_0 + 
                                     pDrive->multiDmaMode;
                }
            break;

        default:
            break;

        }

    /* Set the transfer mode */

    (void) ataCmd (ctrl, drive, ATA_CMD_SET_FEATURE, ATA_SUB_SET_RWMODE,
                   pDrive->rwMode);

    /* Disable reverting to power on defaults */

    (void) ataCmd (ctrl, drive, ATA_CMD_SET_FEATURE, ATA_SUB_DISABLE_REVE, 0);

    /* Set multiple mode (multisector read/write) */

    if ((pDrive->rwPio == ATA_PIO_MULTI) && (pDrive->type == ATA_TYPE_ATA))
        {

        if (pDrive->okMulti)
            (void) ataCmd (ctrl, drive, ATA_CMD_SET_MULTI,
                           pDrive->multiSecs, 0);
        else
            pDrive->rwPio = ATA_PIO_SINGLE;
        }

    pDrive->state = ATA_DEV_OK;

driveInitExit:

    semGive (&pCtrl->muteSem);

    if (pDrive->state != ATA_DEV_OK)
        {
        ATA_DEBUG_MSG (1, "ataDriveInit%d/%d: ERROR: state=%d dev=0x%x "
                       "status=0x%x error=0x%x\n", ctrl, drive, pDrive->state, 
                       ATA_IO_BYTE_READ (pCtrl->sdh), 
                       ATA_IO_BYTE_READ (pCtrl->status), 
                       ATA_IO_BYTE_READ (pCtrl->error));
        return (ERROR);
        }

    return (OK);
    } /* ataDriveInit */

/*******************************************************************************
*
* ataDrv - initialize the ATA driver
*
* This routine initializes the ATA/IDE/ATAPI CDROM driver, sets up interrupt
* vectors, and performs hardware initialization of the ATA/IDE chip.
*
* This routine must be called exactly once, before any reads, writes,
* or calls to ataDevCreate().  Normally, it is called by usrRoot()
* in usrConfig.c.
*
* RETURNS: OK, or ERROR if initialization fails.
*
* SEE ALSO: ataDevCreate()
*/

STATUS ataDrv
    (
    int  ctrl,			/* controller no. */
    int  drives,		/* number of drives */
    int  vector,		/* interrupt vector */
    int  level,			/* interrupt level */
    int  configType,		/* configuration type */
    int  semTimeout,		/* timeout seconds for sync semaphore */
    int  wdgTimeout		/* timeout seconds for watch dog */
    )
    {
    ATA_CTRL        *pCtrl	= &ataCtrl[ctrl];
    ATA_RESOURCE    *pAta	= &ataResources[ctrl];
    PCCARD_RESOURCE *pResource	= &pAta->resource;
    ATA_DRIVE       *pDrive;
    ATA_PARAM       *pParam;
    ATA_TYPE        *pType;
    int             drive;
    int             ix;

    if ((ctrl >= ATA_MAX_CTRLS) || (drives > ATA_MAX_DRIVES))
	return (ERROR);

    if (!ataDrvInstalled)
	{

	for (ix = 0; ix < ATA_MAX_CTRLS; ix++)
            ataCtrl[ix].wdgId = wdCreate ();

    	ataDrvInstalled = TRUE;
	}

    if (!pCtrl->installed)
	{

	if (semTimeout == 0)
	    pCtrl->semTimeout = ATA_SEM_TIMEOUT_DEF;
	else
	    pCtrl->semTimeout = semTimeout;

	if (wdgTimeout == 0)
	    pCtrl->wdgTimeout = ATA_WDG_TIMEOUT_DEF;
	else
	    pCtrl->wdgTimeout = wdgTimeout;

        semBInit (&pCtrl->syncSem, SEM_Q_FIFO, SEM_EMPTY);
        semMInit (&pCtrl->muteSem, SEM_Q_PRIORITY | SEM_DELETE_SAFE |
	          SEM_INVERSION_SAFE);

	pCtrl->data	= ATA_DATA	(pResource->ioStart[0]);
	pCtrl->error	= ATA_ERROR	(pResource->ioStart[0]);
	pCtrl->feature	= ATA_FEATURE	(pResource->ioStart[0]);
	pCtrl->seccnt	= ATA_SECCNT	(pResource->ioStart[0]);
	pCtrl->sector	= ATA_SECTOR	(pResource->ioStart[0]);
	pCtrl->cylLo	= ATA_CYL_LO	(pResource->ioStart[0]);
	pCtrl->cylHi	= ATA_CYL_HI	(pResource->ioStart[0]);
	pCtrl->sdh	= ATA_SDH	(pResource->ioStart[0]);
	pCtrl->command	= ATA_COMMAND	(pResource->ioStart[0]);
	pCtrl->status	= ATA_STATUS	(pResource->ioStart[0]);
	pCtrl->aStatus	= ATA_A_STATUS	(pResource->ioStart[1]);
	pCtrl->dControl	= ATA_D_CONTROL (pResource->ioStart[1]);
	pCtrl->dAddress	= ATA_D_ADDRESS (pResource->ioStart[1]);

        (void) intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (vector),
		           (VOIDFUNCPTR)ataIntr, ctrl);
        sysIntEnablePIC (level);	/* unmask the interrupt level */
	pCtrl->intLevel = level;
	pCtrl->wdgOkay  = TRUE;
	pCtrl->configType = configType;

	semTake (&pCtrl->muteSem, WAIT_FOREVER);

	pCtrl->installed = TRUE;

        for (drive = 0; drive < drives; drive++)
	    {
	    pType  = &ataTypes[ctrl][drive];
	    pDrive = &pCtrl->drive[drive];
	    pParam = &pDrive->param;
            pDrive->state	= ATA_DEV_INIT;
            pDrive->type	= ATA_TYPE_INIT;
            pDrive->diagCode	= 0;
            pDrive->Reset	= ataInit;

            ataDriveInit (ctrl, drive);
            }

        ATA_DEBUG_MSG (1, "ataDrv Calling sysAtaInit (if present):\n",
                      0, 0, 0, 0, 0, 0);
 
	/* Call system INIT routine to allow proper PIO mode setup */

        SYS_ATA_INIT_RTN (ctrl);
 
        ATA_DEBUG_MSG (1, "ataDrv sysAtaInit returned:\n",
                      0, 0, 0, 0, 0, 0);

	semGive (&pCtrl->muteSem);
	}

    return (OK);
    }

/*******************************************************************************
*
* ataDevCreate - create a device for a ATA/IDE disk
*
* This routine creates a device for a specified ATA/IDE or ATAPI CDROM disk.
*
* <ctrl> is a controller number for the ATA controller; the primary controller
* is 0.  The maximum is specified via ATA_MAX_CTRLS.
*
* <drive> is the drive number for the ATA hard drive; the master drive
* is 0.  The maximum is specified via ATA_MAX_DRIVES.
*
* The <nBlocks> parameter specifies the size of the device in blocks.
* If <nBlocks> is zero, the whole disk is used.
*
* The <blkOffset> parameter specifies an offset, in blocks, from the start
* of the device to be used when writing or reading the hard disk.  This
* offset is added to the block numbers passed by the file system during
* disk accesses.  (VxWorks file systems always use block numbers beginning
* at zero for the start of a device.)
*
*
* RETURNS:
* A pointer to a block device structure (BLK_DEV) or NULL if memory cannot
* be allocated for the device structure.
*
* SEE ALSO: dosFsMkfs(), dosFsDevInit(), rt11FsDevInit(), rt11FsMkfs(),
* rawFsDevInit()
*/

BLK_DEV *ataDevCreate
    (
    int ctrl,     /* ATA controller number, 0 is the primary controller */
    int drive,    /* ATA drive number, 0 is the master drive */
    int nBlocks,  /* number of blocks on device, 0 = use entire disc */
    int blkOffset /* offset BLK_DEV nBlocks from the start of the drive */
    )
    {
    ATA_CTRL  *pCtrl	= &ataCtrl[ctrl];
    ATA_TYPE  *pType	= &ataTypes[ctrl][drive];
    ATA_DRIVE *pDrive	= &pCtrl->drive[drive];
    ATA_PARAM *pParam	= &pDrive->param; /* move * */
    ATA_DEV   *pDev;
    BLK_DEV   *pBlkdev;
    int	      maxBlks;

    if ((ctrl >= ATA_MAX_CTRLS) || (drive >= ATA_MAX_DRIVES) ||
        !ataDrvInstalled || !pCtrl->installed)
	return (NULL);

    if ((pDev = (ATA_DEV *)malloc(sizeof (ATA_DEV))) == NULL)
	return (NULL);

    pBlkdev = &pDev->blkDev;

    if ((pDrive->state == ATA_DEV_OK) || (pDrive->state == ATA_DEV_MED_CH))
        {
        pDrive->state = ATA_DEV_MED_CH;

        if (pDrive->type == ATA_TYPE_ATA)
            {

	    /* 
	     * if LBA is supported and ataLbaTotalSecs is not zero
	     * and ataLbaTotalSecs is greater than the product of
	     * CHS, then we should use the LBA value.
	     */

            if ((pDrive->okLba == TRUE)                     && 
		(ataLbaTotalSecs[ctrl][drive] != 0)         &&
		(ataForceCHSonLBA != TRUE)		    &&
		(ataLbaTotalSecs[ctrl][drive] > 
		 (pType->cylinders * pType->heads * pType->sectors)))   
	        {
		maxBlks = (ataLbaTotalSecs[ctrl][drive]) - blkOffset;
		}
	    else /* just use CHS */
	        {
		maxBlks = ((pType->cylinders * pType->heads * pType->sectors) 
			   - blkOffset);
		}

	    if (nBlocks == 0)
	        nBlocks = maxBlks;
    
	    if (nBlocks > maxBlks)
	        nBlocks = maxBlks;

	    pBlkdev->bd_nBlocks		= nBlocks;
	    pBlkdev->bd_bytesPerBlk	= pType->bytes;
	    pBlkdev->bd_blksPerTrack	= pType->sectors;
	    pBlkdev->bd_nHeads		= pType->heads;
	    pBlkdev->bd_removable	= TRUE;
	    pBlkdev->bd_retry		= 1;
	    pBlkdev->bd_mode		= O_RDWR;
	    pBlkdev->bd_readyChanged	= TRUE;
	    pBlkdev->bd_blkRd		= ataBlkRd;
	    pBlkdev->bd_blkWrt		= ataBlkWrt;
	    pBlkdev->bd_ioctl		= ataIoctl;
	    pBlkdev->bd_reset		= ataReset;
	    pBlkdev->bd_statusChk	= ataStatus;
	    pBlkdev->bd_reset		= NULL;
	    pBlkdev->bd_statusChk	= NULL;
	    pDev->ctrl			= ctrl;
	    pDev->drive			= drive;
	    pDev->blkOffset		= blkOffset;
	    }
        else if (pDrive->type == ATA_TYPE_ATAPI)
            {
            pBlkdev->bd_nBlocks         = 100;
            pBlkdev->bd_bytesPerBlk     = 2048;
            pBlkdev->bd_blksPerTrack    = pBlkdev->bd_nBlocks;
            pBlkdev->bd_nHeads          = 1;

            if (pParam->config & CONFIG_REMOVABLE)
                pBlkdev->bd_removable   = TRUE;
            else
                pBlkdev->bd_removable   = FALSE;

            pBlkdev->bd_retry           = 1;
            pBlkdev->bd_mode            = O_RDONLY;
            pBlkdev->bd_readyChanged    = TRUE;
            pBlkdev->bd_blkRd           = ataPiBlkRd;
            pBlkdev->bd_blkWrt          = ataStub;
            pBlkdev->bd_ioctl           = ataPiIoctl;
            pBlkdev->bd_reset           = ataPiReset;
            pBlkdev->bd_statusChk       = ataPiStatusChk;

	    pDev->ctrl		        = ctrl;
	    pDev->drive		        = drive;
	    pDev->blkOffset	        = blkOffset;
            pDev->nBlocks               = nBlocks;

	    }

        ataDriveInit (ctrl, drive);

        pDrive->state = ATA_DEV_OK;
	}

    return (&pDev->blkDev);
    }

/*******************************************************************************
*
* ataRawio - do raw I/O access
*
* This routine is called to perform raw I/O access.
*
* <drive> is a drive number for the hard drive: it must be 0 or 1.
*
* The <pAtaRaw> is a pointer to the structure ATA_RAW which is defined in 
* ataDrv.h.
*
* RETURNS:
* OK, or ERROR if the parameters are not valid.
*
*/

STATUS ataRawio
    (
    int      ctrl,
    int      drive,
    ATA_RAW  *pAtaRaw
    )
    {
    ATA_CTRL  *pCtrl	= &ataCtrl[ctrl];
    ATA_DRIVE *pDrive	= &pCtrl->drive[drive];
    ATA_TYPE  *pType	= &ataTypes[ctrl][drive];
    ATA_DEV   ataDev;
    BLK_DEV   *pBlkdev	= &ataDev.blkDev;
    UINT      startBlk;

    if ((ctrl >= ATA_MAX_CTRLS) || (drive >= ATA_MAX_DRIVES) ||
        !ataDrvInstalled || !pCtrl->installed)
	return (ERROR);

    if ((pAtaRaw->cylinder	>= pType->cylinders)	||
        (pAtaRaw->head		>= pType->heads)	||
        (pAtaRaw->sector	>  pType->sectors)	||
        (pAtaRaw->sector	== 0))
	return (ERROR);

    /*
     * if LBA is supported and ataLbaTotalSecs is not zero
     * and ataLbaTotalSecs is greater than the product of
     * CHS, then we should use the LBA value.
     */

    if ((pDrive->okLba == TRUE)                     && 
   	(ataLbaTotalSecs[ctrl][drive] != 0)         &&
	(ataForceCHSonLBA != TRUE)		    &&
	(ataLbaTotalSecs[ctrl][drive] > 
	(pType->cylinders * pType->heads * pType->sectors)))   
	{
	pBlkdev->bd_nBlocks = ataLbaTotalSecs[ctrl][drive];
	}
    else /* just use CHS value */
	{
        pBlkdev->bd_nBlocks	= pType->cylinders * pType->heads * 
				  pType->sectors;
	}

    pBlkdev->bd_bytesPerBlk	= pType->bytes;
    pBlkdev->bd_blksPerTrack	= pType->sectors;
    pBlkdev->bd_nHeads		= pType->heads;
    pBlkdev->bd_removable	= FALSE;
    pBlkdev->bd_retry		= 1;
    pBlkdev->bd_mode		= O_RDWR;
    pBlkdev->bd_readyChanged	= TRUE;
    pBlkdev->bd_blkRd		= ataBlkRd;
    pBlkdev->bd_blkWrt		= ataBlkWrt;
    pBlkdev->bd_ioctl		= ataIoctl;
    pBlkdev->bd_reset		= ataReset;
    pBlkdev->bd_statusChk	= ataStatus;

    ataDev.ctrl			= ctrl;
    ataDev.drive		= drive;
    ataDev.blkOffset		= 0;

    startBlk = pAtaRaw->cylinder * (pType->sectors * pType->heads) +
	       pAtaRaw->head * pType->sectors + pAtaRaw->sector - 1;

    return (ataBlkRW (&ataDev, startBlk, pAtaRaw->nSecs, pAtaRaw->pBuf,
		     pAtaRaw->direction));
    }

/*******************************************************************************
*
* ataBlkRd - read one or more blocks from a ATA/IDE disk
*
* This routine reads one or more blocks from the specified device,
* starting with the specified block number.
*
* If any block offset was specified during ataDevCreate(), it is added
* to <startBlk> before the transfer takes place.
*
* RETURNS: OK, ERROR if the read command didn't succeed.
*/

LOCAL STATUS ataBlkRd
    (
    ATA_DEV *pDev,
    int     startBlk,
    int     nBlks,
    char    *pBuf
    )
    {
    return (ataBlkRW (pDev, startBlk, nBlks, pBuf, O_RDONLY));
    }

/*******************************************************************************
*
* ataBlkWrt - write one or more blocks to a ATA/IDE disk
*
* This routine writes one or more blocks to the specified device,
* starting with the specified block number.
*
* If any block offset was specified during ataDevCreate(), it is added
* to <startBlk> before the transfer takes place.
*
* RETURNS: OK, ERROR if the write command didn't succeed.
*/

LOCAL STATUS ataBlkWrt
    (
    ATA_DEV *pDev,
    int     startBlk,
    int     nBlks,
    char    *pBuf
    )
    {
    return (ataBlkRW (pDev, startBlk, nBlks, pBuf, O_WRONLY));
    }

/*******************************************************************************
*
* ataReset - reset a ATA/IDE disk controller
*
* This routine resets a ATA/IDE disk controller.
*
* RETURNS: OK, always.
*/

LOCAL STATUS ataReset
    (
    ATA_DEV  *pDev
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[pDev->ctrl];
    
    if (!pCtrl->installed)
	return (ERROR);

    semTake (&pCtrl->muteSem, WAIT_FOREVER);

    (void) ataInit (pDev->ctrl, 0);

    semGive (&pCtrl->muteSem);

    return (OK);
    }

/*******************************************************************************
*
* ataStatus - check status of a ATA/IDE disk controller
*
* This routine check status of a ATA/IDE disk controller.
*
* RETURNS: OK, ERROR if the card is removed.
*/

LOCAL STATUS ataStatus
    (
    ATA_DEV  *pDev
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[pDev->ctrl];
    BLK_DEV  *pBlkdev	= &pDev->blkDev;
    
    if (!pCtrl->installed)
	return (ERROR);

    if (pCtrl->changed)
	{
	pBlkdev->bd_readyChanged = TRUE;
	pCtrl->changed		 = FALSE;
	}

    return (OK);
    }

/*******************************************************************************
*
* ataIoctl - do device specific control function
*
* This routine is called when the file system cannot handle an ioctl()
* function.
*
* RETURNS:  OK or ERROR.
*/

LOCAL STATUS ataIoctl
    (
    ATA_DEV  *pDev,
    int      function,
    int      arg
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[pDev->ctrl];
    FAST int status	= ERROR;

    if (!pCtrl->installed)
        return (ERROR);

    semTake (&pCtrl->muteSem, WAIT_FOREVER);

    switch (function)
	{
	case FIODISKFORMAT:
	    (void) errnoSet (S_ioLib_UNKNOWN_REQUEST);
	    break;

	default:
	    (void) errnoSet (S_ioLib_UNKNOWN_REQUEST);
	}

    semGive (&pCtrl->muteSem);

    return (status);
    }

/*******************************************************************************
*
* ataBlkRW - read or write sectors to a ATA/IDE disk.
*
* Read or write sectors to a ATA/IDE disk.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ataBlkRW
    (
    ATA_DEV *pDev,
    int     startBlk,
    int     nBlks,
    char    *pBuf,
    int     direction
    )
    {
    ATA_CTRL  *pCtrl	= &ataCtrl[pDev->ctrl];
    ATA_DRIVE *pDrive	= &pCtrl->drive[pDev->drive];
    BLK_DEV   *pBlkdev	= &pDev->blkDev;
    ATA_TYPE  *pType	= &ataTypes[pDev->ctrl][pDev->drive];
    int       status	= ERROR;
    int       retryRW0	= 0;
    int       retryRW1	= 0;
    int       retrySeek	= 0;
    int       cylinder;
    int       head;
    int       sector;
    int       nSecs;
    int       ix;

    /* sanity check */

    if (!pCtrl->installed)
        return (ERROR);

    nSecs = pBlkdev->bd_nBlocks;

    if ((startBlk + nBlks) > nSecs)
	{
	ATA_DEBUG_MSG (1, "startBlk=%d nBlks=%d: 0 - %d\n", startBlk, nBlks, 
                       nSecs, 0, 0, 0);

	return (ERROR);
	}

    startBlk += pDev->blkOffset;

    semTake (&pCtrl->muteSem, WAIT_FOREVER);

    for (ix = 0; ix < nBlks; ix += nSecs)
	{

	if (pDrive->okLba)
	    {
	    head     = (startBlk & 0x0f000000) >> 24;
	    cylinder = (startBlk & 0x00ffff00) >> 8;
	    sector   = (startBlk & 0x000000ff);
	    }
	else
	    {
	    cylinder = startBlk / (pType->sectors * pType->heads);
	    sector   = startBlk % (pType->sectors * pType->heads);
	    head     = sector / pType->sectors;
	    sector   = sector % pType->sectors + 1;
	    }
	nSecs    = min (nBlks - ix, ATA_MAX_RW_SECTORS);

	retryRW1 = 0;
	retryRW0 = 0;

	while (ataRW(pDev->ctrl, pDev->drive, cylinder, head, sector, 
	       pBuf, nSecs, direction) != OK)
	    {

	    if (++retryRW0 > ataRetry)
		{
	        (void)ataCmd (pDev->ctrl, pDev->drive, ATA_CMD_RECALIB, 0, 0);

	        if (++retryRW1 > ataRetry)
		    goto done;

	        retrySeek = 0;

	        while (ataCmd (pDev->ctrl, pDev->drive, ATA_CMD_SEEK, cylinder,
			       head) != OK)

	            if (++retrySeek > ataRetry)
		        goto done;

	        retryRW0 = 0;
		}
	    }

        startBlk += nSecs;
        pBuf += pBlkdev->bd_bytesPerBlk * nSecs;
	}

    status = OK;

done:

    if (status == ERROR)
        (void)errnoSet (S_ioLib_DEVICE_ERROR);

    semGive (&pCtrl->muteSem);

    return (status);
    }

/*******************************************************************************
*
* ataIntr - ATA/IDE controller interrupt handler.
*
* RETURNS: N/A
*/

LOCAL void ataIntr
    (
    int ctrl
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[ctrl];

    pCtrl->intCount++;
    pCtrl->intStatus = ATA_IO_BYTE_READ (pCtrl->status);
    semGive (&pCtrl->syncSem);
    }

/*******************************************************************************
*
* ataWdog - ATA/IDE controller watchdog handler.
*
* RETURNS: N/A
*/

LOCAL void ataWdog
    (
    int ctrl
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[ctrl];

    pCtrl->wdgOkay = FALSE;
    }

/*******************************************************************************
*
* ataWait - wait the ATA/ATAPI drive ready
*
* Wait the drive ready
*
* RETURNS: OK, ERROR if the drive didn't become ready in certain period of time.
*/

LOCAL void ataWait
    (
    int ctrl,
    int request
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[ctrl];

    ATA_DEBUG_MSG (3, "ataWait: ctrl=%d  request=0x%x\n", ctrl, request,
                  0, 0, 0, 0);

    switch (request)
	{
	case ATA_STAT_READY:
	    wdStart (pCtrl->wdgId, (sysClkRateGet() * pCtrl->wdgTimeout), 
		     (FUNCPTR)ataWdog, ctrl);

            while ((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_BUSY) && 
		   (pCtrl->wdgOkay))
	        ;

            while (((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_READY) == 0)
                   && (pCtrl->wdgOkay))
	        ;

	    wdCancel (pCtrl->wdgId);

            if (!pCtrl->wdgOkay)
	        {
	        pCtrl->wdgOkay = TRUE;
	        (void) ataInit (ctrl, 0);
	        }

	    break;

	case ATA_STAT_BUSY:

            while (ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_BUSY)
		;

	    break;

	case ATA_STAT_DRQ:

            while ((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_DRQ) == 0)
	        ;

	    break;

	case ATA_STAT_SEEKCMPLT:

            while ((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_SEEKCMPLT) == 0)
	        ;

	    break;
	}

    ATA_DEBUG_MSG (3, "ataWait end:\n", 0, 0, 0, 0, 0, 0);
    }

/*******************************************************************************
*
* ataPiWait - wait the ATAPI drive ready
*
* Wait the drive ready
*
* RETURNS: OK, ERROR if the drive didn't become ready in certain period of time.
*/

LOCAL STATUS ataPiWait
    (
    int 	ctrl,
    int 	request,
    BOOL	reset
    )
    {
    ATA_CTRL    *pCtrl	= &ataCtrl[ctrl]; 
    ATA_DRIVE   *pDrive;
    int		drive;

    ATA_DEBUG_MSG (3, "ataPiWait: ctrl=%d  request=0x%x\n", ctrl, request,
                   0, 0, 0, 0);

    drive = (ATA_IO_BYTE_READ (pCtrl->sdh) >> 4) & 1;

    pDrive = &pCtrl->drive[drive];

    switch (request)
	{
	case ATA_STAT_READY:
	    wdStart (pCtrl->wdgId, (sysClkRateGet() * pCtrl->wdgTimeout), 
		     (FUNCPTR)ataWdog, ctrl);

            while ((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_BUSY) && 
		   (pCtrl->wdgOkay))
	        ;

            while (((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_READY) == 0)
                   && (pCtrl->wdgOkay))
	        ;

            while ((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_DRQ) &&
		   (pCtrl->wdgOkay))
                ;

	    wdCancel (pCtrl->wdgId);

            if (!pCtrl->wdgOkay)
	        {
	        pCtrl->wdgOkay = TRUE;

                if (reset == TRUE)
	            (void) pDrive->Reset (ctrl, drive);

                return (ERROR);
	        }

	    break;

	case ATA_STAT_ACCESS:
	    wdStart (pCtrl->wdgId, (sysClkRateGet() * pCtrl->wdgTimeout), 
		     (FUNCPTR)ataWdog, ctrl);

            while ((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_BUSY) && 
		   (pCtrl->wdgOkay))
	        ;

            while ((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_DRQ) &&
		   (pCtrl->wdgOkay))
                ;

	    wdCancel (pCtrl->wdgId);

            if (!pCtrl->wdgOkay)
	        {
	        pCtrl->wdgOkay = TRUE;

                if (reset == TRUE)
	            (void) pDrive->Reset (ctrl, drive);

                return (ERROR);
	        }

	    break;

	case ATA_STAT_BUSY:

            while (ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_BUSY)
		;

	    break;

	case ATA_STAT_DRQ:

            while ((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_DRQ) == 0)
	        ;

	    break;

	case ATA_STAT_SEEKCMPLT:

            while ((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_SEEKCMPLT) 
                   == 0)
	        ;

	    break;
	}

    ATA_DEBUG_MSG (3, "ataPiWait end:\n", 0, 0, 0, 0, 0, 0);

    return (OK);
    } /* ataPiWait */


/*******************************************************************************
*
* ataInit - init a ATA/IDE disk controller
*
* This routine initializes a ATA/IDE disk controller.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ataInit
    (
    int ctrl,
    int dev
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[ctrl];
    int      ix;

    ATA_DEBUG_MSG (2, "ataInit: ctrl=%d\n", ctrl, 0, 0, 0, 0, 0);

    ATA_IO_BYTE_WRITE (pCtrl->dControl,
                       ATA_CTL_4BIT | ATA_CTL_RST);

    for (ix = 0; ix < 100; ix++)
        sysDelay ();

    ATA_IO_BYTE_WRITE (pCtrl->dControl, ATA_CTL_4BIT);

    for (ix = 0; ix < 100; ix++)
        sysDelay ();

    pCtrl->wdgOkay = TRUE;

    /* start the ata  watchdog */

    wdStart (pCtrl->wdgId, (sysClkRateGet() * pCtrl->wdgTimeout),
	     (FUNCPTR)ataWdog, ctrl);

    while ((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_BUSY) &&
           (pCtrl->wdgOkay))
	;

    ATA_IO_BYTE_WRITE (pCtrl->dControl, ATA_CTL_4BIT);

    for (ix = 0; ix < 100; ix++) 
        sysDelay ();

    while (((ATA_IO_BYTE_READ (pCtrl->aStatus) & ATA_STAT_READY) == 0) &&
	   (pCtrl->wdgOkay))
	;

    wdCancel (pCtrl->wdgId);

    if (!pCtrl->wdgOkay)
        {
	ATA_DEBUG_MSG (1, "ataInit error:\n", 0, 0, 0, 0, 0, 0);

        pCtrl->wdgOkay = TRUE;

        return (ERROR);
        }

    ATA_DEBUG_MSG (3, "ataInit Calling sysAtaInit (if present):\n",
                   0, 0, 0, 0, 0, 0);

    /* Call out to bsp specific setup routine */

    SYS_ATA_INIT_RTN (ctrl);
    ATA_DEBUG_MSG (3, "ataInit sysAtaInit returned\n", 0, 0, 0, 0, 0, 0);

    /*
     * The following allows recovery after an interrupt
     * caused by drive software reset
     */

    semBInit(&pCtrl->syncSem, SEM_Q_FIFO, SEM_EMPTY);

    ATA_DEBUG_MSG (2, "ataInit end\n", 0, 0, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* ataCmd - issue non data command
*
* Issue a non data command.  Non data commands have the same protocol.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ataCmd
    (
    int ctrl,
    int drive,
    int cmd,
    int arg0,
    int arg1
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[ctrl];
    ATA_TYPE *pType	= &ataTypes[ctrl][drive];
    BOOL     retry	= TRUE;
    int      retryCount	= 0;
    int      semStatus;

    ATA_DEBUG_MSG (2, "ataCmd%d/%d: cmd=0x%x\n", ctrl, drive, cmd, arg0, arg1, 0);

    while (retry)
	{
        ataWait (ctrl, ATA_STAT_READY);

	switch (cmd)
	    {
	    case ATA_CMD_DIAGNOSE:
        	ATA_IO_BYTE_WRITE (pCtrl->sdh, ATA_SDH_IBM | (drive << 4));
		break;

	    case ATA_CMD_INITP:
		ATA_IO_BYTE_WRITE (pCtrl->cylLo, pType->cylinders);
		ATA_IO_BYTE_WRITE (pCtrl->cylHi, pType->cylinders >> 8);
		ATA_IO_BYTE_WRITE (pCtrl->seccnt, pType->sectors);
		ATA_IO_BYTE_WRITE (pCtrl->sdh, ATA_SDH_IBM | (drive << 4) | 
			    ((pType->heads - 1) & 0x0f));
		break;

	    case ATA_CMD_RECALIB:
		ATA_IO_BYTE_WRITE (pCtrl->sdh, ATA_SDH_IBM | (drive << 4));
		break;

	    case ATA_CMD_SEEK:
		ATA_IO_BYTE_WRITE (pCtrl->cylLo, arg0);
		ATA_IO_BYTE_WRITE (pCtrl->cylHi, arg0>>8);
		ATA_IO_BYTE_WRITE (pCtrl->sdh, ATA_SDH_IBM | (drive << 4) |
			    (arg1 & 0xf));
		break;

	    case ATA_CMD_SET_FEATURE:
        	ATA_IO_BYTE_WRITE (pCtrl->seccnt, arg1);
        	ATA_IO_BYTE_WRITE (pCtrl->feature, arg0);
        	ATA_IO_BYTE_WRITE (pCtrl->sdh, ATA_SDH_IBM | (drive << 4));
		break;

	    case ATA_CMD_SET_MULTI:
        	ATA_IO_BYTE_WRITE (pCtrl->seccnt, arg0);
        	ATA_IO_BYTE_WRITE (pCtrl->sdh, ATA_SDH_IBM | (drive << 4));
		break;

	    }

        ATA_IO_BYTE_WRITE (pCtrl->command, cmd);
        semStatus = semTake (&pCtrl->syncSem, 
			     sysClkRateGet() * pCtrl->semTimeout);

        if ((pCtrl->intStatus & ATA_STAT_ERR) || (semStatus == ERROR))
	    {

            ATA_DEBUG_MSG (1, "ataCmd err: status=0x%x semStatus=%d err=0x%x\n",
                           pCtrl->intStatus, semStatus,
                           ATA_IO_BYTE_READ (pCtrl->error), 0, 0, 0);

	    if (++retryCount > ataRetry)
	        return (ERROR);
	    }
	else
	    retry = FALSE;
	}

	switch (cmd)
	    {
	    case ATA_CMD_SEEK:
		ataWait (ctrl, ATA_STAT_SEEKCMPLT);
		break;

	    }

    ATA_DEBUG_MSG (2, "ataCmd end - ctrl %d, drive %d: Ok\n", ctrl, drive, 0, 0, 0, 0);

    return (OK);
    }

/*******************************************************************************
*
* ataPread - Read drive parameters
*
* Read drive parameters.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ataPread
    (
    int      ctrl,
    int      drive,
    void     *buffer
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[ctrl];
    BOOL     retry	= TRUE;
    int      retryCount	= 0;
    int      semStatus;

    ATA_DEBUG_MSG (2, "ataPread: ctrl=%d drive=%d\n", ctrl, drive, 0, 0, 0, 0);

    while (retry)
	{
        ataWait (ctrl, ATA_STAT_READY);

        ATA_IO_BYTE_WRITE (pCtrl->sdh, ATA_SDH_IBM | (drive << 4));
        ATA_IO_BYTE_WRITE (pCtrl->command, ATA_CMD_READP);
        semStatus = semTake (&pCtrl->syncSem, 
			     sysClkRateGet() * pCtrl->semTimeout);

        if ((pCtrl->intStatus & ATA_STAT_ERR) || (semStatus == ERROR))
	    {

            ATA_DEBUG_MSG (1, "ataPread%d/%d - err: status=0x%x intStatus=0x%x "

                          "error=0x%x semStatus=%d\n", ctrl, drive, 
                          ATA_IO_BYTE_READ (pCtrl->aStatus), pCtrl->intStatus, 
                          ATA_IO_BYTE_READ (pCtrl->error), semStatus);

	    if (++retryCount > ataRetry)
	        return (ERROR);
	    }
        else
	    retry = FALSE;
        }

    ataWait (ctrl, ATA_STAT_DRQ);

    ATA_IO_NWORD_READ_SWAP (pCtrl->data, (short *)buffer, 256);

    ATA_DEBUG_MSG (2, "ataPread end:\n", 0, 0, 0, 0, 0, 0);


    return (OK);
    }

/*******************************************************************************
*
* ataRW - read/write a number of sectors on the current track
*
* Read/write a number of sectors on the current track
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ataRW
    (
    int ctrl,
    int drive,
    int cylinder,
    int head,
    int sector,
    void *buffer,
    int nSecs,
    int direction
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[ctrl];
    ATA_DRIVE *pDrive	= &pCtrl->drive[drive];
    ATA_TYPE *pType	= &ataTypes[ctrl][drive];
    int retryCount	= 0;
    int block		= 1;
    int nSectors;
    int nWords;
    int semStatus;
    short *pBuf;

    ATA_DEBUG_MSG (2, "ataRW: ctrl=%d drive=%d c=%d h=%d s=%d buf=0x%x ",
                   ctrl, drive, cylinder, head, sector, (int)buffer);
    ATA_DEBUG_MSG (2, "n=%d dir=%d\n", nSecs, direction, 0, 0, 0, 0);

retryRW:

    ataWait (ctrl, ATA_STAT_READY);

    pBuf = (short *)buffer;
    nSectors = nSecs;

    ATA_IO_BYTE_WRITE (pCtrl->feature, pType->precomp);
    ATA_IO_BYTE_WRITE (pCtrl->seccnt, nSecs);
    ATA_IO_BYTE_WRITE (pCtrl->sector, sector);
    ATA_IO_BYTE_WRITE (pCtrl->cylLo, cylinder);
    ATA_IO_BYTE_WRITE (pCtrl->cylHi, cylinder>>8);

    if (pDrive->okLba)
        ATA_IO_BYTE_WRITE (pCtrl->sdh,
                           ATA_SDH_LBA | (drive << 4) | (head & 0xf));
    else
        ATA_IO_BYTE_WRITE (pCtrl->sdh,
                           ATA_SDH_IBM | (drive << 4) | (head & 0xf));

    if (pDrive->rwPio == ATA_PIO_MULTI)
	block = pDrive->multiSecs;

    nWords = (pType->bytes * block) >> 1;

    if (direction == O_WRONLY)
	{

        if (pDrive->rwPio == ATA_PIO_MULTI)
	    ATA_IO_BYTE_WRITE (pCtrl->command, ATA_CMD_WRITE_MULTI);
	else
	    ATA_IO_BYTE_WRITE (pCtrl->command, ATA_CMD_WRITE);

	while (nSectors > 0)
	    {

	    if ((pDrive->rwPio == ATA_PIO_MULTI) && (nSectors < block))
		{
		block = nSectors;
		nWords = (pType->bytes * block) >> 1;
		}

            ataWait (ctrl, ATA_STAT_BUSY);
            ataWait (ctrl, ATA_STAT_DRQ);

	    if (pDrive->rwBits == ATA_BITS_16)
	        ATA_IO_NWORD_WRITE (pCtrl->data, pBuf, nWords);
	    else
	        ATA_IO_NLONG_WRITE (pCtrl->data, (long *)pBuf, nWords >> 1);

            semStatus = semTake (&pCtrl->syncSem, 
				 sysClkRateGet() * pCtrl->semTimeout);

    	    if ((pCtrl->intStatus & ATA_STAT_ERR) || (semStatus == ERROR))
	        goto errorRW;

	    pBuf     += nWords;
	    nSectors -= block;
            }
	}
    else
	{

        if (pDrive->rwPio == ATA_PIO_MULTI)
	    ATA_IO_BYTE_WRITE (pCtrl->command, ATA_CMD_READ_MULTI);
	else
	    ATA_IO_BYTE_WRITE (pCtrl->command, ATA_CMD_READ);

	while (nSectors > 0)
	    {

	    if ((pDrive->rwPio == ATA_PIO_MULTI) && (nSectors < block))
		{
		block = nSectors;
		nWords = (pType->bytes * block) >> 1;
		}

            semStatus = semTake (&pCtrl->syncSem, 
				 sysClkRateGet() * pCtrl->semTimeout);

    	    if ((pCtrl->intStatus & ATA_STAT_ERR) || (semStatus == ERROR))
	        goto errorRW;

            ataWait (ctrl, ATA_STAT_BUSY); /* wait for slow disk */
            ataWait (ctrl, ATA_STAT_DRQ);

	    if (pDrive->rwBits == ATA_BITS_16)
	        ATA_IO_NWORD_READ (pCtrl->data, pBuf, nWords);
	    else
	        ATA_IO_NLONG_READ (pCtrl->data, (long *)pBuf, nWords >> 1);

	    pBuf     += nWords;
	    nSectors -= block;
	    }
	}

    ATA_DEBUG_MSG (2, "ataRW: end\n", 0, 0, 0, 0, 0, 0);

    return (OK);

errorRW:

    ATA_DEBUG_MSG (1, "ataRW err: stat=0x%x 0x%x semStatus=%d error=0x%x\n",
	           pCtrl->intStatus, ATA_IO_BYTE_READ (pCtrl->aStatus), 
                   semStatus, ATA_IO_BYTE_READ (pCtrl->error), 0, 0);

    if (++retryCount < ataRetry)
	goto retryRW;

    return (ERROR);
    }

/******************************* ATAPI Devices *********************************/

/*******************************************************************************
*
* ataDevIdentify - identify device
*
* This routine checks whether the device is connected to the controller, and if
* so determines its type.  The routine set `type' field in the corresponding 
* ATA_DRIVE structure.
* If device identification failed, the routine set `state' field in the 
* corresponding ATA_DRIVE structure to ATA_DEV_NONE.
*
* RETURNS: TRUE if a device is present, FALSE otherwise
*/

LOCAL STATUS ataDevIdentify
    (
    int	ctrl,
    int	dev
    )
    {
    ATA_CTRL    *pCtrl	= &ataCtrl[ctrl];
    ATA_DRIVE   *pDrive	= &pCtrl->drive [dev];
    ATA_PARAM   *pParam	= &pDrive->param;

    pDrive->type = ATA_TYPE_NONE;

    /* Select device */ 

    ATA_IO_BYTE_WRITE (pCtrl->sdh, ATA_SDH_IBM | (dev << 4));
 
    /* Wait for device selection */
 
    ATA_WAIT_STATUS;
 
    /* Clear semaphore: Selection of nonexistent device may rise an interrupt */

    semTake (&pCtrl->syncSem, NO_WAIT);

    ATA_IO_BYTE_WRITE (pCtrl->seccnt, 0xaa);
    ATA_IO_BYTE_WRITE (pCtrl->sector, 0x55);
 
    if (ATA_IO_BYTE_READ (pCtrl->seccnt) == 0xaa)
        {

        if (ataPiPread (ctrl, dev, pParam) == OK)
            {
            pDrive->type = ATA_TYPE_ATAPI;
            }
        else 

            if ( ((ATA_IO_BYTE_READ (ATAPI_STATUS) & (ATA_STAT_BUSY | 
		   ATA_STAT_WRTFLT | ATA_STAT_DRQ | ATA_STAT_ERR)) == 
                   ATA_STAT_ERR) &&
                  (ATA_IO_BYTE_READ (ATAPI_ERROR) == ERR_ABRT) )
                {

                if (ataPiWait (ctrl, ATA_STAT_READY, FALSE) == OK)
                    pDrive->type = ATA_TYPE_ATA;
                }
        }
 
    if (pDrive->type != ATA_TYPE_NONE)
        return (OK);

    ATA_DEBUG_MSG (1, "ataDevIdentify%d/%d: ERROR: status=0x%x dev=0x%x "
                   "error=0x%x\n", ctrl, dev, ATA_IO_BYTE_READ (pCtrl->status),
                   ATA_IO_BYTE_READ (pCtrl->sdh), 
                   ATA_IO_BYTE_READ (pCtrl->error), 0);

    /* Select present device */

    ATA_IO_BYTE_WRITE (pCtrl->sdh, ATA_SDH_IBM | (((~dev) & 0x1) << 4)); /* define macro for 0x1 */

    pDrive->state = ATA_DEV_NONE;

    return (ERROR);
    } /* ataDevIdentify */

/*******************************************************************************
*
* ataPiInit - init a ATAPI CD-ROM disk controller
*
* This routine resets a ATAPI CD-ROM disk controller.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ataPiInit 
    (
    int ctrl,
    int drive
    )
    {
    ATA_CTRL    *pCtrl = &ataCtrl[ctrl];
    int		retryCount = 0;
    int		i;

    ATA_DEBUG_MSG (2, "ataPiInit%d/%d: \n", ctrl, drive, 0, 0, 0, 0);

    while (TRUE) /* Forever */
        {
        ATA_IO_BYTE_WRITE (ATAPI_D_SELECT, DSEL_FILLER | (drive << 4));

        ATA_WAIT_STATUS;

        ATA_IO_BYTE_WRITE (ATAPI_COMMAND, ATA_PI_CMD_SRST);

        for (i = 0; i < 5000; i++)	/* 2 ms */
            {
            ATA_WAIT_STATUS;
            }

        ATA_IO_BYTE_WRITE (ATAPI_D_SELECT, DSEL_FILLER | (drive << 4));

        ATA_WAIT_STATUS;

        pCtrl->wdgOkay = TRUE;

        wdStart (pCtrl->wdgId, (sysClkRateGet() * pCtrl->wdgTimeout), 
                 (FUNCPTR)ataWdog, ctrl);

        while ( (ATA_IO_BYTE_READ (ATAPI_STATUS) & (ATA_STAT_BUSY | 
                 ATA_STAT_READY | ATA_STAT_WRTFLT | ATA_STAT_DRQ | 
                 ATA_STAT_ECCCOR | ATA_STAT_ERR)) && (pCtrl->wdgOkay) )
	    ;

        wdCancel (pCtrl->wdgId);

        if (pCtrl->wdgOkay)
            break;

        pCtrl->wdgOkay = TRUE;

        ATA_DEBUG_MSG (1, "ataPiInit%d/%d: ERROR: status=0x%x error=0x%x\n", 
                       ctrl, drive, ATA_IO_BYTE_READ(ATAPI_STATUS), 
                       ATA_IO_BYTE_READ(ATAPI_ERROR), 0, 0);

	if (++retryCount == ataRetry)
	    return (ERROR);
        }

    /* The following allow to recover after accidental interrupt */

    if (semTake (&pCtrl->syncSem, NO_WAIT) == OK)
        {
        ATA_DEBUG_MSG (2, "ataPiInit%d/%d: WARNING: interrupt cleared: "
                       "status=0x%x dev=0x%x error=0x%x\n", ctrl, drive, 
                       ATA_IO_BYTE_READ(ATAPI_STATUS), 
                       ATA_IO_BYTE_READ(ATAPI_D_SELECT), 
                       ATA_IO_BYTE_READ(ATAPI_ERROR), 0);
        }
    else
        {
        ATA_DEBUG_MSG (2, "ataPiInit%d/%d: Ok: status=0x%x dev=0x%x error=0x%x"
                       "\n", ctrl, drive, ATA_IO_BYTE_READ(ATAPI_STATUS), 
                       ATA_IO_BYTE_READ(ATAPI_D_SELECT), 
                       ATA_IO_BYTE_READ(ATAPI_ERROR), 0);
        }

    return (OK);
    } /* ataPiInit */

/*******************************************************************************
*
* ataPiPread - Read ATAPI drive parameters
*
* This routine reads drive parameters.
*
* RETURNS: OK, ERROR if the command didn't succeed.
*/

LOCAL STATUS ataPiPread
    (
    int		ctrl,
    int		drive,
    void        *buffer
    )
    {
    ATA_CTRL    *pCtrl		= &ataCtrl[ctrl];
    int		status;
    int		error;
    int		semStatus;

    ATA_DEBUG_MSG (2, "ataPiPread%d/%d: \n", ctrl, drive, 0, 0, 0, 0);

    ataPiWait (ctrl, ATA_STAT_ACCESS, TRUE);

    /* Select device */

    ATA_IO_BYTE_WRITE (ATAPI_D_SELECT, DSEL_FILLER | (drive << 4));

    ATA_WAIT_STATUS;

    if (ataPiWait (ctrl, ATA_STAT_ACCESS, TRUE) != OK)
        {
        ATA_IO_BYTE_WRITE (ATAPI_D_SELECT, DSEL_FILLER | (drive << 4));;
        ATA_WAIT_STATUS;
        }

    /* ATAPI Identify Device Command */

    ATA_IO_BYTE_WRITE (ATAPI_COMMAND, ATA_PI_CMD_IDENTD);

    ATA_WAIT_STATUS;

    /*
     * Poll Status Register instead of wait for interrupt to avoid needless 
     * delay.
     */

    ataPiWait (ctrl, ATA_STAT_BUSY, FALSE);

    semStatus = semTake (&pCtrl->syncSem, 1);

    status = ATA_IO_BYTE_READ (ATAPI_STATUS);

    if ( (status & ATA_STAT_ERR) || !(status & ATA_STAT_DRQ) || 
         (semStatus != OK) )
        {
        error = ATA_IO_BYTE_READ (ATAPI_ERROR);

        ATA_DEBUG_MSG (1, "ataPiPread%d/%d: ERROR: status=0x%x intStatus=0x%x "
                      "error=0x%x semStatus=%d\n", ctrl, drive, status, 
                       pCtrl->intStatus, error, semStatus);

        return (ERROR);
        }

    /* Receive parameter information from the drive */

    ATA_IO_NWORD_READ_SWAP (ATAPI_DATA, (short *)buffer, 
                            sizeof(ATA_PARAM)/sizeof(short));

    /* To prevent from reading status before it is valid */

    ATA_IO_BYTE_READ (pCtrl->aStatus);

    /* Wait for device to be ready !!! */

    ataPiWait (ctrl, ATA_STAT_READY, TRUE);

    ATA_DEBUG_MSG (2, "ataPiPread%d/%d:Ok: status=0x%x devReg=0x%x\n", 
                  ctrl, drive, ATA_IO_BYTE_READ (pCtrl->status), 
                  ATA_IO_BYTE_READ (pCtrl->sdh), 0, 0);

    return (OK);
    } /* ataPiPread */

/*******************************************************************************
*
* atapiDMAInit - initialize the DMA engine
*
* This routine initializes the DMA engine.
*
* RETURN: OK, or ERROR
*/

LOCAL STATUS atapiDMAInit (void)
    {
    printErr("ATAPI (DMA Init.) ERROR: DMA transfer is not emplemented.\n");
    return ERROR;
    }

/*******************************************************************************
*
* atapiDMATransfer - transfer a single data packet via DMA
*
* This routine transfers a single data packet via DMA.
*
* RETURN: OK, or ERROR
*
* SEE ALSO: ataPiPIOTransfer()
*/

LOCAL STATUS atapiDMATransfer 
    (
    ATA_DEV *pAtapiDev
    )
    {
    pAtapiDev->errNum = 27;
    return ERROR;
    }

/*******************************************************************************
*
* ataPiOverlapTransferLoop - loop for DRQ Interrupts with Overlapping
*
* This routine loops for Data Request Interrupts until all data packets are 
* transferred.  It is invoked when ataPiPktCmd() executes an Overlapped command.  
* When Device executes an Overlapped command, it releases the ATA bus until the
* device is ready to transfer a data or to present the completion status.
*
* RETURN: OK, or ERROR
*
* SEE ALSO: ataPiNonOverTransferLoop()
*/

LOCAL STATUS ataPiOverlapTransferLoop
    (
    ATA_DEV *pAtapiDev,
    FUNCPTR pTransferProc
    )
    {
    pAtapiDev->errNum = 26;
    return ERROR;
    }

/*******************************************************************************
*
* ataPiPIOTransfer - transfer a single data packet via PIO
*
* This routine transfers a single data packet via PIO.
*
* RETURN: OK, or ERROR
*
* SEE ALSO: atapiDMATransfer()
*/

LOCAL STATUS ataPiPIOTransfer
    (
    ATA_DEV     *pAtapiDev
    )
    {
    ATA_CTRL    *pCtrl	= &ataCtrl[pAtapiDev->ctrl];
    uint32_t	desiredSize;
    uint32_t	remainderSize;
    short	bucket;

    /* Read Byte Count */

    pAtapiDev->transSize = ((uint16_t)ATA_IO_BYTE_READ (ATAPI_BCOUNT_HI)) << 8;
    pAtapiDev->transSize += ATA_IO_BYTE_READ (ATAPI_BCOUNT_LO);

    /*
     * From ATA Packet Interface for CD-ROMs SFF-8020i:
     * If the Device requests more data be transfered than required by the 
     * command protocol, the Host shall pad when sending data to the Device, 
     * and dump extra data into a bit bucket when reading data from the Device.
     *
     * Why device will request more data than required? It is not clear.
     * (May be some devices can't transfer part of the internal buffer?) !!!
     */

    if ( (pAtapiDev->pBufEnd == NULL) && (pAtapiDev->transSize != 0) )
        {
        pAtapiDev->errNum = 8;
        return ERROR;
        }
    desiredSize = pAtapiDev->pBufEnd - pAtapiDev->pBuf;

    if (desiredSize <= pAtapiDev->transSize)
        {
        remainderSize = pAtapiDev->transSize - desiredSize;
        pAtapiDev->pBufEnd = NULL;
        }
    else
        {
        desiredSize = pAtapiDev->transSize;
        remainderSize = 0;
        }

    /* Transfer Data Bytes */

    switch (pAtapiDev->direction)
        {
        case OUT_DATA:

            if((pAtapiDev->intReason & INTR_IO) != 0)	/* IO cleared */
                    goto errorPIOTransfer;

            ATA_IO_NWORD_WRITE (ATAPI_DATA, (short *)pAtapiDev->pBuf, 
                                desiredSize / 2);

            while (remainderSize)
                {
                bucket = 0;
                ATA_IO_NWORD_WRITE (ATAPI_DATA, &bucket, 1);
                remainderSize -= 2;
                }

            break;

        case IN_DATA:

            if((pAtapiDev->intReason & INTR_IO) == 0)	/* IO set */
                    goto errorPIOTransfer;
            ATA_IO_NWORD_READ (ATAPI_DATA, (short *)pAtapiDev->pBuf, 
                               desiredSize / 2);

            while (remainderSize)
                {
                ATA_IO_NWORD_READ (ATAPI_DATA, &bucket, 1);
                remainderSize -= 2;
                }

            break;

        default:

            goto errorPIOTransfer;

        }

    pAtapiDev->pBuf += pAtapiDev->transSize;

    return OK;

errorPIOTransfer:

    pAtapiDev->errNum = 10;

    return ERROR;
    }

/*******************************************************************************
*
* ataPiNonOverTransferLoop - loop for DRQ Interrupts without Overlapping
*
* This routine loops for Data Request Interrupts until all data packets are 
* transferred.  It is invoked when ataPiPktCmd() executes a NON Overlapped command.
*
* RETURN: OK, or ERROR 
*
* SEE ALSO: ataPiOverlapTransferLoop()
*/

LOCAL STATUS ataPiNonOverTransferLoop
    (
    ATA_DEV  *pAtapiDev,
    FUNCPTR  pTransferProc
    )
    {
    ATA_CTRL *pCtrl	= &ataCtrl[pAtapiDev->ctrl];
    int	     semStatus;

    while ( TRUE )
        {
        /* Wait for INTRQ */

        semStatus = semTake (&pCtrl->syncSem, sysClkRateGet() * 
                                              pCtrl->semTimeout);
        if ((semStatus == ERROR))
            {
            pAtapiDev->errNum = 4;
            return ERROR;
            }

        /* Note: ideIntr() reads Status to negate INTRQ */

        pAtapiDev->status = pCtrl->intStatus;
        pAtapiDev->intReason = ATA_IO_BYTE_READ (ATAPI_INTREASON);

        if ( (pAtapiDev->status & ATA_STAT_DRQ) == 0)
            break; /* Command terminated */

        if (pAtapiDev->direction == NON_DATA)	   	/* non data command */
            {
            pAtapiDev->errNum = 5;

            return ERROR;
            }

        if ( ((pAtapiDev->intReason & INTR_COD) != 0) ||/* CoD cleared */
             ((pAtapiDev->status & ATA_STAT_BUSY) != 0)	/* BUSY cleared */
           )
            {
            pAtapiDev->errNum = 6;

            return ERROR;
            }

        /* Transfer single data packet */

        if (pTransferProc(pAtapiDev) != OK)
            return ERROR;

        pAtapiDev->transCount++;
        } /* while ( TRUE ) */

    if ( ((pAtapiDev->intReason & INTR_COD) == 0)    ||	/* CoD set */
         ((pAtapiDev->intReason & INTR_IO) == 0)     ||	/* IO set */
         ((pAtapiDev->status & ATA_STAT_READY) == 0) ||	/* DRDY set */
         ((pAtapiDev->status & ATA_STAT_BUSY) != 0)	/* BUSY cleared */
       )
        {
        pAtapiDev->errNum = 7;

        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* ataPiPktCmdExec - execute an ATAPI command without error processing
*
* This routine executes a single ATAPI command without checking of completion
* status, and attempts to recover.
* An invoking routine is responsible for error processing.
*
* RETURN: OK, or ERROR if an error is encountered during data transfer.
*
* ERRNO:
*  S_ioLib_DEVICE_ERROR
*/

LOCAL STATUS ataPiPktCmdExec
    (
    ATA_DEV   *pAtapiDev,
    ATAPI_CMD *pComPack
    )
    {
    int	      device	= pAtapiDev->drive;
    ATA_CTRL  *pCtrl	= &ataCtrl[pAtapiDev->ctrl];
    ATA_DRIVE *pDrive	= &pCtrl->drive[device];
    ATA_PARAM *pParam	= &pDrive->param;
    int	      semStatus;
    FUNCPTR   pTransferProc = (FUNCPTR)ataPiPIOTransfer;
    FUNCPTR   pTransferLoopProc = (FUNCPTR)ataPiNonOverTransferLoop;
    UCHAR     featers = 0; /* Features Register */
    uint32_t  desiredTransferSize = pComPack->desiredTransferSize;

    ATA_DEBUG_MSG (2, "ataPiPktCmdExec%d/%d: com=0x%x\n", pAtapiDev->ctrl, 
                   device, pComPack->cmdPkt[0], 0, 0, 0);

    /* Initialize transfer control structure */

    if (NULL != pComPack->ppBuf)
         {
         pAtapiDev->pBuf	  = *(pComPack->ppBuf);
	 pAtapiDev->pBufEnd	  = *(pComPack->ppBuf) + pComPack->bufLength;
         }
    else
         {
         pAtapiDev->pBuf	  = NULL;
         pAtapiDev->pBufEnd	  = NULL;
         }

    pAtapiDev->direction  = pComPack->direction;
    pAtapiDev->transCount = 0;
    pAtapiDev->errNum	  = 0;
    pAtapiDev->intReason  = 0;
    pAtapiDev->status	  = 0;
    pAtapiDev->transSize  = 0;

    if (pComPack->dma)
        if (pParam->capabilities & CAPABIL_DMA)
            if (atapiDMAInit() == OK) /* Initialize the DMA engine */
                {
                featers |= FEAT_DMA;
                pTransferProc = (FUNCPTR)atapiDMATransfer;
                }

    if (pComPack->overlap)
        if (pParam->capabilities & CAPABIL_OVERLAP)
            {
            featers |= FEAT_OVERLAP;
            pTransferLoopProc = (FUNCPTR)ataPiOverlapTransferLoop;
            }

    ataPiWait (pAtapiDev->ctrl, ATA_STAT_ACCESS, TRUE);

    /* Select device */

    ATA_IO_BYTE_WRITE (ATAPI_D_SELECT, DSEL_FILLER | (device << 4));

    ATA_WAIT_STATUS;

    if (ataPiWait (pAtapiDev->ctrl, ATA_STAT_ACCESS, TRUE) != OK)
        {
        ATA_IO_BYTE_WRITE (ATAPI_D_SELECT, DSEL_FILLER | (device << 4));;
        ATA_WAIT_STATUS;
        }

    /***** Issue `ATAPI Packet Command' *****/

    /* Initialize Task File */

    ATA_IO_BYTE_WRITE (ATAPI_FEATURE, featers);
    ATA_IO_BYTE_WRITE (ATAPI_BCOUNT_LO, (uint8_t)desiredTransferSize);
    ATA_IO_BYTE_WRITE (ATAPI_BCOUNT_HI, (uint8_t)(desiredTransferSize >> 8));

    /* Wait for BSY=0 & DRQ=0 */

    while ( (ATA_IO_BYTE_READ (ATAPI_STATUS) & (ATA_STAT_BUSY | ATA_STAT_DRQ)) 
            != 0 )
        ;

    /* Issue Packet Command */

    ATA_IO_BYTE_WRITE (ATAPI_COMMAND, ATA_PI_CMD_PKTCMD);

    /***** Transfer the Command Packet In to Host *****/

    /* Wait for DRQ or INTRQ */

    switch (pParam->config & CONFIG_PKT_TYPE) /* CMD DRQ Type */
        {
        case CONFIG_PKT_TYPE_INTER: /* Interrupt DRQ */
            /* Wait for INTRQ */

            semStatus = semTake (&pCtrl->syncSem, sysClkRateGet() * 
                                                  pCtrl->semTimeout);
            if ((semStatus == ERROR))
                {
                pAtapiDev->errNum = 1;

                goto errPktCmdExec;
                }

            /* Note: ideIntr() read Status to negate INTRQ */

            pAtapiDev->status = pCtrl->intStatus;

            if ( ((pAtapiDev->status & ATA_STAT_DRQ) == 0) || /* DRQ set */
                 ((pAtapiDev->status & ATA_STAT_BUSY) != 0)   /* BUSY cleared */
                )
                {
                pAtapiDev->errNum = 2;

                goto errPktCmdExec;
                }
            break;

        case CONFIG_PKT_TYPE_MICRO: /* Microprocessor DRQ */
        case CONFIG_PKT_TYPE_ACCEL: /* Accelerated DRQ */

            while ( ((pAtapiDev->status = ATA_IO_BYTE_READ (ATAPI_STATUS)) & 
                     (ATA_STAT_DRQ | ATA_STAT_BUSY)) != ATA_STAT_DRQ )
                ;			 /* DRQ set, BUSY cleared */

        }
 
    pAtapiDev->intReason = ATA_IO_BYTE_READ (ATAPI_INTREASON);

    if ( ((pAtapiDev->intReason & INTR_COD) == 0) || /* CoD set */
         ((pAtapiDev->intReason & INTR_IO) != 0)     /* IO cleared */
        )
        {
        pAtapiDev->errNum = 3;

        goto errPktCmdExec;
        }

    /* Write Command Packet Bytes (6 words of Command) */

    ATA_IO_NWORD_WRITE (ATAPI_DATA, (short *)pComPack->cmdPkt, 6);

    /***** Transfer the Data *****/

    if (pTransferLoopProc(pAtapiDev, pTransferProc) == OK)
        {
        ATA_DEBUG_MSG (2, "ataPiPktCmdExec%d/%d: Ok: bufStart=%p buf=%p n=%d "
                       "com=0x%x\n", pAtapiDev->ctrl, device, 
                       *(pComPack->ppBuf), pAtapiDev->pBuf, 
                       pAtapiDev->transCount, pComPack->cmdPkt[0]);


        if (NULL != pComPack->ppBuf)
            {
            if (pAtapiDev->pBufEnd == NULL)
                *(pComPack->ppBuf) += pComPack->bufLength;
            else
                *(pComPack->ppBuf) = pAtapiDev->pBuf;
            }
        return (OK);
        }

errPktCmdExec:

    ATA_DEBUG_MSG (1, "ataPiPktCmdExec%d/%d: ERROR: %s: status=0x%x "
                   "dev=0x%x error=0x%x\n", pAtapiDev->ctrl, device, 
                   ataErrStrs[pAtapiDev->errNum], pAtapiDev->status, 
                   ATA_IO_BYTE_READ(ATAPI_D_SELECT), 
                   ATA_IO_BYTE_READ(ATAPI_ERROR));
    ATA_DEBUG_MSG (1, "bufStart=%p buf=%p n=%d com=0x%x intReason=0x%x "
                   "byteCount=0x%x\n", *(pComPack->ppBuf), pAtapiDev->pBuf, 
                   pAtapiDev->transCount, pComPack->cmdPkt[0], 
                   pAtapiDev->intReason, pAtapiDev->transSize);

    return (ERROR);
    } /* ataPiPktCmdExec */

/*******************************************************************************
*
* ataPiReqSense - issue a REQUEST_SENSE command to a device and read the results
*
* This routine issues a REQUEST_SENSE command to a specified ATAPI device and
* read the results.
*
* RETURNS: OK, or ERROR if the command fails.
*/

LOCAL STATUS ataPiReqSense
    (
    ATA_DEV     *pAtapiDev
    )
    {
#ifdef ATA_DEBUG
    ATA_CTRL    *pCtrl	   = &ataCtrl[pAtapiDev->ctrl];
#endif
    ATAPI_CMD	atapiCmd;
    char	reqSenseData [18];
    char        *pBuf = (char *)reqSenseData;
    int 	i;
    int senseKey;
    int senseCode;

    ATA_DEBUG_MSG (2, "ataPiReqSense%d/%d: \n", pAtapiDev->ctrl, 
                   pAtapiDev->drive, 0, 0, 0, 0);

    /* build a REQUEST SENSE command and execute it */

    atapiCmd.cmdPkt[0] = CDROM_CMD_REQUEST_SENSE;

    for (i = 1; i < 4; i++)
        atapiCmd.cmdPkt[i] = 0;

    atapiCmd.cmdPkt[4] = 18;

    for (i = 5; i < 12; i++)
        atapiCmd.cmdPkt[i] = 0;

    atapiCmd.ppBuf		 = &pBuf;
    atapiCmd.bufLength		 = 18;
    atapiCmd.direction		 = IN_DATA;
    atapiCmd.desiredTransferSize = 18;
    atapiCmd.dma		 = FALSE;
    atapiCmd.overlap		 = FALSE;

    if (ataPiPktCmdExec (pAtapiDev, &atapiCmd) != OK)
        return (ERROR);

    /* REQUEST SENSE command status != GOOD indicates fatal error */

    if ( (pAtapiDev->status & ATA_STAT_ERR) != 0 )
        {
        ATA_DEBUG_MSG (1, "ataPiReqSense%d/%d: ERROR: status=0x%x dev=0x%x "
                       "error=0x%x\n", pAtapiDev->ctrl, pAtapiDev->drive, 
                       pAtapiDev->status, ATA_IO_BYTE_READ(ATAPI_D_SELECT), 
                       ATA_IO_BYTE_READ(ATAPI_ERROR), 0);

        return (ERROR);
        }

    bzero (pBuf, 18 - (pBuf - reqSenseData));

    senseKey  = reqSenseData[2] & 0x0f;
    senseCode = reqSenseData[12];

    ATA_DEBUG_MSG (1, "ataPiReqSense%d/%d: Ok: senseKey=0x%x addSenseCode=0x%x"
                   "\n", pAtapiDev->ctrl, pAtapiDev->drive, 
                   (reqSenseData[2] & 0x0f), reqSenseData[12], 0, 0);

    return (OK);
    } /* ataPiReqSense */

/*******************************************************************************
*
* ataPiPktCmd - execute an ATAPI command with error processing
*
* This routine executes a single ATAPI command, checks the command completion 
* status and tries to recover if an error encountered during command execution 
* at any stage.
*
* RETURN: OK, or ERROR if not successful for any reason.
*
* ERRNO:
*  S_ioLib_DEVICE_ERROR
*/

LOCAL STATUS ataPiPktCmd
    (
    ATA_DEV     *pAtapiDev,
    ATAPI_CMD   *pComPack
    )
    {
    int		device	   = pAtapiDev->drive;
    ATA_CTRL    *pCtrl	   = &ataCtrl[pAtapiDev->ctrl];
    ATA_DRIVE   *pDrive	   = &pCtrl->drive[device];
    UCHAR	error	   = 0; /* Error Register */
    int		retryCount = 0;

    if (pComPack->bufLength != 0)
        {

        if (pComPack->desiredTransferSize == 0)
            return (ERROR);

        if (pComPack->desiredTransferSize > 65534)
            pComPack->desiredTransferSize = 0xfffe;
				/* ATA-4 Rev.18: 0xffff is interpreted by device
                                 * as though it where 0xfffe */
        else

            if (pComPack->desiredTransferSize < pComPack->bufLength)
                pComPack->desiredTransferSize &= 0xfffffffe;
				/* ATA-4 Rev.18: shall be even */
        }
    else
        {
        pComPack->dma = FALSE;
        pComPack->desiredTransferSize = 0;
        }

    semTake (&pCtrl->muteSem, WAIT_FOREVER);

retryPktCmd:

    error = 0;

    if (ataPiPktCmdExec (pAtapiDev, pComPack) == OK)
        {

        /* Check the Completion Status */

        if ( (pAtapiDev->status & ATA_STAT_ERR) == 0 )
            goto okPktCmd;

        /* If Error, Read Error Register */

        error = ATA_IO_BYTE_READ (ATAPI_ERROR);

        switch (error & ERR_SENSE_KEY)
            {
            case SENSE_NO_SENSE:
                pAtapiDev->errNum = 11;
                break;

            case SENSE_RECOVERED_ERROR:

                if (pAtapiDev->pBuf < pAtapiDev->pBufEnd)
                    {	/* not medium access commands */
                    pAtapiDev->errNum = 12;
                    break;
                    }
                else
		    /*
                     * Mode parameters can specify transfer disabling on 
                     * recovered error !!!) 
                     */

                    goto okPktCmd;

            case SENSE_NOT_READY:
                pAtapiDev->errNum = 13;
                break;

            case SENSE_MEDIUM_ERROR:
                pAtapiDev->errNum = 14;
                break;

            case SENSE_HARDWARE_ERROR:
                pAtapiDev->errNum = 15;
                break;

            case SENSE_ILLEGAL_REQUEST:
                pAtapiDev->errNum = 16;
                break;

            case SENSE_UNIT_ATTENTION:
                /*
                 * Medium may have changed, or Power on or Reset occured, or
                 * Mode parameter changed
                 */

                pAtapiDev->errNum = 17;
                break;

            case SENSE_DATA_PROTECT:
                pAtapiDev->errNum = 18;
                break;

            case SENSE_ABBORTED_COMMAND:
                pAtapiDev->errNum = 19;
                break;

            case SENSE_MISCOMPARE:
                pAtapiDev->errNum = 20;
                break;

            default:
                pAtapiDev->errNum = 0;
                break;

            }

        ATA_DEBUG_MSG (1, "ataPiPktCmd%d/%d: ERROR: %s: status=0x%x dev=0x%x "
                       "error=0x%x\n", pAtapiDev->ctrl, device, 
                       ataErrStrs[pAtapiDev->errNum], pAtapiDev->status, 
                       ATA_IO_BYTE_READ(ATAPI_D_SELECT), error);
        }

    /*
     * Execute a request sense command to try to find out more about what 
     * went wrong (and clear a unit attention)? 
     */

    if (error != 0)
        {
        /* Test for transition from NOT READY to READY */

        if ((error & ERR_SENSE_KEY) == SENSE_UNIT_ATTENTION)
            {
            pDrive->state = ATA_DEV_MED_CH;
            pAtapiDev->blkDev.bd_readyChanged = TRUE;
            }

        else if ( ataPiReqSense(pAtapiDev) == OK )
	    {

	    if (++retryCount < ataRetry)
		{
                pDrive->state = ATA_DEV_OK;
                pAtapiDev->blkDev.bd_readyChanged = FALSE;
                pAtapiDev->errNum = 0;
		error = 0;

		goto retryPktCmd;
		}
            }
        }
    else
        error = -1;

    if ( (error & ERR_SENSE_KEY) != SENSE_UNIT_ATTENTION )
        {
        ataPiInit (pAtapiDev->ctrl, device);

        if (++retryCount < ataRetry)
            goto retryPktCmd;

        }

    semGive (&pCtrl->muteSem);

    (void)errnoSet (S_ioLib_DEVICE_ERROR);

    return error;

okPktCmd:

    semGive (&pCtrl->muteSem);

    return (SENSE_NO_SENSE); 
    } /* ataPiPktCmd */

/*******************************************************************************
*
* ataPiTestUnitRdy - issue a TEST UNIT READY command to a ATAPI device
*
* This routine issues a TEST UNIT READY command to a specified ATAPI device.
*
* RETURNS: OK, or ERROR if the command fails.
*/
 
LOCAL STATUS ataPiTestUnitRdy
    (
    ATA_DEV     *pAtapiDev
    )
    {
    ATAPI_CMD	atapiCmd;
    int 	i;
    int		error;

    ATA_DEBUG_MSG (2, "ataPiTestUnitRdy%d/%d: \n", pAtapiDev->ctrl, 
                   pAtapiDev->drive, 0, 0, 0, 0);

    /* build a TEST UNIT READY command and execute it */

    atapiCmd.cmdPkt[0] = CDROM_CMD_TEST_UNIT_READY;

    for (i = 1; i < 12; i++)
        atapiCmd.cmdPkt[i] = 0;

    atapiCmd.ppBuf		 = NULL;
    atapiCmd.bufLength		 = 0;
    atapiCmd.direction		 = NON_DATA;
    atapiCmd.desiredTransferSize = 0;
    atapiCmd.dma		 = FALSE;
    atapiCmd.overlap		 = FALSE;

    i = 0;

    do  {
        error = ataPiPktCmd (pAtapiDev, &atapiCmd) & ERR_SENSE_KEY;

        if ( ((error != SENSE_NO_SENSE) && (error != SENSE_UNIT_ATTENTION)) ||
             (++i > 5) )	/* drive may queue unit attention conditions */
            {
            return (ERROR);	/* problem with the device is detected */
            }

        } while (error != SENSE_NO_SENSE);

    ATA_DEBUG_MSG (2, "ataPiTestUnitRdy%d/%d: Ok\n", pAtapiDev->ctrl, 
                   pAtapiDev->drive, 0, 0, 0, 0);

    return (OK);
    } /* ataPiTestUnitRdy */

/*******************************************************************************
*
* ataPiReadCapacity - issue a READ_CAPACITY command to a ATAPI device
*
* This routine issues a READ_CAPACITY command to a specified ATAPI device.
*
* RETURN: OK, or ERROR if the command fails.
*/

LOCAL STATUS ataPiReadCapacity
    (
    ATA_DEV     *pAtapiDev
    )
    {
    BLK_DEV     *pBlkDev = &pAtapiDev->blkDev;
    ATAPI_CMD	atapiCmd;
    UINT8	resultBuf[8];
    char        *pBuf = (char *)resultBuf;
    int		i;

    ATA_DEBUG_MSG (2, "ataPiReadCapacity%d/%d: \n", pAtapiDev->ctrl, 
                   pAtapiDev->drive, 0, 0, 0, 0);

    /* build a READ CD-ROM CAPACITY command and execute it */

    atapiCmd.cmdPkt[0] = CDROM_CMD_READ_CDROM_CAP;

    for (i = 1; i < 12; i++)
        atapiCmd.cmdPkt[i] = 0;

    atapiCmd.ppBuf		 = &pBuf;
    atapiCmd.bufLength		 = 8;
    atapiCmd.direction		 = IN_DATA;
    atapiCmd.desiredTransferSize = 8;
    atapiCmd.dma		 = FALSE;
    atapiCmd.overlap		 = FALSE;

    if (ataPiPktCmd (pAtapiDev, &atapiCmd) != SENSE_NO_SENSE)
        return (ERROR);

    pBlkDev->bd_nBlocks = 0;
    pBlkDev->bd_bytesPerBlk = 0;

    for (i = 0; i < 4; i++)
        pBlkDev->bd_nBlocks += ((ULONG)resultBuf[i]) << (8 * (3 - i));

    pBlkDev->bd_nBlocks++;                 /* ULONG blocks on device */
    pBlkDev->bd_blksPerTrack = pBlkDev->bd_nBlocks; 
                                           /* ULONG blocks per track */
    for (i = 4; i < 8; i++)
        pBlkDev->bd_bytesPerBlk += ((ULONG)resultBuf[i]) << (8 * (7 - i));
                                           /* ULONG bytes per block */

    ATA_DEBUG_MSG (2, "ataPiReadCapacity%d/%d: Ok: nBlocks=%d bytesPerBlk=%d\n",
                   pAtapiDev->ctrl, pAtapiDev->drive, pBlkDev->bd_nBlocks, 
                   pBlkDev->bd_bytesPerBlk, 0, 0);

    return (OK);
    } /* ataPiReadCapacity */

/*******************************************************************************
*
* ataPiBlkRd - read one or more blocks from a ATAPI CD-ROM disk
*
* This routine reads one or more blocks from the specified device,
* starting with the specified block number.
*
* RETURNS: OK, ERROR if the read command didn't succeed.
*/

LOCAL STATUS ataPiBlkRd
    (
    ATA_DEV     *pAtapiDev,
    int         startBlk,
    int         nBlks,
    char        *pBuf
    )
    {
    ATA_CTRL    *pCtrl	= &ataCtrl[pAtapiDev->ctrl];
    ATA_DRIVE   *pDrive	= &pCtrl->drive[pAtapiDev->drive];
    ATAPI_CMD	atapiCmd;
    int		i;

    char *pUpdatedBuf = pBuf;

    ATA_DEBUG_MSG (2, "ataPiBlkRd%d/%d: state=0x%x startBlk=%d nBlks=%d pBuf=%p"
                   "\n", pAtapiDev->ctrl, pAtapiDev->drive, pDrive->state, 
                   startBlk, nBlks, pBuf);

    if (!pCtrl->installed)
        return (ERROR);

    if (pDrive->state != ATA_DEV_OK)
        return (ERROR);

    /* Sanity check */

    if ( (startBlk >= pAtapiDev->blkDev.bd_nBlocks) || 
         ((startBlk + nBlks) >= pAtapiDev->blkDev.bd_nBlocks) )
        {
        ATA_DEBUG_MSG (1, "ataPiBlkRd%d/%d ERROR: block numbers must be in "
                       "range 0 - %d\n", pAtapiDev->ctrl, pAtapiDev->drive, 
                       (pAtapiDev->blkDev.bd_nBlocks - 1), 0, 0, 0);

        return ERROR;
        }

    /* Fill Command Packet */

    atapiCmd.cmdPkt[0] = CDROM_CMD_READ_12;
    atapiCmd.cmdPkt[1] = 0; /* Reserved */

    /* clear bytes 2 - 9 */

    *(u_char *)(&atapiCmd.cmdPkt[2]) = 0; 
    *(u_char *)(&atapiCmd.cmdPkt[3]) = 0;
    *(u_char *)(&atapiCmd.cmdPkt[4]) = 0;
    *(u_char *)(&atapiCmd.cmdPkt[5]) = 0;
    *(u_char *)(&atapiCmd.cmdPkt[6]) = 0; 
    *(u_char *)(&atapiCmd.cmdPkt[7]) = 0; 
    *(u_char *)(&atapiCmd.cmdPkt[8]) = 0; 
    *(u_char *)(&atapiCmd.cmdPkt[9]) = 0; 

    for (i = 2; i < 6; i++)
        atapiCmd.cmdPkt[i] += (UINT8)( startBlk >> (8 * (5 - i)) );

    for (i = 6; i < 10; i++)
        atapiCmd.cmdPkt[i] += (UINT8)( nBlks >> (8 * (9 - i)) );

    atapiCmd.cmdPkt[10] = 0; /* Reserved */
    atapiCmd.cmdPkt[11] = 0; /* Reserved */

    i = nBlks * pAtapiDev->blkDev.bd_bytesPerBlk;

    atapiCmd.ppBuf		 = &pUpdatedBuf;
    atapiCmd.bufLength		 = i;
    atapiCmd.direction		 = IN_DATA;
    atapiCmd.desiredTransferSize = i;
    atapiCmd.dma		 = FALSE;
    atapiCmd.overlap		 = FALSE;

    /* Execute Packet Command */

    if (ataPiPktCmd (pAtapiDev, &atapiCmd) != SENSE_NO_SENSE)
        return (ERROR);

    ATA_DEBUG_MSG (2, "ataPiBlkRd%d/%d: Ok\n", pAtapiDev->ctrl, 
                   pAtapiDev->drive, 0, 0, 0, 0);

    return (OK);
    } /* ataPiBlkRd */

/*******************************************************************************
*
* ataPiReset - reset a ATAPI CD-ROM disk controller
*
* This routine resets a ATAPI CD-ROM disk controller.
*
* RETURNS: OK, or ERROR.
*/

LOCAL STATUS ataPiReset
    (
    ATA_DEV   *pAtapiDev
    )
    {
    ATA_CTRL  *pCtrl	= &ataCtrl[pAtapiDev->ctrl];
    ATA_DRIVE *pDrive	= &pCtrl->drive[pAtapiDev->drive];

    ATA_DEBUG_MSG (2, "ataPiReset%d/%d: state=0x%x\n", pAtapiDev->ctrl, 
                   pAtapiDev->drive, pDrive->state, 0, 0, 0);


    if (!pCtrl->installed)
        return (ERROR);

    if (pDrive->state != ATA_DEV_MED_CH)
        {	/* power on, entire drive changed, or failure recovering */
        semTake (&pCtrl->muteSem, WAIT_FOREVER);

        if (pDrive->state != ATA_DEV_INIT)
            {	/* higher level tried to recover after failure */

            if (pDrive->Reset (pAtapiDev->ctrl, pAtapiDev->drive) != OK)
                {
                semGive (&pCtrl->muteSem);
                return (ERROR);
                }
           }

        if (ataDriveInit (pAtapiDev->ctrl, pAtapiDev->drive) != OK)
            {
            semGive (&pCtrl->muteSem);
            return (ERROR);
            }

        semGive (&pCtrl->muteSem);
        }

    /* Identify medium */

    if (ataPiTestUnitRdy (pAtapiDev) != OK)
        return (ERROR);

    if (ataPiReadCapacity (pAtapiDev) != OK)
        {
        pAtapiDev->blkDev.bd_readyChanged = TRUE;
        return (ERROR);
        }
    else
        pDrive->state = ATA_DEV_OK;

    ATA_DEBUG_MSG (2, "ataPiReset%d/%d: Ok\n", pAtapiDev->ctrl, 
                   pAtapiDev->drive, 0, 0, 0, 0);

    return (OK);
    } /* ataPiReset */

/*******************************************************************************
*
* ataPiIoctl - do device specific control function
*
* This routine is called when the file system cannot handle an ioctl()
* function.
*
* RETURNS:  OK or ERROR.
*/

LOCAL STATUS ataPiIoctl
    (
    ATA_DEV     *pAtapiDev,
    int         function,
    int         arg
    )
    {
    ATA_CTRL    *pCtrl	= &ataCtrl[pAtapiDev->ctrl];
    ATA_DRIVE   *pDrive	= &pCtrl->drive[pAtapiDev->drive];
    int		status = ERROR;

    ATA_DEBUG_MSG (2, "ataPiIoctl%d/%d: state=0x%x func=%d arg=%d\n", 
                   pAtapiDev->ctrl, pAtapiDev->drive, pDrive->state, function, 
                   arg, 0);

    if (!pCtrl->installed)
        return (ERROR);

    if (pDrive->state != ATA_DEV_OK)
        return (ERROR);

    ATA_DEBUG_MSG (2, "ataPiIoctl%d/%d: Ok\n", pAtapiDev->ctrl, 
                   pAtapiDev->drive, 0, 0, 0, 0);

    return status;
    } /* ataPiIoctl */

/*******************************************************************************
*
* ataPiStatusChk - check device status
*
* This routine issues a TEST UNIT READY command to a ATAPI device to detect a
* medium change.  It called by filesystems before doing open()'s or creat()'s.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS ataPiStatusChk
    (
    ATA_DEV   *pAtapiDev	/* pointer to device descriptor */
    )
    {
    ATA_CTRL  *pCtrl	= &ataCtrl[pAtapiDev->ctrl];
    ATA_DRIVE *pDrive	= &pCtrl->drive[pAtapiDev->drive];

    ATA_DEBUG_MSG (2, "ataPiStatusChk%d/%d: state=%d readyChanged=%d\n", 
                   pAtapiDev->ctrl, pAtapiDev->drive, pDrive->state, 
                   pAtapiDev->blkDev.bd_readyChanged, 0, 0);

    if (!pCtrl->installed)
        return (ERROR);

    if ( (pDrive->state != ATA_DEV_MED_CH) && (pDrive->state != ATA_DEV_OK)  )
        return (ERROR);

    if (ataPiTestUnitRdy (pAtapiDev) != OK)
        return (ERROR);

    /* Device OK, medium may be changed, check Medium Change */

    if (pDrive->state == ATA_DEV_MED_CH || pAtapiDev->blkDev.bd_readyChanged )
        {

        if (ataPiReadCapacity (pAtapiDev) != OK)
            {
            pAtapiDev->blkDev.bd_readyChanged = TRUE;
            return (ERROR);
            }
        else
            pDrive->state = ATA_DEV_OK;
        }

    ATA_DEBUG_MSG (2, "ataPiStatusChk%d/%d: Ok: state=%d readyChanged=%d\n", 
                   pAtapiDev->ctrl, pAtapiDev->drive, pDrive->state, 
                   pAtapiDev->blkDev.bd_readyChanged, 0, 0);

    return (OK); /* open or create operations can continue */
    } /* ataPiStatusChk */





#ifdef ATA_DEBUG

/* DEBUG functions */

STATUS ataIntCheck (ATA_DEV *	pAtapiDev)
{
ATA_CTRL *  pCtrl   = &ataCtrl[pAtapiDev->ctrl];
int         semStatus;

semStatus = semTake (&pCtrl->syncSem, 1);
if (semStatus == ERROR)
    return ERROR;

return OK;
}

void ataSemGive (ATA_DEV *	pAtapiDev)
{
ATA_CTRL *  pCtrl   = &ataCtrl[pAtapiDev->ctrl];

semGive (&pCtrl->syncSem);
}

void ataTimeoutSet (int ctrl, int val)
{
ATA_CTRL *pCtrl		= &ataCtrl[ctrl];

pCtrl->semTimeout = val;
pCtrl->wdgTimeout = val;
}
#endif
