/* sun4Vme.c - Sun VMEbus library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,04jan97,p_m  changed sysBusTasClear back to sysBusClearTas for backward
                 compatibility
01d,11jun96,wlf  doc: cleanup; changed sysBusClearTas() to sysBusTasClear().
01c,15oct92,jwt  cleaned up ANSI compiler warnings.
01b,25sep92,ccc  fixed warning.
01a,17aug92,ccc  created by copying routines from sun1e sysLib.c.
*/

/*
DESCRIPTION
This library contains routines which relate to the VMEbus functions of the
Sun VMEbus gate array.

The functions addressed here include:

    Bus interrupt functions:
	- enable/disable VMEbus interrupt levels
	- generate bus interrupts

    Mailbox/locations monitor functions:
	- enable mailbox/location monitor interrupts

*/

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
    if (intLevel < 1 || intLevel > 7)
	return (ERROR);

    *SUN_BUS_IR &= ~(0x01 << intLevel);
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

    *SUN_BUS_IR |= (0x01 << intLevel);
    return (OK);
    }
 
/********************************************************************************
* sysBusIntAck - acknowledge a bus interrupt
*
* This routine acknowledges a specified VMEbus interrupt level.
*
* RETURNS: The vector put on the bus by the interrupting device.
*
* SEE ALSO: sysBusIntGen()
*/
 
int sysBusIntAck
    (
    int intLevel       /* interrupt level to acknowledge */
    )
    {
    return ((int) (SUN_ACK [intLevel << 1]));
    }

/*******************************************************************************
*
* sysBusIntGen - generate a bus interrupt
*
* This routine generates a VMEbus interrupt for a specified level with a
* specified vector.
*
* NOTE: This routine has no effect, since the CPU board cannot generate a 
* VMEbus interrupt.
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

/**************************************************************************
*
* sysBusTas - test and set a location across the bus
*
* This routine performs an atomic SPARC test-and-set (TAS) instruction 
* across a backplane.
*
* Read-modify-write cycles to DRAM can be made non-atomic by a VMEbus
* access.  In that case, this routine locks out VMEbus access during the
* call to vxTas().  For VMEbus accesses, vxTas() is called directly.
*
* NOTE: This routine is equivalent to vxTas(), since there is no VMEbus.
*
* RETURNS: TRUE if the value had not been set but is now,
* or FALSE if the value was set already.
*
* SEE ALSO: vxTas()
*/
 
BOOL sysBusTas
    (
    char *      addr                    /* address to be tested and set */
    )
    {
    int  oldLevel;                      /* Lock interrupts for DRAM TAS */
    BOOL status;                        /* vxTas() return value */
 
    if (((void *) addr >= (void *) LOCAL_MEM_LOCAL_ADRS) &&
        ((void *) addr < (void *) (LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE)))
        {
        oldLevel = intLock ();          /* Lock interrupts */

        *SUN_BUS_LOCK = BUS_LOCK_REQUEST;
        while ((*SUN_BUS_LOCK & BUS_LOCK_GRANT) != BUS_LOCK_GRANT);
 
        status = vxTas (addr);          /* Local RMW cycle */
 
        *SUN_BUS_LOCK = BUS_LOCK_RELEASE;
 
        intUnlock (oldLevel);           /* Restore interrupt level */
        }
    else
        {
        status = vxTas (addr);          /* VMEbus RMW cycle */
        }
 
    return (status);
    }
 
/**************************************************************************
*
* sysBusClearTas - clear a location set by sysBusTas()
*
* This routine clears the specified location if it has been set by a
* call to sysBusTas(). 
*
* RETURNS: N/A
*
* SEE ALSO: sysBusTas()
*/

void sysBusClearTas
    (
    char *      addr                    /* address to be cleared */
    )
    {
    *addr = 0;                          /* clear byte set by LDSTUB */
    }

/*******************************************************************************
*
* sysMailboxInt - handle a mailbox interrupt
*
* This routine handles the mailbox interrupt.  It is attached to the mailbox
* interrupt vector by the routine sysMailboxConnect().  The appropriate
* routine is called and the interrupts are acknowledged.
*
* RETURNS: N/A
*
* SEE ALSO: sysMailboxConnect()
*/

LOCAL void sysMailboxInt (void)
    {
    char mboxReg = *SUN_MBOX;

    if (((mboxReg & MBOX_INT_PENDING) == MBOX_INT_PENDING) &&
        (sysMailboxRoutine != NULL))
        (*(FUNCPTR) sysMailboxRoutine) (sysMailboxArg);
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
    if (!sysMailboxConnected)
        if (intConnect ((VOIDFUNCPTR *) INUM_TO_IVEC (INT_VEC_MBOX),
                    (VOIDFUNCPTR) sysMailboxInt, 0) == ERROR)
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
* NOTE: All bits, except bits 1 - 3, 14, and 15, must be zero for the
* mailbox address.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysMailboxConnect()
*/

STATUS sysMailboxEnable
    (
    INT8 *mailboxAdrs   /* address of mailbox (ignored) */
    )
    {
    static char dummy = 0;      /* force compiler to access location */
    int ix;

    dummy = *SUN_MBOX;          /* clear interrupt */

    /* Enable interrupt at the location */

    ix = (int) mailboxAdrs;
    *SUN_MBOX = 0x40 | ((ix >> 11) & 0x18) | ((ix >> 1) & 0x07);

    return (OK);
    }
