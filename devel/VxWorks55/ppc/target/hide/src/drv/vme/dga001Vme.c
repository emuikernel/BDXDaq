/* dga001Vme.c - VMEbus controller (DGA-001) library */

/* Copyright 1994-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01o,07dec97,hk   added INTC control for DVE7043 in sysMailboxEnable().
01n,12jul97,hk   changed inclusion of multi/dga001.h to vme/dga001Vme.h.
01m,27apr97,hk   doc: cleanup.
01l,22jan96,hk   made sysMailboxEnable() generic for dve7604/dve7043.
01k,24nov95,hk   supplemented documentation.
01j,10nov95,hk   followed dga001.h 01j.
01i,09nov95,hk   followed dga001.h 01i.
01h,08nov95,hk   renamed as dga001Vme.c, it was originally dga001.c.
01g,08nov95,hk   changed dga001.h directory to drv/multi/.
01f,08nov95,hk   changed to use INT_VEC_MBOX, made CSR24 init code safer.
01e,14jun95,hk   code review.
01d,08jun95,hk   changed sysMailboxEnable() for CSR20 header mod.
01c,28feb95,hk   changed sysMailboxConnect() to conform the ivSh.h 01e rule.
01b,08feb95,sa   just modified comment.
01a,15dec94,sa   created for sysIntDisable(), sysIntEnable(), sysBusIntAck(),
		 sysBusIntGen(), sysMailboxInt(), sysMailboxConnect(), 
	         sysMailboxEnable().
*/

/*
DESCRIPTION
This library contains routines which relate to the 'Densan' VMEbus 
controller (DGA-001).

The functions addressed here include:

    Bus interrupt functions:
	- enable/disable VMEbus interrupt levels
	- generate bus interrupts

    Mailbox/locations monitor functions:
	- enable mailbox/location monitor interrupts
*/

#include "drv/vme/dga001Vme.h"

LOCAL FUNCPTR	sysMailboxRoutine = NULL;
LOCAL int	sysMailboxArg     = NULL;

/*******************************************************************************
*
* sysIntDisable - disable a bus interrupt level
*
* This routine disables a specified VMEbus interrupt level.
*
* NOTE: This routine is currently not called from VxWorks drivers.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 1 - 7.
*
* SEE ALSO: sysIntEnable()
*/

STATUS sysIntDisable
    (
    int intLevel	/* interrupt level to disable (1-7) */
    )
    {
    if (intLevel < 1 || intLevel > 7)
	return (ERROR);

    *DGA_CSR21 &= (UINT32) ~(1 << intLevel);

    return (OK);
    }

/*******************************************************************************
*
* sysIntEnable - enable a bus interrupt level
*
* This routine enables a specified VMEbus interrupt level.
*
* NOTE: This routine is currently called from if_bp, if_egl, if_enp, if_ex,
* and smUtilLib.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 1 - 7.
*
* SEE ALSO: sysIntDisable()
*/

STATUS sysIntEnable
    (
    int intLevel	/* interrupt level to enable (1-7) */
    )
    {
    if (intLevel < 1 || intLevel > 7)
	return (ERROR);

    *DGA_CSR21 |= (UINT32)(1 << intLevel);

    return (OK);
    }

/*******************************************************************************
*
* sysBusIntAck - acknowledge a bus interrupt
*
* This routine acknowledges a specified VMEbus interrupt level.
*
* NOTE: This routine has no effect, since VMEbus interrupts are acknowledged
* automatically by hardware if the interrupt level is enabled.  This routine
* is currently called from if_bp, if_egl, if_enp, if_ex, and smNetLib.
*
* RETURNS: NULL.
*
* SEE ALSO: sysBusIntGen()
*/

int sysBusIntAck
    (
    int intLevel	/* interrupt level to acknowledge */
    )
    {
    return (NULL);
    }

/*******************************************************************************
*
* sysBusIntGen - generate a bus interrupt
*
* This routine generates a VMEbus interrupt for a specified interrupt level.
*
* RETURNS: OK, or ERROR if <level> or <vector> are out of range or
* a previous interrupt is not acknowledged.
*
* SEE ALSO: sysBusIntAck()
*/
 
STATUS sysBusIntGen
    (
    int level,		/* interrupt level to generate    */
    int vector		/* interrupt vector for interrupt */
    )
    {
    if (level < 1 || level > 7 || vector > 255 || vector < 2)
	return (ERROR);

    if (*DGA_CSR18_2 != 0)	/* previous interrupt is not acknowledged */
	return (ERROR);

    *DGA_CSR18_3 = (UINT8)vector;
    *DGA_CSR18_2 = (UINT8)level;

    return (OK);
    }

/*******************************************************************************
*
* sysMailboxInt - handle a mailbox interrupt request from DGA-001
*
* NOMANUAL
*/

LOCAL void sysMailboxInt (void)
    {
    *DGA_IFR0   = (UINT32)IFR0_SRQF;		/* clear SRQ       */
    *DGA_CSR23 |= (UINT32)CSR23_SRQICL;		/* clear interrupt */

    if (sysMailboxRoutine != NULL)
	sysMailboxRoutine (sysMailboxArg);
    }

/*******************************************************************************
*
* sysMailboxConnect - connect a routine to the mailbox interrupt
*
* This routine specifies the interrupt serivce routine to be called at each
* mailbox interrupt.  The DGA-001 SRQ (service request) interrupt is used to
* implement the mailbox interrupt.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysMailboxEnable()
*/

STATUS sysMailboxConnect
    (
    FUNCPTR routine,	/* routine called at each mailbox interrupt */
    int arg		/* argument with which to call routine      */
    )
    {
    static BOOL sysMailboxConnected = FALSE;

    if (!sysMailboxConnected &&
	intConnect (INT_VEC_MBOX, sysMailboxInt, NULL) == ERROR)
	{
	return (ERROR);
	}

    sysMailboxConnected	= TRUE;
    sysMailboxRoutine	= routine;
    sysMailboxArg	= arg;

    return (OK);
    }

/*******************************************************************************
*
* sysMailboxEnable - enable the mailbox interrupt
*
* This routine enables the mailbox interrupt.  The DGA-001 SRQ (service request)
* interrupt is used to implement the mailbox interrupt.  The SRQ interrupt is
* driven by accessing to a DGA-001 IFR (interface register) across VMEbus.
*
* NOTE: This routine has an argument <mailboxAdrs> to specify an address of
* mailbox, but the address of DGA-001 IFR (interface register) is pre-configured
* in sysProcNumSet() as a function of processor number; thus <mailboxAdrs> is
* ignored at here.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysMailboxConnect()
*/

STATUS sysMailboxEnable
    (
    char *mailboxAdrs		/* address of mailbox (ignored) */
    )
    {
#ifdef TARGET_DVE7043
    *INTC_IPRA = (UINT16)((*INTC_IPRA & 0x0fff) | ((INT_LVL_MBOX & 0xf) << 12));
#endif

    *DGA_CSR21 |= (UINT32)CSR21_SRQIEN;		/* enable interrupt */

    return (OK);
    }
