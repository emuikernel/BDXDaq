/* pccVme.c - Peripheral Channel Controller (PCC) library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01d,12jun96,wlf  more doc: fix ClearCase comment, cleanup.
01c,06jun96,wlf  doc: cleanup.
01b,25sep92,ccc  acknowledge SIGHP interrupt before calling MailboxRoutine.
01a,11jun92,ccc  created by copying routines from mv147 sysLib.c, ansified.
*/

/*
DESCRIPTION
This library contains routines which relate to the Peripheral Channel
Controller (PCC).

The functions addressed here include:

    Bus interrupt functions:
	- enable/disable VMEbus interrupt levels
	- generate bus interrupts

    Mailbox/locations monitor functions:
	- enable mailbox/location monitor interrupts

*/

#include "drv/multi/pccchip.h"

LOCAL FUNCPTR sysMailboxRoutine  = NULL;
LOCAL int sysMailboxArg          = NULL;
LOCAL BOOL sysMailboxConnected   = FALSE;

/********************************************************************************
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
    int intLevel       /* interrupt level to disable (1-7) */
    )
    {
    if (intLevel < 1 || intLevel > 7)
        return (ERROR);
 
    *LCSR_INT_MASK &= ~(1 << intLevel);
 
    return (OK);
    }

/********************************************************************************
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
    int intLevel       /* interrupt level to enable (1-7) */
    )
    {
    if (intLevel < 1 || intLevel > 7)
        return (ERROR);
 
    *LCSR_INT_MASK |= (1 << intLevel);
 
    return (OK);
    }
 
/********************************************************************************
* sysBusIntAck - acknowledge a bus interrupt
*
* This routine acknowledges a specified VMEbus interrupt level.
*
* NOTE: This routine has no effect, since the hardware acknowledges VMEbus
* interrupts automatically if the interrupt level is enabled.
*
* RETURNS: NULL.
*
* SEE ALSO: sysBusIntGen()
*/
 
int sysBusIntAck
    (
    int intLevel       /* interrupt level to acknowledge */
    )
    {
    return (NULL);
    }

/********************************************************************************
* sysBusIntGen - generate a bus interrupt
*
* This routine generates a VMEbus interrupt for a specified level with a
* specified vector.
*
* RETURNS: OK, or ERROR if <level> is out of range or the board cannot
* generate bus interrupt.
*
* SEE ALSO: sysBusIntAck()
*/
 
STATUS sysBusIntGen
    (
    int  level,        /* VMEbus interrupt level to generate (1-7) */
    int  vector        /* interrupt vector to generate (0-255)     */
    )
    {
    *LCSR_INT_STATUS_ID = (char) vector;
    *LCSR_INT_REQ       = (char) level; /* check if level between 1 & 7? */
 
    return (OK);
    }

/********************************************************************************
* sysMailboxInt - handle mailbox interrupt
*
* Mailbox interrupts must be acknowledged.
*/
 
LOCAL void sysMailboxInt (void)
 
    {
    /* acknowledge SIGHP interrupt */
    *GCSR_GLOBAL_1 |= GLOBAL_1_SIGHP;

    if (sysMailboxRoutine != NULL)
        sysMailboxRoutine (sysMailboxArg);
    }

/********************************************************************************
* sysMailboxConnect - connect a routine to the mailbox interrupt
*
* This routine specifies the interrupt service routine to be called at each
* mailbox interrupt.
*
* NOTE: The mailbox interrupt is assigned as signal high priority (SIGHP).
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysMailboxEnable()
*/
 
STATUS sysMailboxConnect
    (
    FUNCPTR routine,   /* routine called at each mailbox interrupt */
    int arg            /* argument with which to call routine      */
    )
    {
 
    sysMailboxConnected = TRUE;
    sysMailboxRoutine   = routine;
    sysMailboxArg       = arg;
 
    return (OK);
    }

/********************************************************************************
* sysMailboxEnable - enable the mailbox interrupt
*
* This routine enables the mailbox interrupt.
*
* NOTE:  The mailbox interrupt is assigned as signal high priority (SIGHP).
* The address of the register used to set SIGHP is configured as a function
* of the processor number and is set in sysProcNumSet(); thus <mailboxAdrs>
* is ignored.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysMailboxConnect()
*/
 
STATUS sysMailboxEnable
    (
    char *mailboxAdrs          /* address of mailbox (ignored) */
    )
    {
    /* unmask SIGHP interrupt */
 
    *LCSR_UTIL_INT_MASK |= UTIL_INT_MASK_SIGHEN;
 
    return (OK);
    }
