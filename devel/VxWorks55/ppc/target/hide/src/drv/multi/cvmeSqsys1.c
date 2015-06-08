/* cvmeSqsys1.c - Cyclone Squall sys-1 module library */

/* Copyright 1984-1993 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01a,03mar93,ccc  created.
*/

/*
DESCRIPTION
This library contains routines to manipulate the Cyclone Squall sys-1
module.  This library addresses initialization, plus the SCSI and
LANCE ethernet functions of the Squall sys-1.
*/

/* memory configuration table entries for SQSYS1 (82596 ethernet) */
#define	SQSYS1_MCON_C		(UINT32) 0x00100002
#define	SQSYS1_MCON_D		(UINT32) 0x00000000

/*******************************************************************************
*
* sysSquallInit - initialize the Cyclone Squall sys-1 module hardware
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
* Set up the control table entries for the SQSYS1 Squall module.  In addition,
* save these parameters in the Cyclone NVRAM area.
*
* NOMANUAL
*/

void sys596Init
    (
    volatile int unit           /* ignored */
    )
    {
    sysSquallCtrlTableSetup (SQSYS1_MCON_C, SQSYS1_MCON_D,
			     CVME960_VEC_XINT3, CVME960_VEC_XINT4,
			     LEVEL_TRIG, EDGE_TRIG);

    sysSquallRamSet (SQSYS1_MCON_C, SQSYS1_MCON_D,
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

#ifdef INCLUDE_SCSI

/******************************************************************************
*
* sysScsiBusReset - assert the RST line on the SCSI bus
*
* This routine asserts the RST line on the SCSI bus, which causes all
* connected devices to return to a quiescent state.
*
* RETURNS: N/A
*/

void sysScsiBusReset
    (
    FAST WD_33C93_SCSI_CTRL *pSbic	/* ptr to SBIC info */
    )
    {
    unsigned long oldLevel;		/* To restore interrupt mask */
    unsigned long ix;			/* To restore interrupt mask */

    oldLevel = intLock ();		/* Get current irq mask */

    /* Toggle the reset line, delaying a bit in between */
    *SCSI_BUS_RESET_CTRL = ASSERT_SCSI_BUS_RESET;

    /* This is ~30ms at 33 Mhz.  SCSI spec says RST must be low > 25us.
     * We can't use task delay, as it causes rescheduling, and the
     * reset interrupt will be re-enabled.
     */

    for (ix = 0; ix < 250000; ix++) ;

    *SCSI_BUS_RESET_CTRL = DEASSERT_SCSI_BUS_RESET;

    /* Clear flag bit to stop reset interrupt to CPU */
    *SCSI_BUS_RESET_CTRL = CLEAR_SCSI_RESET_IRQ;

    vxIPNDClear (CVME960_MASK_XINT3);
    intUnlock (oldLevel);		/* Restore original mask */
    }

/******************************************************************************
*
* sysScsiIntr - interrupt handler for xint3 interrupts
*
* This routine is the interrupt handler for the xint3 interrupt from the
* SQSYS1 squall module.  This interrupt can come from three sources:
* the Western Digital 33C93 INTRQ pin, the SCSI bus reset interrupt
* (signalled when the SCSI bus is reset), and the squall module SCC.
* This routine examines the squall's status register to determine the
* source of the interrupt, and then handles it appropriately.
*
* RETURNS: N/A
*/

LOCAL void sysScsiIntr
    (
    WD_33C93_SCSI_CTRL *pSysScsiCtrl
    )
    {
    /* Read squall xint3 status register */
    FAST unsigned char status = *SQ_STATUS_REG;

    if (!(status & (1 << SCC_IRQ_BIT)))
	logMsg ("Unexpected SCC interrupt!\n");

    if (!(status & (1 << SCSI_IRQ_BIT)))
	sbicIntr (pSysScsiCtrl);

    if (!(status & (1 << SCSI_BUS_RESET_BIT)))
	logMsg ("Unexpected SCSI bus reset!\n");

    vxIPNDClear (CVME960_MASK_XINT3);
    }

/******************************************************************************
*
* sysScsiInit - initialize a Western Digital SBIC
*
* This routine creates and initializes an SBIC structure, enabling use of the
* on-board SCSI port. It also connects the proper interrupt service routine
* to the desired vector, and enables the interrupt at the desired level.
*
* RETURNS: OK, or ERROR if the SBIC structure cannot be connected, or the
* controller cannot be initialized.
*/

STATUS sysScsiInit (void)
    {
    if ((pSysScsiCtrl =
	(SCSI_CTRL *) wd33c93CtrlCreate (CVME960_SBIC_BASE_ADRS,
					 CVME960_SBIC_REG_OFFSET,
					 CVME960_SBIC_CLK_PERIOD,
					 CVME960_SBIC_DEV_TYPE,
					 (FUNCPTR) sysScsiBusReset,
					 (FUNCPTR) NULL,
					 (FUNCPTR) NULL
					)) == NULL)
	{
	return (ERROR);
	}

    /* connect the SCSI controller's interrupt service routine */

    if (intConnect ((VOIDFUNCPTR *) CVME960_VEC_XINT3, sysScsiIntr,
		    (int) pSysScsiCtrl) == ERROR)
	{
	return (ERROR);
	}

    /* enable the SBIC interrupt */

    vxIPNDClear (CVME960_MASK_XINT3);
    vxIMRSet (CVME960_MASK_XINT3);	/* Re-enable xint3 irq's */

    /* initialize SCSI controller with default parameters (user tunable) */

    if (wd33c93CtrlInit ((WD_33C93_SCSI_CTRL *) pSysScsiCtrl,
			 SCSI_DEF_CTRL_BUS_ID,
			 SCSI_DEF_SELECT_TIMEOUT,
			 NONE) == ERROR)
	{
	return (ERROR);
	}

    return (OK);
    }

#endif /* INCLUDE_SCSI */
