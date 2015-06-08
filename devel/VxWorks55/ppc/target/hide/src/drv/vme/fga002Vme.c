/* fga002Vme.c - Force Computers FGA-002 gate array VMEbus library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01b,21jun96,wlf  doc: cleanup.
01a,13jul92,caf  created by copying routines from frc30 sysLib.c, ansified.
*/

/*
DESCRIPTION
This library contains routines which relate to the VMEbus functions of the
Force Computers FGA-002 gate array.

The functions addressed here include:

    Bus interrupt functions:
	- enable/disable VMEbus interrupt levels
	- generate bus interrupts

    Mailbox/locations monitor functions:
	- enable mailbox/location monitor interrupts

*/

/* includes */

#include "drv/multi/fga002.h"

/* locals */

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
    switch (intLevel)
        {
        case 1:
            *FGA_ICRVME1 &= ~FGA_ICR_ENABLE;
            break;
        case 2:
            *FGA_ICRVME2 &= ~FGA_ICR_ENABLE;
            break;
        case 3:
            *FGA_ICRVME3 &= ~FGA_ICR_ENABLE;
            break;
        case 4:
            *FGA_ICRVME4 &= ~FGA_ICR_ENABLE;
            break;
        case 5:
            *FGA_ICRVME5 &= ~FGA_ICR_ENABLE;
            break;
        case 6:
            *FGA_ICRVME6 &= ~FGA_ICR_ENABLE;
            break;
        case 7:
            *FGA_ICRVME7 &= ~FGA_ICR_ENABLE;
            break;
        default:
            return (ERROR);
        }

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
    switch (intLevel)
        {
        case 1:
            *FGA_ICRVME1 |= FGA_ICR_ENABLE;
            break;
        case 2:
            *FGA_ICRVME2 |= FGA_ICR_ENABLE;
            break;
        case 3:
            *FGA_ICRVME3 |= FGA_ICR_ENABLE;
            break;
        case 4:
            *FGA_ICRVME4 |= FGA_ICR_ENABLE;
            break;
        case 5:
            *FGA_ICRVME5 |= FGA_ICR_ENABLE;
            break;
        case 6:
            *FGA_ICRVME6 |= FGA_ICR_ENABLE;
            break;
        case 7:
            *FGA_ICRVME7 |= FGA_ICR_ENABLE;
            break;
        default:
            return (ERROR);
        }
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

/*******************************************************************************
*
* sysBusIntGen - generate a bus interrupt
*
* This routine generates a bus interrupt for a specified level with a specified
* vector.
*
* NOTE: This routine has no effect, since the hardware cannot generate a VMEbus
* interrupt.
*
* RETURNS: ERROR, always.
*
* SEE ALSO: sysBusIntAck()
*/
 
STATUS sysBusIntGen
    (
    int  level,        /* VMEbus interrupt level to generate (1-7) */
    int  vector        /* interrupt vector to generate (0-255)     */
    )
    {
    return (ERROR);
    }

/*******************************************************************************
*
* sysMailboxInt - handle a mailbox interrupt
*
* This routine handles the interrupts associated with the FGA-002 MBOX0.
*
* RETURNS: N/A
*
* SEE ALSO: sysMailboxConnect()
*/

LOCAL void sysMailboxInt (void)
    {
    *FGA_MBOX0 = 0x0;                           /* acknowledge interrupt */

    if (sysMailboxRoutine != NULL)
        (* sysMailboxRoutine) (sysMailboxArg);  /* call mailbox routine */
    }

/********************************************************************************
* sysMailboxConnect - connect a routine to the mailbox interrupt
*
* This routine specifies the interrupt service routine to be called at each
* mailbox interrupt.
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

/*******************************************************************************
*
* sysMailboxEnable - enable the mailbox interrupt
*
* This routine enables the first mailbox of the FGA-002 chip.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysMailboxConnect()
*/

STATUS sysMailboxEnable
    (
    char *mailboxAdrs   /* address of mailbox (ignored) */
    )
    {
    *FGA_ICRMBOX0 |= FGA_ICR_ENABLE;

    return (OK);
    }
