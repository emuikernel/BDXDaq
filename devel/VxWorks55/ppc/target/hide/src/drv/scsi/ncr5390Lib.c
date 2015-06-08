/* ncr5390Lib.c - NCR5390 SCSI-Bus Interface Controller library (SBIC) */
  
/* Copyright 1989-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,12jul96,dds changed filename from ncr5390.c to ncr5390Lib.c
01a,03may96,dds re-written; sample used was wd33c93Lib.c. Old ncr5390Lib.c
		is now in ncr5390Lib1.c
*/

/*
DESCRIPTION
This library contains the main interface routines to the SCSI-Bus
Interface Controllers (SBIC). These routines simply switch the calls
to the  SCSI-1 or SCSI-2 drivers, implemented in ncr5390Lib1.c or
ncr5390Lib2.c as configured by the Board Support Package (BSP).

In order to configure the SCSI-1 driver, which depends upon scsi1Lib, the
ncr5390CtrlCreate() routine, defined in ncr5390Lib1, must be invoked. 
Similarly ncr5390CtrlCreateScsi2(), defined in ncr5390Lib2 and dependent
on scsi2Lib, must be called to configure and initialize the SCSI-2 driver.

INCLUDE FILES
ncr5390.h, ncr5390_1.h, ncr5390_2.h

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

typedef struct ncr5390functbl
    {
    FUNCPTR ascCommand;
    FUNCPTR ascIntr;
    FUNCPTR ascRegRead;
    FUNCPTR ascRegWrite;
    FUNCPTR ascXferCountGet;
    FUNCPTR ascXferCountSet;
    FUNCPTR ncr5390CtrlInit;
    FUNCPTR ncr5390Show;
    } SCSIDRV_FUNC_TBL;

/* globals */

SCSIDRV_FUNC_TBL *pNcr5390IfTbl;

/*******************************************************************************
*
* ncr5390IfTblInit - create and partially initialize a NCR5390 ASC structure
*
* NOMANUAL
*/
void ncr5390IfTblInit ()
    {
    /* clear allocation to initialize the table */

    pNcr5390IfTbl = calloc (1, sizeof (SCSIDRV_FUNC_TBL));

    }

/*******************************************************************************
*
* ncr5390CtrlInit - initialize the user-specified fields in an ASC structure
*
* This routine initializes an ASC structure, after the structure is created
* with ncr5390CtrlCreate().
* This structure must be initialized before the ASC can be used.
* It may be called more than once; however, it should be called only 
* while there is no activity on the SCSI interface.
*
* Before returning, this routine pulses RST (reset) on the SCSI bus, thus
* resetting all attached devices.
*
* The input parameters are as follows:
* .iP <pAsc> 4
* a pointer to the NCR5390_SCSI_CTRL structure created with
* ncr5390CtrlCreate().
* .iP <scsiCtrlBusId>
* the SCSI bus ID of the ASC, in the range 0 - 7.  The ID is somewhat
* arbitrary; the value 7, or highest priority, is conventional.
* .iP <defaultSelTimeOut>
* the timeout, in microseconds, for selecting a SCSI device attached to this
* controller.  This value is used as a default if no timeout is specified in
* scsiPhysDevCreate().  The recommended value zero (0) specifies
* SCSI_DEF_SELECT_TIMEOUT (250 millisec).  The maximum timeout possible is
* approximately 2 seconds.  Values exceeding this revert to the
* maximum.  
* .iP <scsiPriority>
* the priority to which a task is set when performing a SCSI
* transaction.  Valid priorities are 0 to 255.  Alternatively, the value -1
* specifies that the priority should not be altered during SCSI transactions.
*
* RETURNS: OK, or ERROR if a parameter is out of range.
*
* SEE ALSO: scsiPhysDevCreate(),
*/

STATUS ncr5390CtrlInit
    (
    FAST int   *pAsc,            /* ptr to ASC info                       */
    FAST int   scsiCtrlBusId,     /* SCSI bus ID of this ASC               */
    FAST UINT  defaultSelTimeOut, /* default dev. select timeout (microsec) */
    int        scsiPriority       /* priority of task when doing SCSI I/O   */
    )
    {
    if (pNcr5390IfTbl->ncr5390CtrlInit != NULL)
	return ((int) (pNcr5390IfTbl->ncr5390CtrlInit) (pAsc, scsiCtrlBusId,
		       defaultSelTimeOut, scsiPriority));
    else
	return ((int) ERROR);

    }

/*******************************************************************************
*
* ascXferCountSet - load the ASC transfer counter with the specified count
*
* RETURNS: OK, or ERROR if <count> is not in the
* range 0 - NCR5390_MAX_BYTES_PER_XFER.
*
* NOMANUAL
*/

int ascXferCountSet
    (
    FAST int  *pAsc,   /* ptr to ASC info    */
    FAST UINT  count    /* count value to load */
    )
    {
    if (pNcr5390IfTbl->ascXferCountSet != NULL)
	return ((int) (pNcr5390IfTbl->ascXferCountSet) (pAsc, count));
    else
	return ((int) ERROR);
    }

/*******************************************************************************
*
* ascXferCountGet - fetch the ASC transfer count
*
* The value of the transfer counter is copied to *pCount.
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void ascXferCountGet
    (
    FAST int  *pAsc,   /* ptr to ASC info      */
    FAST int  *pCount   /* ptr to returned value */
    )
    {
    if (pNcr5390IfTbl->ascXferCountGet != NULL)
        (pNcr5390IfTbl->ascXferCountGet) (pAsc, pCount);

    }

/*******************************************************************************
*
* ascCommand - write a command code to the ASC Command Register
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void ascCommand
    (
    int  *pAsc,        /* ptr to ASC info */
    UINT8 cmdCode       /* new command code */
    )
    {
    if (pNcr5390IfTbl->ascCommand != NULL)
	(pNcr5390IfTbl->ascCommand) (pAsc, cmdCode);
    }

/*******************************************************************************
*
* ascIntr - interrupt service routine for the ASC
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void ascIntr
    (
    int *pAsc          /* ptr to ASC info */
    )
    {
    if (pNcr5390IfTbl->ascIntr != NULL)
	(pNcr5390IfTbl->ascIntr) (pAsc);

    }

/*******************************************************************************
*
* ascRegRead - Get the contents of a specified ASC register
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void ascRegRead
    (
    int  *pAsc,         /* ptr to an ASC structure */
    UINT8 regAdrs,       /* register to read         */
    int  *pDatum         /* put data here            */
    )
    {
    if (pNcr5390IfTbl->ascRegRead != NULL)
	(pNcr5390IfTbl->ascRegRead) (pAsc, regAdrs, pDatum);

    }

/*******************************************************************************
*
* ascRegWrite - write a value to a specified ASC register
*
* RETURNS: N/A.
*
* NOMANUAL
*/

void ascRegWrite
    (
    int  *pAsc,         /* ptr to an ASC structure */
    UINT8 regAdrs,       /* register to write        */
    UINT8 datum          /* data to write            */
    )
    {
    if (pNcr5390IfTbl->ascRegRead != NULL)
	(pNcr5390IfTbl->ascRegRead) (pAsc, regAdrs, datum);

    }

/*******************************************************************************
*
* ncr5390Show - display the values of all readable NCR5390 chip registers
*
* This routine displays the state of the ASC registers in a user-friendly
* manner.  It is useful primarily for debugging.  It should not be invoked
* while another running process is accessing the SCSI controller.
*
* EXAMPLE:
* .CS
*     -> ncr5390Show
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

int ncr5390Show
    (
    FAST int *pScsiCtrl   /* ptr to SCSI controller info */
    )
    {
    if (pNcr5390IfTbl->ncr5390Show != NULL)
	return ((int) (pNcr5390IfTbl->ncr5390Show) (pScsiCtrl));
    else
	return ((int) ERROR);

    }
