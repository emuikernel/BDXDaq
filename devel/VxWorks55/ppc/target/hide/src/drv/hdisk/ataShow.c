/* ataShow.c - ATA/IDE (LOCAL and PCMCIA) disk device driver show routine */

/* Copyright 1989-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01k,09dec01,jkf  changed copyright to 2002
01j,09nov01,jac  SPR#67795, added support to display ATAPI CDROM device info.
01i,18mar99,jdi  doc: updated w/ info about proj facility (SPR 25727).
01h,15jan99,jkf  added endian ifdef to properly display revision
                 and model strings on big endian arches.
01g,11nov96,dgp  doc: final formatting
01f,11nov96,hdn  removed NOMANUAL from ataShowInit() and ataShow().
01e,01nov96,hdn  added support for PCMCIA.
01d,25sep96,hdn  added support for ATA-2.
01c,18mar96,hdn  added ataShowInit().
01b,01mar96,hdn  cleaned up.
01a,02mar95,hdn  written based on ideDrv.c.
*/

/*
DESCRIPTION
This library contains a driver show routine for the ATA/IDE (PCMCIA and LOCAL) 
devices supported on the IBM PC.

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
#include "drv/hdisk/ataDrv.h"

/* imports */

IMPORT ATA_CTRL		ataCtrl [ATA_MAX_CTRLS];
IMPORT ATA_TYPE		ataTypes [ATA_MAX_CTRLS][ATA_MAX_DRIVES];
IMPORT ATA_RESOURCE	ataResources [ATA_MAX_CTRLS];
IMPORT BOOL		ataDrvInstalled;


/* globals */


/* locals */


/* function prototypes */


/*******************************************************************************
*
* ataShowInit - initialize the ATA/IDE disk driver show routine
*
* This routine links the ATA/IDE disk driver show routine into the VxWorks
* system.  It is called automatically when this show facility is configured
* into VxWorks using either of the following methods:
* .iP
* If you use the configuration header files, define
* INCLUDE_SHOW_ROUTINES in config.h.
* .iP
* If you use the Tornado project facility, select INCLUDE_ATA_SHOW.
*
* RETURNS: N/A
*/

void ataShowInit (void)
    {
    }

/*******************************************************************************
*
* ataShow - show the ATA/IDE disk parameters
*
* This routine shows the ATA/IDE disk parameters.  Its first argument is a
* controller number, 0 or 1; the second argument is a drive number, 0 or 1.
*
* RETURNS: OK, or ERROR if the parameters are invalid.
*/

STATUS ataShow
    (
    int ctrl,
    int drive
    )
    {
    ATA_CTRL *pCtrl		= &ataCtrl[ctrl];
    ATA_DRIVE *pDrive		= &pCtrl->drive[drive];
    ATA_PARAM *pParam		= &pDrive->param;
    ATA_RESOURCE *pResource	= &ataResources[ctrl];
    ATA_TYPE *pType		= &ataTypes[ctrl][drive];
    char *pMode			= "UNKNOWN";
    int ix;

    if ((ctrl >= ATA_MAX_CTRLS) || (drive >= ATA_MAX_DRIVES) ||
        !ataDrvInstalled || !pCtrl->installed)
	return (ERROR);

    if (pResource->ctrlType == ATA_PCMCIA)
        printf ("ATA-PCMCIA\n");
    else
        printf ("ATA-LOCAL\n");

    if (pDrive->type == ATA_TYPE_ATA)
        printf ("ATA device\n");
    else if (pDrive->type == ATA_TYPE_ATAPI)
        printf ("ATAPI device\n");
    else if (pDrive->type == ATA_TYPE_NONE)
        printf ("NO device present\n");
    else if (pDrive->type == ATA_TYPE_INIT)
        printf ("UNINITIALIZED device\n");
    else
        printf ("UNRECOGNIZED device\n");

    printf ("device state	=");

    if (pDrive->state == ATA_DEV_OK)
        printf ("OK\n");
    else if (pDrive->state == ATA_DEV_NONE)
        printf ("absent or does not respond\n");
    else if (pDrive->state == ATA_DEV_DIAG_F)
        printf ("diagnostic failed: diagCode=0x%02x\n", pDrive->diagCode);
    else if (pDrive->state == ATA_DEV_PREAD_F)
        printf ("read parameters failed\n");
    else if (pDrive->state == ATA_DEV_MED_CH)
        printf ("medium have been changed\n");
    else if (pDrive->state == ATA_DEV_INIT)
        printf ("uninitialized\n");
    else
        printf ("illegal\n");

    if (pDrive->type == ATA_TYPE_ATA) {
        printf ("pAtaDev		=%p\n", pDrive->pAtaDev);

        printf ("  intCount     =%-8d    intStatus   =0x%-8x\n",
    	    pCtrl->intCount, pCtrl->intStatus);
        printf ("ataTypes\n");
        printf ("  cylinders    =%-8d    heads       =%-8d\n",
    	    pType->cylinders, pType->heads);
        printf ("  sectorsTrack =%-8d    bytesSector =%-8d\n",
    	    pType->sectors, pType->bytes);
        printf ("  precomp      =0x%-8x\n", pType->precomp);

        printf ("ataParams\n");
        printf ("  cylinders    =%-8d    heads       =%-8d\n",
    	    (USHORT)pParam->cylinders, (USHORT)pParam->heads);	
        printf ("  sectorsTrack =%-8d    bytesSector =%-8d\n",
    	    (USHORT)pParam->sectors, (USHORT)pParam->bytesSec);
        printf ("  config       =0x%-8x  removcyl    =0x%-8x\n",
    	    (USHORT)pParam->config, (USHORT)pParam->removcyl);
        printf ("  bytesTrack   =0x%-8x  bytesGap    =0x%-8x\n",
    	    (USHORT)pParam->bytesTrack, (USHORT)pParam->bytesGap);
        printf ("  bytesSync    =0x%-8x  vendstat    =0x%-8x\n",
    	    (USHORT)pParam->bytesSync, (USHORT)pParam->vendstat);
	}
    else
        {
        printf ("  config       =0x%04x\n",(USHORT)pParam->config);
	}

    printf ("  serial       =");

    for (ix = 0; ix < 10; ix++)
	{
#if (_BYTE_ORDER == _LITTLE_ENDIAN)
        printf ("%c", pParam->serial[ix * 2 + 1]);
        printf ("%c", pParam->serial[ix * 2]);
#else
        printf ("%c", pParam->serial[ix * 2]);
        printf ("%c", pParam->serial[ix * 2 + 1]);
#endif /* (_BYTE_ORDER == _LITTLE_ENDIAN) */

	}
    printf ("\n");

    if (pDrive->type == ATA_TYPE_ATA)
        {
        printf ("  type         =0x%-8x  size        =0x%-8x\n",
    	    (USHORT)pParam->type, (USHORT)pParam->size);
        printf ("  bytesEcc     =0x%-8x\n",
    	    (USHORT)pParam->bytesEcc);
	}

    printf ("  rev          =");

    for (ix = 0; ix < 4; ix++)
	{
#if (_BYTE_ORDER == _LITTLE_ENDIAN)
        printf ("%c", pParam->rev[ix * 2 + 1]);
        printf ("%c", pParam->rev[ix * 2]);
#else
        printf ("%c", pParam->rev[ix * 2]);
        printf ("%c", pParam->rev[ix * 2 + 1]);
#endif /* (_BYTE_ORDER == _LITTLE_ENDIAN) */
	}

    printf ("\n");
    printf ("  model        =");

    for (ix = 0; ix < 20; ix++)
	{
#if (_BYTE_ORDER == _LITTLE_ENDIAN)
        printf ("%c", pParam->model[ix * 2 + 1]);
        printf ("%c", pParam->model[ix * 2]);
#else
        printf ("%c", pParam->model[ix * 2]);
        printf ("%c", pParam->model[ix * 2 + 1]);
#endif /* (_BYTE_ORDER == _LITTLE_ENDIAN) */
	}

    printf ("\n");

    if (pDrive->type == ATA_TYPE_ATA)
        {
        printf ("  multiSecs    =0x%-8x  capabilty   =0x%-8x\n",
	    (USHORT)pParam->multiSecs, (USHORT)pParam->capabilities);
        printf ("  pioMode      =0x%-8x  dmaMode     =0x%-8x\n",
	    (USHORT)pParam->pioMode, (USHORT)pParam->dmaMode);
        printf ("  valid        =0x%-8x  curr-cyl    =%-8d\n",
	    (USHORT)pParam->valid, (USHORT)pParam->currentCylinders);
        printf ("  curr-head    =%-8d    curr-sector =%-8d\n",
	    (USHORT)pParam->currentHeads, (USHORT)pParam->currentSectors);
        printf ("  capacity0    =0x%-8x  capacity1   =0x%-8d\n",
	    (USHORT)pParam->capacity0, (USHORT)pParam->capacity1);
        printf ("  multiSet     =0x%-8x  sectors0    =0x%-8x\n",
	    (USHORT)pParam->multiSet, (USHORT)pParam->sectors0);
        printf ("  sectors1     =0x%-8x  singleDma   =0x%-8x\n",
	    (USHORT)pParam->sectors1, (USHORT)pParam->singleDma);
        printf ("  multiDma     =0x%-8x  advancedPio =0x%-8x\n",
	    (USHORT)pParam->multiDma, (USHORT)pParam->advancedPio);
        printf ("  cycleDma     =%-8d    cycleMulti  =%-8d\n",
	    (USHORT)pParam->cycletimeDma, (USHORT)pParam->cycletimeMulti);
        printf ("  cyclePio-wo  =%-8d    cyclePio-w  =%-8d\n",
	    (USHORT)pParam->cycletimePioNoIordy, 
	    (USHORT)pParam->cycletimePioIordy);
	}
    else
        {
        printf ("  pioMode      =0x%04x  dmaMode     =0x%04x\n",
			  (USHORT)pParam->pioMode, (USHORT)pParam->dmaMode);
        printf ("  validity     =0x%04x\n", (USHORT)pParam->valid);
        printf ("  singleDma    =0x%04x  multiDma    =0x%04x\n",
			  (USHORT)pParam->singleDma, (USHORT)pParam->multiDma);

        if (pParam->valid & FIELDS_VALID)
            {
            printf ("  advancedPio  =0x%04x\n",
				(USHORT)pParam->advancedPio);
            printf ("  cycleDma     =%-4d  cycleMulti  =%-4d\n",
				(USHORT)pParam->cycletimeDma, 
				(USHORT)pParam->cycletimeMulti);
            printf ("  cyclePio-wo  =%-4d  cyclePio-w  =%-4d\n",
				(USHORT)pParam->cycletimePioNoIordy,
				(USHORT)pParam->cycletimePioIordy);
            }
        else 
            printf("  words 64-70 not valid\n");

        printf ("  relOverTime  =%-4d    relServTime =%-4d\n",
			  (USHORT)pParam->pktCmdRelTime, (USHORT)pParam->servCmdRelTime);
        printf ("  majorRevNum  =0x%04x  minorVerNum =%-4d\n",
			  (USHORT)pParam->majorRevNum, (USHORT)pParam->minorVersNum);
	}

    printf ("Capability\n");
    printf ("  MULTI: %s, IORDY: %s, DMA: %s, LBA: %s\n",
	    pDrive->okMulti ? "TRUE" : "FALSE",
	    pDrive->okIordy ? "TRUE" : "FALSE",
	    pDrive->okDma ? "TRUE" : "FALSE",
	    pDrive->okLba ? "TRUE" : "FALSE");
    printf ("  multiSectors =0x%-8x  pioMode     =0x%-8x\n",
	    (USHORT)pDrive->multiSecs, (USHORT)pDrive->pioMode);
    printf ("  singleDma    =0x%-8x  multiDma    =0x%-8x\n",
	    (USHORT)pDrive->singleDmaMode, (USHORT)pDrive->multiDmaMode);
    printf ("Configuration\n");

    switch (pDrive->rwMode)
	{
	case ATA_PIO_DEF_W:
	    pMode = "PIO_DEF0";
	    break;

	case ATA_PIO_DEF_WO:
	    pMode = "PIO_DEF1";
	    break;

	case ATA_PIO_W_0:
	    pMode = "PIO_0";
	    break;

	case ATA_PIO_W_1:
	    pMode = "PIO_1";
	    break;

	case ATA_PIO_W_2:
	    pMode = "PIO_2";
	    break;

	case ATA_PIO_W_3:
	    pMode = "PIO_3";
	    break;

	case ATA_PIO_W_4:
	    pMode = "PIO_4";
	    break;

	case ATA_DMA_SINGLE_0:
	case ATA_DMA_MULTI_0:
 	    pMode = "DMA_0";
	    break;

	case ATA_DMA_SINGLE_1:
	case ATA_DMA_MULTI_1:
	    pMode = "DMA_1";
	    break;

	case ATA_DMA_SINGLE_2:
	case ATA_DMA_MULTI_2:
	    pMode = "DMA_2";
	    break;

	}

    printf ("  rwMode       =%-8s    rwBits      =%-8s\n",
	    pMode, (pDrive->rwBits == ATA_BITS_16) ? "16BITS  " : "32BITS  ");
    printf ("  rwPio        =%-8s    rwDma       =%-8s\n",
	    (pDrive->rwPio == ATA_PIO_SINGLE) ? "SINGLE  " : "MULTI   ",
	    (pDrive->rwDma == ATA_DMA_SINGLE) ? "SINGLE  " : "MULTI   ");

    return (OK);
    }
