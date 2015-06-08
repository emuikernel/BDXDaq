/* wd33c93Lib.c - WD33C93 SCSI-Bus Interface Controller (SBIC) library */

/* Copyright 1989-1995 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01f,02May98,sbs  changed sbicRegRead to sbicRegWrite in sbicRegWrite() 
                 function (SPR #20923)
01e,06may96,jds  yet more doc tweaks
01d,01may96,jds  more doc tweaks
01c,20sep95,jdi  doc tweaks.
01b,21feb95,rhp  doc: Describe connection to wd33c93Lib1 more explicitly.
01a,10oct94,jds  written; derived from previous wd33c93Lib.h
*/

/*
DESCRIPTION
This library contains the main interface routines to the Western
Digital WD33C93 and WD33C93A SCSI-Bus Interface Controllers (SBIC). However,
these routines simply switch the calls to either the SCSI-1 or SCSI-2
drivers, implemented in wd33c93Lib1 and wd33c93Lib2 respectively, as configued
by the Board Support Package (BSP). 

In order to configure the SCSI-1 driver, which depends upon scsi1Lib, the 
wd33c93CtrlCreate() routine, defined in wd33c93Lib1, must be invoked. Similarly,
wd33c93CtrlCreateScsi2(), defined in wd33c93Lib2 and dependent on scsi2Lib, 
must be called to configure and initialize the SCSI-2 driver.

INCLUDE FILES
wd33c93.h, wd33c93_1.h, wd33c93_2.h

SEE ALSO: scsiLib, scsi1Lib, scsi2Lib, wd33c93Lib1, wd33c93Lib2,
.I "Western Digital WD33C92/93 SCSI-Bus Interface Controller,"
.I "Western Digital WD33C92A/93A SCSI-Bus Interface Controller,"
.pG "I/O System"
*/

#include "vxWorks.h"
#include "memLib.h"
#include "logLib.h"
#include "scsiLib.h"
#include "stdlib.h"
#include "errnoLib.h"
#include "stdio.h"
#include "sysLib.h"
#include "intLib.h"
#include "tickLib.h"
#include "taskLib.h"
#include "msgQLib.h"

/* data structures */

typedef struct wd33c93functbl
    {
    FUNCPTR sbicCommand;
    FUNCPTR sbicIntr;
    FUNCPTR sbicRegRead;
    FUNCPTR sbicRegWrite;
    FUNCPTR sbicXferCountGet;
    FUNCPTR sbicXferCountSet;
    FUNCPTR wd33c93CtrlInit;
    FUNCPTR wd33c93Show;
    } SCSIDRV_FUNC_TBL;


/* globals */


SCSIDRV_FUNC_TBL *pWd33c93IfTbl;


/*******************************************************************************
*
* wd33c93IfTblInit - create and partially initialize a WD33C93 SBIC structure
*
* NOMANUAL
*/
void wd33c93IfTblInit ()
    {
    /* clear allocation to initialize the table */

    pWd33c93IfTbl = calloc (1, sizeof (SCSIDRV_FUNC_TBL));

    }

/*******************************************************************************
*
* wd33c93CtrlInit - initialize the user-specified fields in an SBIC structure
*
* This routine initializes an SBIC structure, after the structure is created
* with either wd33c93CtrlCreate() or wd33c93CtrlCreateScsi2().  
* This structure must be initialized before the SBIC
* can be used.  It may be called more than once; however, it should
* be called only while there is no activity on the SCSI interface.
*
* Before returning, this routine pulses RST (reset) on the SCSI bus, thus
* resetting all attached devices.
*
* The input parameters are as follows:
* .iP <pSbic> 4
* a pointer to the WD_33C93_SCSI_CTRL structure created with
* wd33c93CtrlCreate() or wd33c93CtrlCreateScsi2().
* .iP <scsiCtrlBusId>
* the SCSI bus ID of the SBIC, in the range 0 - 7.  The ID is somewhat
* arbitrary; the value 7, or highest priority, is conventional.
* .iP <defaultSelTimeOut>
* the timeout, in microseconds, for selecting a SCSI device attached to this
* controller.  This value is used as a default if no timeout is specified in
* scsiPhysDevCreate().  The recommended value zero (0) specifies
* SCSI_DEF_SELECT_TIMEOUT (250 millisec).  The maximum timeout possible is
* approximately 2 seconds.  Values exceeding this revert to the
* maximum.  For more information about chip timeouts, see the manuals
* .I "Western Digital WD33C92/93 SCSI-Bus Interface Controller,"
* .I "Western Digital WD33C92A/93A SCSI-Bus Interface Controller."
* .iP <scsiPriority>
* the priority to which a task is set when performing a SCSI
* transaction.  Valid priorities are 0 to 255.  Alternatively, the value -1
* specifies that the priority should not be altered during SCSI transactions.
*
* RETURNS: OK, or ERROR if a parameter is out of range.
*
* SEE ALSO: scsiPhysDevCreate(),
* .I "Western Digital WD33C92/93 SCSI-Bus Interface Controller,"
* .I "Western Digital WD33C92A/93A SCSI-Bus Interface Controller"
*/

STATUS wd33c93CtrlInit
    (
    FAST int   *pSbic,             /* ptr to SBIC info                       */
    FAST int   scsiCtrlBusId,     /* SCSI bus ID of this SBIC               */
    FAST UINT  defaultSelTimeOut, /* default dev. select timeout (microsec) */
    int        scsiPriority       /* priority of task when doing SCSI I/O   */
    )
    {
    if (pWd33c93IfTbl->wd33c93CtrlInit != NULL)
	return ((int) (pWd33c93IfTbl->wd33c93CtrlInit) (pSbic, scsiCtrlBusId,
		       defaultSelTimeOut, scsiPriority));
    else
	return ((int) ERROR);

    }

/*******************************************************************************
*
* sbicXferCountSet - load the SBIC transfer counter with the specified count
*
* RETURNS: OK, or ERROR if <count> is not in the
* range 0 - WD_33C93_MAX_BYTES_PER_XFER.
*
* NOMANUAL
*/

int sbicXferCountSet
    (
    FAST int  *pSbic,   /* ptr to SBIC info    */
    FAST UINT  count    /* count value to load */
    )
    {
    if (pWd33c93IfTbl->sbicXferCountSet != NULL)
	return ((int) (pWd33c93IfTbl->sbicXferCountSet) (pSbic, count));
    else
	return ((int) ERROR);
    }

/*******************************************************************************
*
* sbicXferCountGet - fetch the SBIC transfer count
*
* The value of the transfer counter is copied to *pCount.
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void sbicXferCountGet
    (
    FAST int  *pSbic,   /* ptr to SBIC info      */
    FAST int  *pCount   /* ptr to returned value */
    )
    {
    if (pWd33c93IfTbl->sbicXferCountGet != NULL)
        (pWd33c93IfTbl->sbicXferCountGet) (pSbic, pCount);

    }

/*******************************************************************************
*
* sbicCommand - write a command code to the SBIC Command Register
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void sbicCommand
    (
    int  *pSbic,        /* ptr to SBIC info */
    UINT8 cmdCode       /* new command code */
    )
    {
    if (pWd33c93IfTbl->sbicCommand != NULL)
	(pWd33c93IfTbl->sbicCommand) (pSbic, cmdCode);
    }

/*******************************************************************************
*
* sbicIntr - interrupt service routine for the SBIC
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void sbicIntr
    (
    int *pSbic          /* ptr to SBIC info */
    )
    {
    if (pWd33c93IfTbl->sbicIntr != NULL)
	(pWd33c93IfTbl->sbicIntr) (pSbic);

    }

/*******************************************************************************
*
* sbicRegRead - Get the contents of a specified SBIC register
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void sbicRegRead
    (
    int  *pSbic,         /* ptr to an SBIC structure */
    UINT8 regAdrs,       /* register to read         */
    int  *pDatum         /* put data here            */
    )
    {
    if (pWd33c93IfTbl->sbicRegRead != NULL)
	(pWd33c93IfTbl->sbicRegRead) (pSbic, regAdrs, pDatum);

    }

/*******************************************************************************
*
* sbicRegWrite - write a value to a specified SBIC register
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void sbicRegWrite
    (
    int  *pSbic,         /* ptr to an SBIC structure */
    UINT8 regAdrs,       /* register to write        */
    UINT8 datum          /* data to write            */
    )
    {
    if (pWd33c93IfTbl->sbicRegWrite != NULL)
	(pWd33c93IfTbl->sbicRegWrite) (pSbic, regAdrs, datum);

    }

/*******************************************************************************
*
* wd33c93Show - display the values of all readable WD33C93 chip registers
*
* This routine displays the state of the SBIC registers in a user-friendly
* manner.  It is useful primarily for debugging.  It should not be invoked
* while another running process is accessing the SCSI controller.
*
* EXAMPLE:
* .CS
*     -> wd33c93Show
*     REG #00 (Own ID         ) = 0x07
*     REG #01 (Control        ) = 0x00
*     REG #02 (Timeout Period ) = 0x20
*     REG #03 (Sectors        ) = 0x00
*     REG #04 (Heads          ) = 0x00
*     REG #05 (Cylinders MSB  ) = 0x00 
*     REG #06 (Cylinders LSB  ) = 0x00
*     REG #07 (Log. Addr. MSB ) = 0x00
*     REG #08 (Log. Addr. 2SB ) = 0x00
*     REG #09 (Log. Addr. 3SB ) = 0x00
*     REG #0a (Log. Addr. LSB ) = 0x00
*     REG #0b (Sector Number  ) = 0x00
*     REG #0c (Head Number    ) = 0x00 
*     REG #0d (Cyl. Number MSB) = 0x00
*     REG #0e (Cyl. Number LSB) = 0x00
*     REG #0f (Target LUN     ) = 0x00
*     REG #10 (Command Phase  ) = 0x00
*     REG #11 (Synch. Transfer) = 0x00
*     REG #12 (Xfer Count MSB ) = 0x00
*     REG #13 (Xfer Count 2SB ) = 0x00
*     REG #14 (Xfer Count LSB ) = 0x00
*     REG #15 (Destination ID ) = 0x03
*     REG #16 (Source ID      ) = 0x00
*     REG #17 (SCSI Status    ) = 0x42
*     REG #18 (Command        ) = 0x07
* .CE
*
* RETURNS: OK, or ERROR if <pScsiCtrl> and <pSysScsiCtrl> are both NULL.
*/

int wd33c93Show
    (
    FAST int *pScsiCtrl   /* ptr to SCSI controller info */
    )
    {
    if (pWd33c93IfTbl->wd33c93Show != NULL)
	return ((int) (pWd33c93IfTbl->wd33c93Show) (pScsiCtrl));
    else
	return ((int) ERROR);

    }
