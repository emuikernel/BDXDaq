/* pcc2Vme.c - Peripheral Channel Controller 2 (PCC2) VME interface library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,17dec96,mas  LM_SIG_LM3 -> LM_SIG_SIG3 in sysMailboxInt() (SPR 3292).
01b,18jun96,wlf  doc: cleanup.
01a,11jun92,ccc  created by copying routines from mv167 sysLib.c, ANSIfied.
*/

/*
DESCRIPTION
This library contains routines which relate to the Peripheral Channel
Controller 2 (PCC2).

The functions addressed here include:

    Bus interrupt functions:
	- enable/disable VMEbus interrupt levels
	- generate bus interrupts

    Mailbox/locations monitor functions:
	- enable mailbox/location monitor interrupts

*/

#include "drv/vme/vmechip2.h"

LOCAL FUNCPTR sysMailboxRoutine  = NULL;
LOCAL int sysMailboxArg          = NULL;

/*******************************************************************************
*
* sysIntDisable - disable a bus interrupt level
*
* This routine disables a specified VMEbus interrupt level.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 1 - 7.
*
* SEE ALSO: sysIntEnable()
*/

STATUS sysIntDisable
    (
    int intLevel        /* interrupt level to disable (1-7) */
    )
    {
    if (intLevel < 1 || intLevel > 7)
        return (ERROR);

    *VMECHIP2_LBIER &= ~(1 << (intLevel - 1));

    return (OK);
    }
/*******************************************************************************
*
* sysIntEnable - enable a bus interrupt level
*
* This routine enables a specified VMEbus interrupt level.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 1 - 7.
*
* SEE ALSO: sysIntDisable()
*/

STATUS sysIntEnable
    (
    int intLevel        /* interrupt level to enable (1-7) */
    )
    {
    if (intLevel < 1 || intLevel > 7)
        return (ERROR);

    *VMECHIP2_LBIER |= (1 << (intLevel - 1));

    return (OK);
    }
/*******************************************************************************
*
* sysBusIntAck - acknowledge a bus interrupt
*
* This routine acknowledges a specified VMEbus interrupt level.
*
* NOTE: This routine has no effect, since VMEbus interrupts are acknowledged
* automatically by hardware if the interrupt level is enabled.
*
* RETURNS: NULL.
*
* SEE ALSO: sysBusIntGen()
*/

int sysBusIntAck
    (
    int intLevel        /* interrupt level to acknowledge */
    )
    {
    return (NULL);
    }
/*******************************************************************************
*
* sysBusIntGen - generate a bus interrupt
*
* This routine generates a VMEbus interrupt for a specified level with a
* specified vector.
*
* RETURNS: OK, or ERROR if <level> or <vector> are out of range or the board
* cannot generate a bus interrupt.
*
* SEE ALSO: sysBusIntAck()
*/
 
STATUS sysBusIntGen
    (
    int level,		/* interrupt level to generate (1-7)      */
    int vector		/* interrupt vector for interrupt (2-255) */
    )
    {
    if (level < 1 || level > 7 || vector > 255 || vector < 2)
        return (ERROR);
 
    *VMECHIP2_ICR       = (vector << 16 |
                           level << 24);
    return (OK);
    }
/*******************************************************************************
*
* sysMailboxInt - handle mailbox interrupt
*
* Mailbox interrupts must be acknowledged.
*/

LOCAL void sysMailboxInt (void)

    {
    *VC2GCSR_LM_SIG = LM_SIG_SIG3;      /* clear signal */
    *VMECHIP2_ICLR  = ICLR_CSIG3;       /* clear IRQ */

    if (sysMailboxRoutine != NULL)
        sysMailboxRoutine (sysMailboxArg);
    }
/*******************************************************************************
*
* sysMailboxConnect - connect a routine to the mailbox interrupt
*
* This routine specifies the interrupt servce routine to be called at each
* mailbox interrupt.
*
* NOTE: The mailbox interrupt is GCSR SIG3.
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysMailboxEnable()
*/

STATUS sysMailboxConnect
    (
    FUNCPTR routine,    /* routine called at each mailbox interrupt */
    int arg             /* argument with which to call routine      */
    )
    {
    static BOOL sysMailboxConnected = FALSE;

    if (!sysMailboxConnected &&
        intConnect (INUM_TO_IVEC (UTIL_INT_VEC_BASE0 + LBIV_GCSR_SIG3),
                        sysMailboxInt, NULL) == ERROR)
        {
        return (ERROR);
        }

    sysMailboxConnected = TRUE;
    sysMailboxRoutine   = routine;
    sysMailboxArg       = arg;

    return (OK);
    }
/*******************************************************************************
*
* sysMailboxEnable - enable the mailbox interrupt
*
* This routine enables the mailbox interrupt.
*
* NOTE: The mailbox interrupt is GCSR SIG3.  The address of the register
* used to set SIG3 is configured as a function of the processor number and
* is set in sysProcNumSet(); thus <mailboxAdrs> is ignored.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysMailboxConnect()
*/

STATUS sysMailboxEnable
    (
    char *mailboxAdrs           /* address of mailbox (ignored) */
    )
    {
    *VMECHIP2_LBTVCR |= (sysProcNumGet() << 20);
    *VMECHIP2_ILR2  |= ILR2_SIG3_LEVEL3;        /* set to level 3 */
    *VMECHIP2_LBIER |= LBIER_ESIG3;     /* enable GCSR SIG3 interrupt */

    return (OK);
    }
