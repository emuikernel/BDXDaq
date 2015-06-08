/* cvmeSqsys3.c - Cyclone Squall sys-3 module library */

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,24jan95,jdi  doc tweak to sysScsiInit().
01a,03mar93,ccc  created.
*/

/*
DESCRIPTION
This library contains routines to manipulate the Cyclone Squall sys-3
module.  This library addresses initialization, plus the SCSI and
LANCE ethernet functions of the Squall sys-3.
*/

/* memory configuration table entries for SQSYS3 (82596 ethernet) */
#define	SQSYS3_MCON_C		(UINT32) 0x00100002
#define	SQSYS3_MCON_D		(UINT32) 0x00000000

/*******************************************************************************
*
* sysSquallInit - initialize the Cyclone Squall sys-3 module hardware
*
* RETURNS: N/A
*
* NOMANUAL
*/

void sysSquallInit (void)
    {
    }

/******************************************************************************
*
* sys596Init - performs any additional board specific 82596 device init
*
* Set up the control table entries for the SQSYS3 Squall module.  In addition,
* save these parameters in the Cyclone NVRAM area.
*
* NOMANUAL
*/

void sys596Init
    (
    volatile int unit           /* ignored */
    )
    {
    sysSquallCtrlTableSetup (SQSYS3_MCON_C, SQSYS3_MCON_D,
			     CVME960_VEC_XINT3, CVME960_VEC_XINT4,
			     LEVEL_TRIG, EDGE_TRIG);

    sysSquallRamSet (SQSYS3_MCON_C, SQSYS3_MCON_D,
		     CVME960_VEC_XINT3, CVME960_VEC_XINT4,
		     LEVEL_TRIG, EDGE_TRIG);
    }

/******************************************************************************
*
* sys596Port - writes a command to the 82596 device PORT location
*
* There are 4 commands the device handles.
*
* NOMANUAL
*/

void sys596Port
    (
    volatile int unit,  /* ignored */
    int cmd,            /* the command to write */
    UINT32 addr         /* address or NULL if PORT_RESET command */
    )
    {
    volatile UINT32 *port;

    port = (volatile UINT32 *) CVME960_EI_PORT;
    *port = (cmd & 0x3) | (addr & 0xfffffffc);  /* requires two accesses */
    *port = (cmd & 0x3) | (addr & 0xfffffffc);
    }

/******************************************************************************
*
* sys596ChanAtn - assert the Channel Attention signal to the 82596 device
*
* NOMANUAL
*/

void sys596ChanAtn
    (
    volatile int unit		/* ignored */
    )
    {
    *CVME960_EI_CA = 1;		/* write to the addr; data is ignored */
    }

/******************************************************************************
*
* sys596IntAck - acknowlege interrupts from the 82596 device
*
* Since the interrupts from the 82596 are falling-edge detected,
* there is no need to acknowlege them.
*
* NOMANUAL
*/

void sys596IntAck
    (
    volatile int unit    /* ignored */
    )
    {
    }

/******************************************************************************
*
* sys596IntEnable - enable interrupts from the 82596 device
*
* Enables 82596 Ethernet controller interrupts by setting the
* appropriate bit in the 960's IMSK register.
*
* NOMANUAL
*/

void sys596IntEnable
    (
    volatile int unit    /* ignored */
    )
    {
    vxIMRSet (CVME960_MASK_XINT4);
    }

/******************************************************************************
*
* sys596IntDisable - disable interrupts from the 82596 device
*
* Disables 82596 Ethernet controller interrupts by clearing the
* appropriate bit in the 960's IMSK register.
*
* NOMANUAL
*/

void sys596IntDisable
    (
    volatile int unit    /* ignored */
    )
    {
    vxIMRClear (CVME960_MASK_XINT4);
    }

#ifdef  INCLUDE_SCSI
/******************************************************************************
*
* sysScsiInit - initialize NCR710 SCSI chip
*
* This routine creates and initializes an SIOP structure, enabling use of the
* on-board SCSI port.  It also connects the proper interrupt service routine
* to the desired vector, and enables the interrupt at the desired level.
*
* RETURNS: OK, or ERROR if the control structure is not created or the
* interrupt service routine cannot be connected to the interrupt.
*/

STATUS sysScsiInit ()

    {
    /* Local structure with a prefill for ncr710SetHwRegister */
    static NCR710_HW_REGS hwRegs = CVME960_SIOP_HW_REGS;

    if ((pSysScsiCtrl = (SCSI_CTRL *) ncr710CtrlCreate (CVME960_SIOP_BASE_ADRS,
                                                        CVME960_SIOP_FREQ
                                                        )) == NULL)
    {
    return (ERROR);
    }
        /* connect the SCSI controller's interrupt service routine */

    if (intConnect ((VOIDFUNCPTR *)(INUM_TO_IVEC (INT_VEC_SCSI)),
		    ncr710Intr, (int) pSysScsiCtrl) == ERROR)
	{
        return (ERROR);
        }

    vxIMRSet (CVME960_MASK_XINT3);

    /* Set the good value in the registers of the SIOP coupled
     * with the hardware implementation
     * NO MUX HOST BUS/NO BURST ACCES/SNOOP :DEFAULT/ENABLE SYNC HOST BUS/
     * BURST SIZE DEFAULT/OTHER :DEFAULT CONFIGURATION
     */

    if (ncr710SetHwRegister (pSysScsiCtrl, &hwRegs) == ERROR)
       return(ERROR);

    /* initialize SCSI controller with default parameters (user tunable) */

    if (ncr710CtrlInit (pSysScsiCtrl, SCSI_DEF_CTRL_BUS_ID, NONE) == ERROR)
        return (ERROR);

    return (OK);
    }

#endif	/* INCLUDE_SCSI */
