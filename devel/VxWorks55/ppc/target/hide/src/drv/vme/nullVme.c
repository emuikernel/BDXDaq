/* nullVme.c - null VMEbus library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,18jun01,mil  Cleaned up compiler warnings.
01g,03jan97,wlf  doc: more cleanup.
01f,04jun96,wlf  doc: cleanup.
01e,30may96,wlf  doc: cleanup.
01d,21may96,dat  documentation
01c,15sep93,hdn  added a title of sysMailboxConnect () for documentation.
01b,18aug93,vin	 fixed title to sysMailboxConnect which causes error
		 when generating man pages.
01a,20aug92,ccc  created by copying routines from mb930 sysLib.c.
*/

/*
DESCRIPTION
This library contains null routines for boards which do not include any
common bus routines.

The functions addressed here include:

    Bus interrupt functions:
	- enable/disable VMEbus interrupt levels
	- generate bus interrupts

    Mailbox/locations monitor functions:
	- enable mailbox/location monitor interrupts

*/

/******************************************************************************
*
* sysLocalToBusAdrs - convert a local address to a bus address
*
* This routine gets the VMEbus address that accesses a specified local
* memory address.
*
* NOTE: This routine has no effect, since there is no VMEbus.
*
* RETURNS: ERROR, always.
*
* SEE ALSO: sysBusToLocalAdrs()
*/

STATUS sysLocalToBusAdrs
    (
    int  adrsSpace,     /* bus address space in which busAdrs resides,  */
                        /* use address modifier codes defined in vme.h, */
                        /* such as VME_AM_STD_SUP_DATA                  */
    char *localAdrs,    /* local address to convert                     */
    char **pBusAdrs     /* where to return bus address                  */
    )
    {
    return (ERROR);
    }

/******************************************************************************
*
* sysBusToLocalAdrs - convert a bus address to a local address
*
* This routine gets the local address that accesses a specified VMEbus
* address.
*
* NOTE: This routine has no effect, since there is no VMEbus.
*
* RETURNS: ERROR, always.
*
* SEE ALSO: sysLocalToBusAdrs()
*/
 
STATUS sysBusToLocalAdrs
    (
    int  adrsSpace,     /* bus address space in which busAdrs resides,  */
                        /* use address modifier codes defined in vme.h, */
                        /* such as VME_AM_STD_SUP_DATA                  */
    char *busAdrs,      /* bus address to convert                       */
    char **pLocalAdrs   /* where to return local address                */
    )
    {
    return (ERROR);
    }

/******************************************************************************
*
* sysBusIntAck - acknowledge a bus interrupt
*
* This routine acknowledges a specified VMEbus interrupt level.
*
* NOTE: This routine has no effect, since there is no VMEbus.
*
* RETURNS: NULL.
*/

int sysBusIntAck
    (
    int intLevel        /* interrupt level to acknowledge */
    )
    {
    return (0);
    }

/******************************************************************************
*
* sysBusIntGen - generate a bus interrupt
*
* This routine generates a VMEbus interrupt for a specified level with a
* specified vector.
*
* NOTE: This routine has no effect, since there is no VMEbus.
*
* RETURNS: ERROR, always.
*/

STATUS sysBusIntGen
    (
    int level,          /* bus interrupt level to generate          */
    int vector          /* interrupt vector to return (0-255)       */
    )
    {
    return (ERROR);
    }

/******************************************************************************
*
* sysMailboxConnect - connect a routine to the mailbox interrupt
*
* This routine specifies the interrupt service routine to be called at each
* mailbox interrupt.
*
* NOTE: This routine has no effect, since the hardware does not support mailbox
* interrupts.
*
* RETURNS: ERROR, always.
*
* SEE ALSO: sysMailboxEnable()
*/

STATUS sysMailboxConnect
    (
    FUNCPTR routine,    /* routine called at each mailbox interrupt */
    int     arg         /* argument with which to call routine      */
    )
    {
    return (ERROR);
    }

/******************************************************************************
*
* sysMailboxEnable - enable the mailbox interrupt
*
* This routine enables the mailbox interrupt.
*
* NOTE: This routine has no effect, since the hardware does not support mailbox
* interrupts.
*
* RETURNS: ERROR, always.
*
* SEE ALSO: sysMailboxConnect()
*/

STATUS sysMailboxEnable
    (
    INT8 *mailboxAdrs           /* mailbox address */
    )
    {
    return (ERROR);
    }

/********************************************************************************
* sysBusTas - test and set a location across the bus
*
* This routine performs a test-and-set (TAS) instruction across the backplane.
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
    INT8 *addr          /* address to be tested and set */
    )
    {
    return(vxTas(addr));       /* RMW cycle */
    }

